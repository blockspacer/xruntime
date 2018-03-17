// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "zengine/zengine/zengine_context.h"

#include "base/logging.h"
#include "base/message_loop/message_loop.h"

#include "zengine/zengine/script/script_manager.h"
#include "zengine/zengine/zengine_context_manager.h"
#include "zengine/zengine/zengine_daemon.h"
#include "zengine/zengine/zengine_thread.h"

const std::string ZEngineContext::kMainInstanceName("main");
const std::string ZEngineContext::kIOInstanceName("io");

bool ZEngineContext::Initialize() {
  script_manager_ = new ScriptManager();
  script_manager_->Initialize();

  timer_manager_ = new TimerManager(this, message_loop_);
  return true;
}

void ZEngineContext::Destroy() {
  if (context_manager_) {
    context_manager_->RemoveContext(this);
  }

  if (timer_manager_) {
    timer_manager_->Destroy();
    timer_manager_ = NULL;
  }

  if (script_manager_) {
    script_manager_->Destroy();
    delete script_manager_;
    script_manager_ = NULL;
  }
}

void ZEngineContext::SetTimer(uint32_t timer_id, int tm) {
  DCHECK(timer_manager_);
  timer_manager_->SetTimer(timer_id, tm);
}

void ZEngineContext::KillTimer(uint32_t timer_id) {
  DCHECK(timer_manager_);
  timer_manager_->KillTimer(timer_id);
}

void ZEngineContext::OnTimer(uint32_t timer_id, int tm) {
  DCHECK(script_manager_);
  script_manager_->ExeScript_OnTimer(this, timer_id, tm);
}

bool ZEngineContext::PostTaskByInstanceName(const std::string& instance_name,
  const std::string& task_data) {
  ZEngineContext* context = context_manager_->LookupContext(instance_name);
  if (context) {
    context->PostTask(task_data);
  }
  return context != NULL;
}

bool ZEngineContext::PostTaskByInstanceID(base::PlatformThreadId instance_id,
  const std::string& task_data) {
  ZEngineContext* context = context_manager_->LookupContext(instance_id);
  if (context) {
    context->PostTask(task_data);
  }
  return context != NULL;
}

bool ZEngineContext::PostTaskToIOThread(const std::string& task_data) {
  ZEngineContext* context = context_manager_->LookupIOContext();
  if (context) {
    context->PostTask(task_data);
  }
  return context != NULL;
}

void ZEngineContext::PostTaskToMainThread(const std::string& task_data) {
  ZEngineContext* context = context_manager_->LookupMainContext();
  if (context) {
    context->PostTask(task_data);
  }
}

void ZEngineContext::PostTask(const std::string& task_data) {
  CHECK(message_loop_);
  message_loop_->PostTask(FROM_HERE,
    base::Bind(&ZEngineContext::OnTaskDataReceived, base::Unretained(this),
      task_data));
}

void ZEngineContext::OnTaskDataReceived(std::string task_data) {
  DCHECK(script_manager_);
  script_manager_->ExeScript_OnTaskDataReceived(this, task_data);
}
