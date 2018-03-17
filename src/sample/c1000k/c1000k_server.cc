// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "sample/c1000k/c1000k_server.h"

#include "base/at_exit.h"
#include "base/sys_info.h"
#include "base/strings/string_number_conversions.h"
#include "base/logging.h"
#include "base/command_line.h"

#if defined(OS_WIN)
#include "net/base/winsock_init.h"
#endif // OS_WIN

#include "net/message_loop/message_pump_net.h"

C1000kServer::C1000kServer()
  : Daemon() {
  message_loop_ = new base::MessageLoop(
    std::unique_ptr<base::MessagePump>(new net::MessagePumpNet()));
}

C1000kServer::~C1000kServer() {
}

int C1000kServer::LoadConfig( const base::FilePath& xml_ini_file ) {
  return 0;
}

int	C1000kServer::Initialize( int argc, char** argv ) {
  int port = 9100;
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch("port")) {
    base::StringToInt(command_line->GetSwitchValueASCII("port"), &port);
  }
  codec_.reset(new net::LengthHeaderCodec(this));

  for (int i=0; i<40; ++i) {
    net::TCPServer* server = new net::TCPServer(message_loop(), codec_.get());
    server->Create("127.0.0.1", base::IntToString(port+i), false);
    servers_.push_back(server);
  }

  return 0;
}

int C1000kServer::Destroy() {
  return 0;
}

int C1000kServer::OnLengthHeaderNewConnection(const net::IOHandlerPtr& ih) {
  LOG(INFO) << "OnLengthHeaderNewConnection()";
  return 0;
}

int C1000kServer::OnLengthHeaderDataReceived(const net::IOHandlerPtr& ih,
  const base::StringPiece& packet, base::Time receive_time) {
  return 0;
}

int C1000kServer::OnLengthHeaderConnectionClosed(const net::IOHandlerPtr& ih) {
  LOG(INFO) << "OnLengthHeaderConnectionClosed()";
  return 0;
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  base::AtExitManager at_exit_manager;

#if defined(OS_WIN)
  net::EnsureWinsockInit();
#endif // OS_WIN

  C1000kServer daemon;
  return daemon.DoMain(argc, argv);
}
