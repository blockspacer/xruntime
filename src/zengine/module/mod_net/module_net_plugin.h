// Copyright (C) 2014 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef MOD_NET_MODULE_NET_PLUGIN_H_
#define MOD_NET_MODULE_NET_PLUGIN_H_

#include "base/memory/singleton.h"

#include "zengine/zengine/zengine_plugin.h"
#include "zengine/module/mod_net/comm_node_info.h"
#include "zengine/module/mod_net/znet_tcp_server.h"
#include "zengine/module/mod_net/znet_tcp_client.h"

namespace zengine {

class ModuleNetPlugin : public Plugin {
public:
  ModuleNetPlugin();
  virtual ~ModuleNetPlugin();

  static const std::string& GetModuleNetName();

  virtual const std::string& GetPluginName() const;
  virtual void Install(const server::ConfigFile* config_file);
  virtual void Initialize();
  virtual void Shutdown();
  virtual void Uninstall();

private:
  std::vector<ZNetTCPServer*> servers_;
  std::vector<ZNetTCPClient*> clients_;

  std::vector<CommNodeInfo*> comm_nodes_;
};

class ModuleNetPluginFactory : public PluginFactory {
public:
  static ModuleNetPluginFactory* GetInstance() {
    return base::Singleton<ModuleNetPluginFactory>::get();
  }

  virtual const std::string& GetPluginName() const;
  virtual Plugin* CreateInstance();
  virtual void DestroyInstance(Plugin* module);

private:
  friend struct base::DefaultSingletonTraits<ModuleNetPluginFactory>;

  ModuleNetPluginFactory() {}
  virtual ~ModuleNetPluginFactory ()  {}
};

}

#endif
