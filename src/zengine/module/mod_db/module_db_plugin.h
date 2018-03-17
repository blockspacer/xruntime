// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef MOD_DB_MODULE_NET_PLUGIN_H_
#define MOD_DB_MODULE_NET_PLUGIN_H_

#include "base/memory/singleton.h"
#include "script/base/linked_hash_map.h"
#include "zengine/zengine/zengine_plugin.h"

//#include "db/conn_pool_manager.h"

class CdbConnPoolManagerProxy {
public:
  CdbConnPoolManagerProxy(/*db::CdbConnPoolManager* conn_pool*/)
   /* : conn_pool_(conn_pool)*/ {
  }

  ~CdbConnPoolManagerProxy() {
  }

  // ��ѯ���
  // ����ֵ���ΪNULL,���������:
  //  1. �޼�¼
  //  2. ִ�в�ѯʱ�����˴���
  // ���ΪNULL�����ǿ��Ե���GetLastError()������Ƿ����޼�¼���Ƿ�������������
  /*db::QueryAnswer*/void * Query(const char* q_str) {
    return nullptr;
    //db::ScopedPtr_DatabaseConnection conn(conn_pool_.get());
    //tmp_answ_.reset(conn->Query(q_str));
    //return  tmp_answ_.get();
  };

  // ����
  // ����INSERT ���������� ID
  // �������ֵΪ0,����
  uint64_t ExecuteInsertID(const char* q_str) {
    return 0;
    //db::ScopedPtr_DatabaseConnection conn(conn_pool_.get());
    //return conn->ExecuteInsertID(q_str);
  }

  // ���룬���º�ɾ��
  // ����ֵΪ>=0���ɹ�����Ӱ�������
  // <0 ����
  int Execute(const char* q_str) {
    return 0;
    //db::ScopedPtr_DatabaseConnection conn(conn_pool_.get());
    //return conn->Execute(q_str);
  }

private:
  //std::unique_ptr<db::CdbConnPoolManager> conn_pool_;
  //std::unique_ptr<db::QueryAnswer> tmp_answ_;
};

namespace zengine {

class ModuleDBPlugin : public Plugin {
public:
  ModuleDBPlugin() {}
  virtual ~ModuleDBPlugin() {}

  static const std::string& GetModuleDBName();

  virtual const std::string& GetPluginName() const;
  virtual void Install(const server::ConfigFile* config_file);
  virtual void Initialize();
  virtual void Shutdown();
  virtual void Uninstall();

  bool IsInitialized() {
    return !db_conn_pool_mgrs_.empty();
  }

  CdbConnPoolManagerProxy* GetDBConnPoolMgrByInstanceName(
    const std::string& instance_name);
  CdbConnPoolManagerProxy* GetDBConnPoolMgr();

private:
  linked_hash_map<std::string, CdbConnPoolManagerProxy*> db_conn_pool_mgrs_;
};

class ModuleDBPluginFactory : public PluginFactory {
public:
  static ModuleDBPluginFactory* GetInstance() {
    return base::Singleton<ModuleDBPluginFactory>::get();
  }

  virtual const std::string& GetPluginName() const;
  virtual Plugin* CreateInstance();
  virtual void DestroyInstance(Plugin* module);

private:
  friend struct base::DefaultSingletonTraits<ModuleDBPluginFactory>;

  ModuleDBPluginFactory() {}
  virtual ~ModuleDBPluginFactory ()  {}
};

}

#endif
