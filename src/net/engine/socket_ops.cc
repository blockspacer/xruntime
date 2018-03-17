// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "net/engine/socket_ops.h"

#include "base/logging.h"
#include "base/strings/stringprintf.h"

#if defined(OS_WIN)
#include <ws2tcpip.h>
#include "base/win/win_util.h"
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <setjmp.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#endif

namespace net {

#if defined(OS_WIN)
namespace win_util {

std::string FormatMessageA(unsigned messageid) {
  char* string_buffer = NULL;
  unsigned string_length = ::FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS, NULL, messageid, 0,
    reinterpret_cast<char *>(&string_buffer), 0, NULL);

  std::string formatted_string;
  if (string_buffer) {
    if (string_length > 1) {
      formatted_string.assign(string_buffer, string_length - 1);
    }
    else {
      formatted_string = string_buffer;
    }

    LocalFree(reinterpret_cast<HLOCAL>(string_buffer));
  }
  else {
    // The formating failed. simply convert the message value into a string.
    base::SStringPrintf(&formatted_string, "message number %d", messageid);
  }
  return formatted_string;
}

}

std::string FormatLastWSAErrorA(unsigned messageid) {
  return win_util::FormatMessageA(messageid);
}

std::string FormatLastWSAErrorA() {
  return win_util::FormatMessageA(WSAGetLastError());
}
#endif

// used to ensure we delete the addrinfo structure
// alloc'd by getaddrinfo
class ScopedAddrinfo {
protected:
  struct addrinfo * addrinfo_ptr_;
public:

  explicit ScopedAddrinfo(struct addrinfo* addrinfo_ptr) :
    addrinfo_ptr_(addrinfo_ptr) {}

  ~ScopedAddrinfo() {
    freeaddrinfo(addrinfo_ptr_);
  }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

  // Sets an FD to be nonblocking.
  void SetNonBlocking(SOCKET s) {
#if defined(OS_WIN)
    unsigned long no_block = 1;
    int ioctl_return = ioctlsocket(s, FIONBIO, &no_block);
    DCHECK_GE(ioctl_return, 0)
      << "error doing ioctlsocket(s, FIONBIO, &no_block) fd: " << s
      << " errno=" << WSAGetLastError();

#elif defined(OS_POSIX)
    DCHECK_GE(s, 0);
    int fcntl_return = fcntl(s, F_GETFL, 0);
    CHECK_NE(fcntl_return, -1)
      << "error doing fcntl(fd, F_GETFL, 0) fd: " << s
      << " errno=" << errno;

    if (fcntl_return & O_NONBLOCK)
      return;

    fcntl_return = fcntl(s, F_SETFL, fcntl_return | O_NONBLOCK);
    CHECK_NE(fcntl_return, -1)
      << "error doing fcntl(fd, F_SETFL, fcntl_return) fd: " << s
      << " errno=" << errno;
#endif
  }

}

int SetDisableNagle(SOCKET s) {
  int on = 1;
  int rc;
  rc = setsockopt(s, IPPROTO_TCP, TCP_NODELAY,
    reinterpret_cast<char*>(&on), sizeof(on));
  if (rc < 0) {
#if defined(OS_WIN)
    closesocket(s);
#elif defined(OS_POSIX)
    close(s);
#endif
    LOG(FATAL) << "setsockopt() TCP_NODELAY: failed on socket " << s;
    return 0;
  }
  return 1;
}

// Summary:
//   Closes a socket, with option to attempt it multiple times.
//   Why do this? Well, if the system-call gets interrupted, close
//   can fail with EINTR. In that case you should just retry.. Unfortunately,
//   we can't be sure that errno is properly set since we're using a
//   multithreaded approach in the filter proxy, so we should just retry.
// Args:
//   fd - the socket to close
//   tries - the number of tries to close the socket.
// Returns:
//   true - if socket was closed
//   false - if socket was NOT closed.
// Side-effects:
//   sets *fd to -1 if socket was closed.
//
bool CloseSocket(SOCKET *s, int tries) {
  for (int i = 0; i < tries; ++i) {
#if defined(OS_WIN)
    if (!closesocket(*s)) {
      *s = kInvalidSocket;
      return true;
    }
#elif defined(OS_POSIX)
    if (!close(*s)) {
      *s = kInvalidSocket;
      return true;
    }
#endif
  }
  return false;
}

int CreateTCPListeningSocket(const std::string& ip, const std::string& port,
  bool is_numeric_host_address, SOCKET* listen_socket) {
  // start out by assuming things will fail.
  *listen_socket = kInvalidSocket;

  const char* node = NULL;
  const char* service = NULL;

  if (!ip.empty()) node = ip.c_str();
  if (!port.empty()) service = port.c_str();

  struct addrinfo *results = 0;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));

  if (is_numeric_host_address) {
    hints.ai_flags = AI_NUMERICHOST;  // iff you know the name is numeric.
  }
  hints.ai_flags |= AI_PASSIVE;

  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int err = 0;
  if ((err = getaddrinfo(node, service, &hints, &results))) {
    // gai_strerror -is- threadsafe, so we get to use it here.
    LOG(ERROR) << "getaddrinfo " << " for (" << ip << ":" << port
      << ") " << gai_strerror(err) << "\n";
    return -1;
  }
  // this will delete the addrinfo memory when we return from this function.
  ScopedAddrinfo addrinfo_guard(results);

  SOCKET sock = socket(results->ai_family,
    results->ai_socktype,
    results->ai_protocol);
  if (sock == kInvalidSocket) {

#if defined(OS_WIN)
    LOG(ERROR) << "Unable to create socket for (" << ip << ":"
      << port << "): " << win_util::FormatMessageA(WSAGetLastError()) << "\n";
#else
    LOG(ERROR) << "Unable to create socket for (" << ip << ":"
      << port << "): " << strerror(errno) << "\n";
#endif
    return -1;
  }

  if (true) {
    // set SO_REUSEADDR on the listening socket.
    int on = 1;
    int rc;
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
      reinterpret_cast<char *>(&on), sizeof(on));
    if (rc < 0) {
#if defined(OS_WIN)
      closesocket(sock);
#elif defined(OS_POSIX)
      close(sock);
#endif
      LOG(FATAL) << "setsockopt() SO_REUSEADDR failed socket=" << sock << "\n";
    }
  }

#ifndef SO_REUSEPORT
#define SO_REUSEPORT 15
#endif
  if (false) {
    // set SO_REUSEADDR on the listening socket.
    int on = 1;
    int rc;
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
      reinterpret_cast<char *>(&on), sizeof(on));
    if (rc < 0) {
#if defined(OS_WIN)
      closesocket(sock);
#elif defined(OS_POSIX)
      close(sock);
#endif
      LOG(FATAL) << "setsockopt() SO_REUSEPORT failed socket=" << sock << "\n";
    }
  }

  int result = bind(sock, results->ai_addr, results->ai_addrlen);
  if (result < 0) {
    // If we are waiting for the interface to be raised, such as in an
    // HA environment, ignore reporting any errors.

#if defined(OS_WIN)
    int saved_errno = WSAGetLastError();
    if (saved_errno != EADDRNOTAVAIL) {
      LOG(ERROR) << "Bind was unsuccessful for (" << ip << ":"
        << port << "): " << win_util::FormatMessageA(saved_errno) << "\n";
    }
#else
    int saved_errno = errno;
    if (errno != EADDRNOTAVAIL) {
      LOG(ERROR) << "Bind was unsuccessful for (" << ip << ":"
        << port << "): " << strerror(errno) << "\n";
    }
#endif
    // if we knew that we were not multithreaded, we could do the following:
    // " : " << strerror(errno) << "\n";
    if (CloseSocket(&sock, 100)) {
      if (saved_errno == EADDRNOTAVAIL) {
        return -3;
      }
      return -2;
    }
    else {
      // couldn't even close the dang socket?!
      LOG(ERROR) << "Unable to close the socket.. Considering this a fatal "
        "error, and exiting\n";
      exit(EXIT_FAILURE);
      return -1;
    }
  }

  if (!SetDisableNagle(sock)) {
    return -1;
  }

  int backlog = 1024;
  result = listen(sock, backlog);
  if (result < 0) {
    // listen was unsuccessful.

#if defined(OS_WIN)
    LOG(ERROR) << "Listen was unsuccessful for (" << ip << ":"
      << port << "): " << FormatLastWSAErrorA() << "\n";
    // if we knew that we were not multithreaded, we could do the following:
    // " : " << strerror(errno) << "\n";
#else
    LOG(ERROR) << "Listen was unsuccessful for (" << ip << ":"
      << port << "): " << strerror(errno) << "\n";
    // if we knew that we were not multithreaded, we could do the following:
    // " : " << strerror(errno) << "\n";
#endif

    if (CloseSocket(&sock, 100)) {
      sock = kInvalidSocket;
      return -1;
    }
    else {
      // couldn't even close the dang socket?!
      LOG(FATAL) << "Unable to close the socket.. Considering this a fatal "
        "error, and exiting\n";
    }
  }

  // If we've gotten to here, Yeay! Success!
  *listen_socket = sock;

  return 0;
}

int CreateTCPConnectedSocket(const std::string& ip, const std::string& port,
  bool is_numeric_host_address, SOCKET *connect_socket) {
  const char* node = NULL;
  const char* service = NULL;

  *connect_socket = kInvalidSocket;
  if (!ip.empty())
    node = ip.c_str();
  if (!port.empty())
    service = port.c_str();

  struct addrinfo *results = 0;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));

  if (is_numeric_host_address) {
    hints.ai_flags = AI_NUMERICHOST;  // iff you know the name is numeric.
  }
  //hints.ai_flags |= AI_PASSIVE;

  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int err = 0;
  if ((err = getaddrinfo(node, service, &hints, &results))) {
    // gai_strerror -is- threadsafe, so we get to use it here.
    LOG(ERROR) << "getaddrinfo for (" << ip << ":" << port << "): "
      << gai_strerror(err);
    return -1;
  }
  // this will delete the addrinfo memory when we return from this function.
  ScopedAddrinfo addrinfo_guard(results);


  //int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  int sock = socket(results->ai_family,
    results->ai_socktype,
    results->ai_protocol);
  if (sock == kInvalidSocket) {
#if defined(OS_WIN)
    LOG(ERROR) << "Unable to create socket for (" << ip << ":" << port
      << "): " << FormatLastWSAErrorA();
#else
    LOG(ERROR) << "Unable to create socket for (" << ip << ":" << port
      << "): " << strerror(errno);
#endif
    return -1;
  }

  SetNonBlocking(sock);
  if (!SetDisableNagle(sock)) {
    return -1;
  }

  int ret_val = 0;

  if (connect(sock, results->ai_addr, results->ai_addrlen) == -1) {
#if defined(OS_WIN)
    int err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK && err != WSAEINVAL && err != WSAEINPROGRESS) {
      LOG(ERROR) << "Connect was unsuccessful for (" << ip << ":" << port
        << "): " << FormatLastWSAErrorA();
      closesocket(sock);
      return -1;
    }
#else
    // if ( errno != EAGAIN && errno != EWOULDBLOCK ) {
    if (errno != EINPROGRESS) {
      LOG(ERROR) << "Connect was unsucessful for (" << ip << ":" << port
        << "): " << strerror(errno);
      close(sock);
      return -1;
    }
#endif
  }
  else {
    ret_val = 1;
  }

  /*
    // ������
    struct timeval timeout ;
    fd_set r;
    FD_ZERO(&r);
    FD_SET(sock, &r);

    timeout.tv_sec = 15; //���ӳ�ʱ15��
    timeout.tv_usec =0;
    int ret = select(0, 0, &r, 0, &timeout);
    if ( ret <= 0 ) {
      ::closesocket(sock);
      return -1;
    }
    ret_val = 1;
    */

    // SetNonBlocking( sock );

    // If we've gotten to here, Yeay! Success!
  *connect_socket = sock;

  return ret_val;
}

#define kDefaultConnTimeout	30

// ���ǵĻ�����̫����
// ��Ҫ��ϸ�ٿ���
bool CreateBlockingTCPConnectedSocket(const std::string& ip, const std::string& port,
  bool is_numeric_host_address, SOCKET *connect_socket) {
  *connect_socket = kInvalidSocket;

  SOCKET sock = kInvalidSocket;
  int ret_val = CreateTCPConnectedSocket(ip, port, is_numeric_host_address, &sock);
  if (ret_val == 0) {
    // �ȴ�����
    int rv;

    struct timeval tv;
    fd_set rd_wr, ex;

    int os_error = 0;
    socklen_t len = sizeof(os_error);

    tv.tv_sec = kDefaultConnTimeout; //���ӳ�ʱkDefaultConnTimeout��
    tv.tv_usec = 0;

    FD_ZERO(&rd_wr);
    FD_ZERO(&ex);

    FD_SET(sock, &rd_wr);
    FD_SET(sock, &ex);

#if defined(OS_WIN)
    rv = select(0, NULL, &rd_wr, &ex, &tv);
#else
    rv = select(0, NULL, &rd_wr, NULL, &tv);
#endif

    if (rv > 0) {
#if defined(OS_WIN)
      /*
      * Call Sleep(0) to work around a Winsock timing bug.
      */
      Sleep(0);
      if (FD_ISSET(sock, &ex)) {
        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&os_error, &len) == SOCKET_ERROR) {
          LOG(ERROR) << "Connect error " << FormatLastWSAErrorA();
          os_error = WSAGetLastError();
          return false;
        }
        if (os_error != 0) {
          LOG(ERROR) << "Connect error " << FormatLastWSAErrorA(os_error);
        }
        else {
          LOG(ERROR) << "Connect unknow error";
        }
        return false;
      }
      if (FD_ISSET(sock, &rd_wr)) {
        /* it's connected */
        *connect_socket = sock;
        return true;
      }
#else
      if (FD_ISSET(sock, &rd_wr)) {

        // Get the error that connect() completed with.
        int os_error = 0;

        if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&os_error, &len) < 0) {
          os_error = errno;
          LOG(ERROR) << "Connect host failed: " << strerror(os_error);
          return false;
        }

        if (os_error != 0) {
          LOG(ERROR) << "Connect host failed: " << strerror(os_error);
          return false;
        }

        /* it's connected */
        *connect_socket = sock;
        return true;
      }

#endif
    } if (rv == 0) {
#if defined(OS_WIN)
      LOG(ERROR) << "select error " << FormatLastWSAErrorA(WSAETIMEDOUT);
      closesocket(sock);
      return false;
#else
      LOG(ERROR) << "select error " << strerror(ETIMEDOUT);
      close(sock);
#endif
      return false;
    }
    else {
#if defined(OS_WIN)
      LOG(ERROR) << "select error " << FormatLastWSAErrorA();
      closesocket(sock);
#else
      LOG(ERROR) << "select error " << strerror(errno);
      close(sock);
#endif
      return false;
    }

    return true;
  }
  else if (ret_val == -1) {
    return false;
  }
  return true;
}

int Accept(SOCKET acceptor, SOCKET *accepted_socket) {
  *accepted_socket = kInvalidSocket;
  sockaddr_in from;
  socklen_t from_len = sizeof(from);
  SOCKET conn = accept(acceptor, reinterpret_cast<sockaddr*>(&from), &from_len);
  if (conn == kInvalidSocket) {
#if defined(OS_WIN)
#else
    if (errno != 11) {
      LOG(ERROR) << "Acceptor: accept fail("
        << acceptor << "): " << errno << ": "
        << strerror(errno);
    }
#endif
    return -1;
  }

  SetNonBlocking(conn);
  if (!SetDisableNagle(conn)) {
    return -1;
  }
  *accepted_socket = conn;
  return 0;
}


}


/*

http://bdxnote.blog.163.com/blog/static/844423520098651256549/
������connect

C/VC/C++ 2009-09-06 17:12:56 �Ķ�1511 ����0   �ֺţ�����С ����
��һ��TCP�׽ӿڱ�����Ϊ������֮�����connect,connect����������EINPROGRESS����,��ʾ���Ӳ������ڽ�����,������δ���;ͬʱTCP����·���ֲ�����������;����֮��,���ǿ��Ե���select�������������Ƿ����ɹ�;������connect��������;:
1.���ǿ�������·���ֵ�ͬʱ��һЩ�����Ĵ���.connect����Ҫ��һ������ʱ�����,���ҿ��������κεط�,�Ӽ�������ľ����������ٺ������Ĺ�����.�����ʱ�������ǿ�����һЩ�����Ĵ�����Ҫִ��;
2.���������ּ���ͬʱ�����������.��Web������к��ձ�;
3.��������ʹ��select���ȴ����ӵ����,������ǿ��Ը�select����һ��ʱ������,�Ӷ�����connect�ĳ�ʱʱ��.�ڴ����ʵ����,connect�ĳ�ʱʱ����75�뵽������֮��.��ʱ��Ӧ�ó�����Ҫһ�����̵ĳ�ʱʱ��,ʹ�÷�����connect����һ�ַ���;
������connect��������Ȼ��,������Ȼ��һЩϸ������Ҫ����:
1.��ʹ�׽ӿ��Ƿ�������,������ӵķ�������ͬһ̨������,��ô�ڵ���connect��������ʱ,����ͨ�������������ɹ�.���Ǳ��봦���������;
2.Դ��Berkeley��ʵ��(��Posix.1g)��������select�ͷ�����IO��صĹ���:
A:�����ӽ����ɹ�ʱ,�׽ӿ���������ɿ�д;
B:�����ӳ���ʱ,�׽ӿ���������ɼȿɶ��ֿ�д;
ע��:��һ���׽ӿڳ���ʱ,���ᱻselect���ñ��Ϊ�ȿɶ��ֿ�д;

������connect����ô��ô�,���Ǵ��������connectʱ�������ܶ����ֲ������;

���������connect�Ĳ���:
��һ��:����socket,�����׽ӿ�������;
�ڶ���:����fcntl���׽ӿ����������óɷ�����;
������:����connect��ʼ��������;
���Ĳ�:�ж������Ƿ�ɹ�����;
A:���connect����0,��ʾ���Ӽ�Ƴɹ�(�������ɿͻ�����ͬһ̨������ʱ���п��ܷ����������);
B:����select���ȴ����ӽ����ɹ����;
���select����0,���ʾ�������ӳ�ʱ;���Ƿ��س�ʱ������û�,ͬʱ�ر�����,�Է�ֹ��·���ֲ�������������ȥ;
���select���ش���0��ֵ,����Ҫ����׽ӿ��������Ƿ�ɶ����д;����׽ӿ��������ɶ����д,�����ǿ���ͨ������getsockopt���õ��׽ӿ��ϴ�����Ĵ���(SO_ERROR),������ӽ����ɹ�,�������ֵ����0,�����������ʱ��������,�����ֵ�����Ӵ�������Ӧ��errnoֵ(����:ECONNREFUSED,ETIMEDOUT��).
"��ȡ�׽ӿ��ϵĴ���"�������ĵ�һ������ֲ������;�����������,getsockoptԴ��Berkeley��ʵ���Ƿ���0,�ȴ�����Ĵ����ڱ���errno�з���;����Solaris����getsockopt����-1,errno��Ϊ������Ĵ���;���Ƕ������������Ҫ����;

����,�ڴ��������connectʱ,�ڲ�ͬ���׽ӿ�ʵ�ֵ�ƽ̨�д��ڵ���ֲ������,����,�п����ڵ���select֮ǰ,���Ӿ��Ѿ������ɹ�,���ҶԷ��������Ѿ�����.�����������,���ӳɹ�ʱ�׽ӿڽ��ȿɶ��ֿ�д.�������ʧ��ʱ��һ����.���ʱ�����ǻ���ͨ��getsockopt����ȡ����ֵ;���ǵڶ�������ֲ������;
��ֲ�������ܽ�:
1.���ڳ�����׽ӿ�������,getsockopt�ķ���ֵԴ��Berkeley��ʵ���Ƿ���0,������Ĵ���ֵ�洢��errno��;��Դ��Solaris��ʵ���Ƿ���0,������Ĵ���洢��errno��;(�׽ӿ�����������ʱ����getsockopt�ķ���ֵ������ֲ)
2.�п����ڵ���select֮ǰ,���Ӿ��Ѿ������ɹ�,���ҶԷ��������Ѿ�����,�����������,�׽ӿ��������Ǽȿɶ��ֿ�д;�����׽ӿ�����������ʱ��һ����;(�����ж������Ƿ����ɹ�������������ֲ)

�����Ļ�,�������ж������Ƿ����ɹ���������Ψһʱ,���ǿ��������µķ���������������:
1.����getpeername����getsockopt.�������getpeernameʧ��,getpeername����ENOTCONN,��ʾ���ӽ���ʧ��,���Ǳ�����SO_ERROR����getsockopt�õ��׽ӿ��������ϵĴ��������;
2.����read,��ȡ����Ϊ0�ֽڵ�����.���read����ʧ��,���ʾ���ӽ���ʧ��,����read���ص�errnoָ��������ʧ�ܵ�ԭ��.������ӽ����ɹ�,readӦ�÷���0;
3.�ٵ���һ��connect.��Ӧ��ʧ��,�������errno��EISCONN,�ͱ�ʾ�׽ӿ��Ѿ�����,���ҵ�һ�������ǳɹ���;����,���Ӿ���ʧ�ܵ�;

���жϵ�connect:
�����һ������ʽ�׽ӿ��ϵ���connect,��TCP����·���ֲ������֮ǰ���ж���,����˵,��������ź��ж�,���ᷢ��ʲô��?�ٶ�connect�����Զ�����,��������EINTR.��ô,���ʱ��,���ǾͲ����ٵ���connect�ȴ����ӽ��������,����ٴε���connect���ȴ����ӽ�����ɵĻ�,connect���᷵�ش���ֵEADDRINUSE.�����������,Ӧ�������ǵ���select,�����ڷ�����ʽconnect��������һ��.Ȼ��,select�����ӽ����ɹ�(ʹ�׽ӿ���������д)�����ӽ���ʧ��(ʹ�׽ӿ��������ȿɶ��ֿ�д)ʱ����;
*/

