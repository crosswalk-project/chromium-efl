// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "message_pump_efl.h"

#include "base/logging.h"
#include "base/message_loop/message_loop.h"

// EFL version 1.8 and later don't define the function
// in the Ecore.h. It's still defined and visible however.
extern "C" {
int ecore_main_loop_iterate_may_block(int);
}

namespace base {

namespace {
void PipeCallback(void *data, void*, unsigned int) {
  static_cast<MessagePumpEfl*>(data)->HandleScheduledWork();
}
}

struct MessagePumpEfl::RunState {
  Delegate* delegate;
  bool should_quit;
  int run_depth;
};

MessagePumpEfl::MessagePumpEfl()
  : state_(NULL)
  , pipe_(ecore_pipe_add(&PipeCallback, this))
  , work_scheduled_(false) {
}

MessagePumpEfl::~MessagePumpEfl() {
  ecore_pipe_del(pipe_);
  pipe_ = NULL;
}

void MessagePumpEfl::Run(Delegate* delegate) {
  RunState state;
  state.delegate = delegate;
  state.should_quit = false;
  state.run_depth = state_ ? state_->run_depth + 1 : 1;

  RunState* previous_state = state_;
  state_ = &state;

  bool more_work_is_plausible = true;
  for (;;) {
    bool block = !more_work_is_plausible;

    more_work_is_plausible = ecore_main_loop_iterate_may_block(block);
    if (state_->should_quit)
      break;

    more_work_is_plausible |= state_->delegate->DoWork();
    if (state_->should_quit)
      break;

    more_work_is_plausible |=
        state_->delegate->DoDelayedWork(&delayed_work_time_);
    if (state_->should_quit)
      break;

    if (more_work_is_plausible)
      continue;

    more_work_is_plausible = state_->delegate->DoIdleWork();
    if (state_->should_quit)
      break;
  }

  state_ = previous_state;
}

void MessagePumpEfl::Quit() {
  if (state_) {
    state_->should_quit = true;
  } else {
    NOTREACHED() << "Quit called outside Run!";
  }
}

void MessagePumpEfl::ScheduleWork() {
  {
    base::AutoLock locker(schedule_work_lock_);
    if (work_scheduled_)
      return;
    work_scheduled_ = true;
  }

  static const char msg[] = "!";
  if (!ecore_pipe_write(pipe_, msg, sizeof(msg))) {
    NOTREACHED() << "Could not write to the UI message loop wakeup pipe!";
  }
}

void MessagePumpEfl::ScheduleDelayedWork(const TimeTicks& delayed_work_time) {
  delayed_work_time_ = delayed_work_time;
  ScheduleWork();
}

void MessagePumpEfl::HandleScheduledWork() {
  {
    base::AutoLock locker(schedule_work_lock_);
    DCHECK(work_scheduled_);
    work_scheduled_ = false;
  }

  if (state_->delegate->DoWork()) {
    ScheduleWork();
  }

  if (state_->should_quit)
    return;

  state_->delegate->DoDelayedWork(&delayed_work_time_);
}

} // namespace base
