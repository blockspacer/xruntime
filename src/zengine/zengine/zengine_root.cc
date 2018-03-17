// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "zengine/zengine/zengine_root.h"

#include <algorithm>
#include "base/logging.h"
#include "server/base/exception.h"

#include "script/luabind_register_manager.h"

#include "zengine/zengine/script/script_file_manager.h"
#include "zengine/zengine/script/script_manager.h"
#include "zengine/zengine/zengine_context.h"
#include "zengine/zengine/zengine_context_manager.h"
#include "zengine/zengine/zengine_plugin_dll_manager.h"
#include "zengine/zengine/zengine_thread.h"

namespace zengine{
Root::Root() 
  : is_initialized_(false)
  , context_(NULL) {
  LuabindRegisterManager::GetInstance();
  ScriptFileManager::GetInstance();
  PluginDllManager::GetInstance();

  ZEngineContextManager::GetInstance();

  message_loop_ = base::MessageLoop::current();
}

Root::~Root() {
  ShutdownModules();
  UnloadModules();
}

void Root::Initialize(const server::ConfigFile* config) {
  config_file_ = config;
  PluginDllManager::GetInstance()->Initialize();

  server::StringVector values = config->GetStringList("Modules", "file_path");
  LoadModules(values.string_vector);

  context_ = ZEngineContextManager::GetInstance()->
    CreateContext(ZEngineContext::kZEngineContextType_Main,
      base::PlatformThread::CurrentId(),
      ZEngineContext::kMainInstanceName, base::MessageLoop::current());
  
  is_initialized_ = true;
}

void Root::Shutdown() {
  if (message_loop_) {
    message_loop_->PostTask(FROM_HERE,
      base::Bind(&Root::DoShutdown, base::Unretained(this)));
  }
}

bool Root::CreateZEngineThread(const std::string& instance_name) {
  if (instance_name==ZEngineContext::kIOInstanceName
    || instance_name==ZEngineContext::kMainInstanceName) {
    LOG(ERROR) << "CreateZEngineThread error, instance_name is "
      << instance_name << ", not equsal main or io.";
    return false;
  }

  if (zengine_threads_.find(instance_name) != zengine_threads_.end()) {
    LOG(ERROR) << "CreateZEngineThread error, instance_name "
      << instance_name << " existed.";
    return false;
  }

  shared_ptr<ZEngineThread> zthread(new ZEngineThread(instance_name));
  zthread->Start();
  zengine_threads_.insert(std::make_pair(instance_name, zthread));

  return true;
}

void Root::Run() {
  OnCreate();
  base::MessageLoop::current()->Run();
  OnDestroy();
}

void Root::OnCreate() {
  InitializeModules();
  context_->script_manager()->ExeScript_MainInitialize();
  context_->script_manager()->ExeScript_OnThreadInitialize(context_);
}

void Root::OnDestroy() {
  if (is_initialized_) {
    ZEngineThreadMap::iterator it;
    for (it=zengine_threads_.begin(); it!=zengine_threads_.end(); ++it) {
      it->second->Stop();
    }
    zengine_threads_.clear();

    if (context_) {
      context_->script_manager()->ExeScript_OnThreadDestroy(context_);
      context_->script_manager()->ExeScript_MainDestroy();

      context_->Destroy();
      delete context_;
      context_ = NULL;
    }
    ShutdownModules();
    is_initialized_ = false;
  }
}

void Root::DoShutdown() {
  if (message_loop_) {
    message_loop_->QuitNow();
  }
}

void Root::LoadModules(const std::vector<std::string>& module_names) {
  for (size_t i=0; i<module_names.size(); ++i) {
    LoadModule(module_names[i]);
  }
}

void Root::InitializeModules() {
  for (size_t i=0; i<modules_.size(); ++i) {
    modules_[i]->Initialize();
  }
}

void Root::ShutdownModules() {
  for (size_t i=0; i<modules_.size(); ++i) {
    modules_[i]->Shutdown();
  }
}

void Root::UnloadModules() {
  for (ModuleInstanceList::reverse_iterator it = modules_.rbegin();
    it != modules_.rend(); ++it) {
    PluginDllManager::GetInstance()->DllStopModule((*it)->GetPluginName());
  }
  modules_.clear();

  // now deal with any remaining plugins that were registered through other
  // means
  for (ModuleInstanceList::reverse_iterator it=modules_.rbegin();
    it!=modules_.rend(); ++it) {
    // Note this does NOT call uninstallPlugin - this shutdown is for the 
    // detail objects
    (*it)->Uninstall();
  }
  modules_.clear();
}

void Root::LoadModule(const std::string& module_name) {
  PluginDllManager::GetInstance()->DllStartModule(module_name);
}

void Root::UnloadModule(const std::string& module_name) {
  for (size_t i = 0; i<modules_.size(); ++i) {
    if (modules_[i]->GetPluginName() == module_name) {
      PluginDllManager::GetInstance()->DllStopModule(module_name);
    }
  }
}

void Root::InstallModule(Plugin* module) {
  LOG(INFO) << "void Root::InstallModule - Installing plugin: "
    << module->GetPluginName();
  modules_.push_back(module);
  module->Install(config_file_);
  if (is_initialized_) {
    module->Initialize();
  }
  LOG(INFO) << "void Root::InstallModule - Plugin successfully installed";
 }

void Root::UninstallModule(Plugin* module) {
  std::string module_name = module->GetPluginName();
  LOG(INFO) << "void Root::UninstallModule - Uninstalling plugin: "
    << module_name;
  ModuleInstanceList::iterator it =
    std::find(modules_.begin(), modules_.end(), module);
  if (it != modules_.end()) {
    if (is_initialized_) {
      module->Shutdown();
    }
    module->Uninstall();
  }
  LOG(INFO) << "void Root::UninstallModule - Successfully uninstalled plugin: "
    << module_name;
}

}
