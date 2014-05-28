/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MESSAGE_PUMP_FOR_UI_EFL
#define MESSAGE_PUMP_FOR_UI_EFL

#include "base/message_loop/message_pump.h"
#include "base/synchronization/lock.h"
#include <Ecore.h>
#include <Eina.h>

namespace base {

class RunLoop;
class TimeTicks;

class BASE_EXPORT MessagePumpForUIEfl : public base::MessagePump {
 public:
  MessagePumpForUIEfl();
  virtual ~MessagePumpForUIEfl();

  virtual void Run(Delegate *) OVERRIDE;
  virtual void Quit() OVERRIDE;
  virtual void ScheduleWork() OVERRIDE;
  virtual void ScheduleDelayedWork(const base::TimeTicks&) OVERRIDE;

 private:
  static void PipeCallback(void*, void*, unsigned int);
  static Eina_Bool TimerCallback(void*);
  void DoWork();
  void DoDelayedWork();

  Ecore_Pipe* pipe_;
  Delegate* delegate_;
  RunLoop* run_loop_;
  base::Lock schedule_work_lock_;
  bool work_scheduled_;

  DISALLOW_COPY_AND_ASSIGN(MessagePumpForUIEfl);
};

}

#endif
