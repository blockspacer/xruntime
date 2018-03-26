// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// Socket api �ļ򵥷�װ
// 

#ifndef NET_ENGINE_SOCKET_OPS_H_
#define NET_ENGINE_SOCKET_OPS_H_

#include <stdint.h>
#include <string>

#include "build/build_config.h"

#if defined(OS_WIN)
#include <winsock2.h>
const SOCKET kInvalidSocket = INVALID_SOCKET;
const int kSocketError = SOCKET_ERROR;
#elif defined(OS_POSIX)
typedef int SOCKET;
const SOCKET kInvalidSocket = -1;
const int kSocketError = -1;
#endif

namespace net {

enum WaitState {
  NOT_WAITING = 0,
  WAITING_ACCEPT = 1,
  WAITING_READ = 3,
  WAITING_WRITE = 4,
  WAITING_CLOSE = 5
};

int CreateTCPListeningSocket(const std::string& ip, const std::string& port,
  bool is_numeric_host_address, SOCKET *listen_socket);

// ����������
// ����ֵ��
// -1����������
//  1�����ӳɹ�
//  0����������
int CreateTCPConnectedSocket(const std::string& ip, const std::string& port,
  bool is_numeric_host_address, SOCKET *connect_socket);

// ������������
bool CreateBlockingTCPConnectedSocket(const std::string& ip,
  const std::string& port, bool is_numeric_host_address,
  SOCKET *connect_socket);

int Accept(SOCKET acceptor, SOCKET *accepted_socket);

#if defined(OS_WIN)
std::string FormatLastWSAErrorA(unsigned messageid);
#endif  // OS_WIN

}

#endif  // NET_ENGINE_SOCKET_OPS_H_
