// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef SCRIPT_SCRIPT_ENGINE_H_
#define SCRIPT_SCRIPT_ENGINE_H_

#include <string>

#include "script/luatinker/lua_tinker.h"

namespace script {
class ScriptEngine {
public:
  ScriptEngine() : lua_state_(NULL) {
  }

  ~ScriptEngine();

  void Initialize();
  void Refresh();
  void Destroy();

  void DoFile(const std::string& file_path);
  void DoBuffer(const char* data, size_t data_len);
  void DoBuffer(const std::string& data);

  //void RegisterDefault();

  lua_State* GetLuaState() {
    return lua_state_;
  }

  //HACK ʹ�����ô��ݣ�
  template<typename T>
  void SetGlobal(const char* name, T object) {
    lua_tinker::setglobal(lua_state_, name, object);
  }

  template<typename T>
  T GetGlobal(const char* name) {
    return lua_tinker::getglobal<T>(lua_state_, name);
  }

  //����ȫ�ֺ��� 
  template<typename F>
  void RegisterFunction(const char *func_name, F func);

  //����class 
  template<typename Class>
  void RegisterClass(const char *name) {
    lua_tinker::class_add<Class>(lua_state_, name);
  }

  //����class 
  template<typename Class>
  void RegisterClassConstructor() {
    lua_tinker::class_con<Class>(lua_state_, lua_tinker::constructor<Class>);
  }

  //����class 
  template<typename Class, typename T1>
  void RegisterClassConstructor() {
    lua_tinker::class_con<Class>(lua_state_, lua_tinker::constructor<Class, T1>);
  }

  //����class 
  template<typename Class, typename T1, typename T2>
  void RegisterClassConstructor() {
    lua_tinker::class_con<Class>(lua_state_, lua_tinker::constructor<Class, T1, T2>);
  }

  template<typename Class, typename F>
  void RegisterClassFuncnction(const char* name, F func) {
    lua_tinker::class_def<Class>(lua_state_, name, func);
  }

  template<typename Class, typename BASE, typename VAR>
  void RegisterClassMember(const char* name, VAR BASE::*val) {
    lua_tinker::class_mem<Class>(lua_state_, name, val);
  }

  // Call Lua Func {
  template <typename R>
  inline R CallFunction(const char *func) {
    return lua_tinker::call<R>(lua_state_, func);
  }

  template <typename R, typename T1>
  inline R CallFunction(const char *func, const T1& t1) {
    return lua_tinker::call<R, T1>(lua_state_, func, t1);
  }

  template <typename R, typename T1, typename T2>
  inline R CallFunction(const char *func, const T1& t1, const T2& t2) {
    return lua_tinker::call<R, T1, T2>(lua_state_, func, t1, t2);
  }

  template <typename R, typename T1, typename T2, typename T3>
  inline R CallFunction(const char *func, const T1& t1, const T2& t2, const T3& t3) {
    return lua_tinker::call<R, T1, T2, T3>(lua_state_, func, t1, t2, t3);
  }

  template <typename R, typename T1, typename T2, typename T3, typename T4>
  inline R CallFunction(const char *func, const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
    return lua_tinker::call<R, T1, T2, T3, T4>(lua_state_, func, t1, t2, t3, t4);
  }

private:
  lua_State* lua_state_;
};
}

//�����lua_CFunction ���͵ĺ���
template<typename F>
inline void script::ScriptEngine::RegisterFunction(const char *func_name, F func) {
  lua_tinker::def<F>(lua_state_, func_name, func);
}

//����ȫ�ֺ��� warp for lua_register
template<>
inline void script::ScriptEngine::RegisterFunction<lua_CFunction>(
  const char *func_name, lua_CFunction func) {
  lua_register(lua_state_, func_name, func);
}

#endif
