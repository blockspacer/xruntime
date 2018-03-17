// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef ZENGINE_ZENGINE_PLUGIN_H_
#define ZENGINE_ZENGINE_PLUGIN_H_
#pragma once

#include <string>

namespace server {
class ConfigFile;
}

namespace zengine {

class Plugin {
public:
  Plugin() {}
  virtual ~Plugin() {}

  virtual const std::string& GetPluginName() const = 0;

  virtual void Install(const server::ConfigFile* config_file) {
    config_file_ = config_file;
  }

  virtual void Initialize() = 0;
  virtual void Shutdown() = 0;
  virtual void Uninstall() = 0;

protected:
  const server::ConfigFile* config_file_;
};

class PluginFactory {
public:
  virtual ~PluginFactory ()  {}

  virtual const std::string& GetPluginName() const = 0;
  virtual Plugin* CreateInstance() = 0;
  virtual void DestroyInstance(Plugin* module) = 0;
};

}

#endif
