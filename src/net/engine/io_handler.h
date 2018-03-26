// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef NET_ENGINE_IO_HANDLER_H_
#define NET_ENGINE_IO_HANDLER_H_

#include "base/time/time.h"

#include "net/base/io_buffer.h"
#include "net/engine/socket_ops.h"

#include "base/memory/ref_counted.h"
#include "base/atomic_sequence_num.h"

#include "net/message_loop/message_loop_for_net.h"

namespace net {

class IOHandler;
typedef scoped_refptr<IOHandler> IOHandlerPtr;

class IOHandler : 
  public base::RefCountedThreadSafe<IOHandler>,
  public net::MessageLoopForNet::Watcher {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}

    virtual int  OnNewConnection(const IOHandlerPtr& ih) { return 0; }
    virtual int  OnDataReceived(const IOHandlerPtr& ih, IOBuffer* data,
      base::Time receive_time) { return 0; }
    virtual int  OnConnectionClosed(const IOHandlerPtr& ih) { return 0; }
    virtual void OnConnectedError(const IOHandlerPtr& ih, int err_code) {}
  };

  IOHandler(base::MessageLoop* message_loop, SOCKET socket, Delegate* delegate);
  virtual ~IOHandler();

  inline SOCKET socket() { return socket_; }
  inline void SetIOHandlerDelegate(Delegate* delegate) {
    delegate_ = delegate;
  }

  void Create();

  bool SendData(net::IOBuffer* data);
  bool SendData(const char* data) {
    return SendData(base::StringPiece(data));
  }
  bool SendData(const void* data, uint32_t data_len);
  bool SendData(const base::StringPiece& data);

  virtual void OnCreated();
  virtual void Close( );
  virtual void Read(base::Time recv_time);
  virtual void SendInternal2(const std::string& data);
  virtual void SendInternal(const base::StringPiece& data);
  virtual void SendInternal(const void* data, uint32_t data_len);

  virtual void CloseSocket(SOCKET s);

  const std::string& GetAddressString() {
    return remote_string_;
  }
  inline int io_handler_id() const {
    return io_handler_id_;
  }

  void  SetIOContext(void* ctx) {
    context_ = ctx;
  }
  void* GetIOContext() {
    return context_;
  }
  void* ReleaseIOContext() {
    void* ctx = context_;
    context_ = NULL;
    return ctx;
  }

  // for lua
  bool SendStringPiece(base::StringPiece data);

protected:
  void OnWrite();

  bool IsWriting() const {
    return write_wait_state_ == WAITING_WRITE;
  }

  virtual void OnFileCanReadWithoutBlocking(int fd);
  virtual void OnFileCanWriteWithoutBlocking(int fd);

  WaitState read_wait_state_;
  WaitState write_wait_state_;

  // The socket's libevent wrapper
  net::MessageLoopForNet::FileDescriptorWatcher read_watcher_;
  net::MessageLoopForNet::FileDescriptorWatcher write_watcher_;

  SOCKET socket_;
  std::string remote_string_;

private:
  base::MessageLoop* message_loop_;
  Delegate* delegate_;

  IOBuffer read_buf_;
  IOBuffer write_buf_;

  int io_handler_id_;

  void* context_;

  static base::StaticAtomicSequenceNumber g_last_sequence_number_;
};

}

#endif
