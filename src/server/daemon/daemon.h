// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
// ����������,Ϊ���������ṩ��һ��ͨ�õ�����:
// �����������ļ�,��ʼ������,���ٺ���,�Լ�ѭ������
//

#ifndef SERVER_DAEMON_H_
#define SERVER_DAEMON_H_
#pragma once

#include <string>

#include "base/message_loop/message_loop.h"

#include "base/files/file_path.h"

namespace server {

class Daemon {
public:
  Daemon();
  virtual ~Daemon();

  /**
    * ������,��ִ���ļ���������Ϊ��־�ı�־����־;����SIGINT��SIGPIPE�ź�;
    * ������Ƿ���"-xml"��Ĭ�ϲ���,������,������Ϊ�����ļ�������,������ִ���ļ������ֺ�׺
    * ��".xml"��Ϊ�����ļ�������,��ȡ�����ļ�;����ִ�г�ʼ������,ѭ����������,�����ٺ���,���ر���־
    * ����ÿһ������¼����־��.
    * @param argc ����ִ��ʱ�����Ĳ�������,����ִ���ļ�
    * @param argv ����ִ��ʱ�����Ĳ���,����ִ���ļ���
    * @return �ɹ��򷵻�0
    * @see loadConfig(),initialize(),run(),destroy(),
    */
  virtual int DoMain(int argc, char** argv);

  void Quit() {
    running_ = false;
  }

  virtual base::MessageLoop* message_loop() {
    if (message_loop_ == NULL) {
      message_loop_ = new base::MessageLoop();
    }
    return message_loop_;
  }

  virtual int Run();

protected:
  /**
    * ��ȡ�����ļ�,Ĭ�ϵ������ļ���Ϊargv[0].ini,��argv[0]��ͬһĿ¼
    * ��initialize()ǰִ��
    * @return ����ȡ���ɹ�Ҫ��ֹ���� ʱ��Ӧ�÷��ط�0,��Ӧ�ó���ֱ���˳�
    * ע���ʱ����ִ��destroy()
    */
  virtual int LoadConfig(const base::FilePath& xml_ini_file);

  /**
    * ��LoadConfig()��ĵ�һ������,���ڳ�ʼ,�紴�����ӵȵ�
    * @param argc ����ִ��ʱ�����Ĳ�������,����ִ���ļ�
    * @param argv ����ִ��ʱ�����Ĳ���,����ִ���ļ���
    * @return ����ʼ�����ɹ�Ҫ��ֹ���� ʱ��Ӧ�÷��ط�0,��Ӧ�ó��򽫵���destroy()���˳�
    */
  virtual int Initialize(int argc, char** argv);

  /**
    * ������,��Ӧ�ó��򼴽��˳�ǰִ��.����ִ���ͷ���initialize()�������
    * ��Դ�ȹ���.
    * @return ���ɹ��򷵻�0
    */
  virtual int Destroy();

  virtual int DoLoop() {
    return 0;
  }

  int Run2();

  bool running_;
  base::MessageLoop* message_loop_;
  base::FilePath exe_path_;
};
}

#endif // SERVER_DAEMON_H_

