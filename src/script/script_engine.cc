// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "script/script_engine.h"

#include <list>

#include "base/logging.h"

namespace script {
void ScriptEngine::Initialize() {
  // ȷ��ֻ���ʼ��һ��
  if (!lua_state_) {
    lua_state_ = luaL_newstate(); // ����Lua״̬
    if (!lua_state_) {
      LOG(ERROR) << "ScriptEngine::Init() lua_open failed!";
      return;
    }
    luaL_openlibs(lua_state_); // Ϊ��Lua״̬������Lua��
    //luabind::open(lua_state_); // Ϊ��Lua״̬��luabind��
    //luaopen_zmq(lua_state_);

    //lua_interface_.Init(lua_state_);

	lua_tinker::init(lua_state_);
  }
}

ScriptEngine::~ScriptEngine() {
  Destroy();
}

void ScriptEngine::Refresh() {
  Destroy();
  Initialize();
}

void ScriptEngine::Destroy() {
  if (lua_state_) {
    lua_close(lua_state_);
    lua_state_ = NULL;
  }
}

void ScriptEngine::DoFile(const std::string& file_path) {
  CHECK(lua_state_ != NULL);
  lua_tinker::dofile( lua_state_, file_path.c_str());
}

void ScriptEngine::DoBuffer(const char* data, size_t data_len) {
  CHECK(lua_state_ != NULL);

  lua_tinker::dobuffer(lua_state_, data, data_len);
}

void ScriptEngine::DoBuffer(const std::string& buffer) {
  DoBuffer(buffer.c_str(), buffer.length());
}
}
