/*
   Copyright (C) 2014 Samsung Electronics

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

#include "gpu/gpu_thread_override_efl.h"

#include "content/common/gpu/gpu_messages.h"
#include "content/gpu/gpu_process.h"
#include "gl/gl_shared_context_efl.h"
#include "gpu/command_buffer/service/mailbox_manager.h"
#include "ipc/ipc_channel.h"
#include "ipc/ipc_message_macros.h"

#define private public
#include "content/common/gpu/gpu_channel_manager.h"
#include "content/gpu/in_process_gpu_thread.h"
#include "content/gpu/gpu_child_thread.h"

// Implementation of InProcessGpuThread and GpuChildThread overrides
// in order to provide on startup shared context and mailbox manager
// created form Efl shared evas gl context.

struct GpuChildThreadEfl : public content::GpuChildThread {
  explicit GpuChildThreadEfl(const std::string& channel_id)
      : GpuChildThread(channel_id) { }

  virtual bool OnControlMessageReceived(const IPC::Message& msg) OVERRIDE {
    bool handled = true;
    IPC_BEGIN_MESSAGE_MAP(GpuChildThreadEfl, msg)
      IPC_MESSAGE_HANDLER(GpuMsg_Initialize, OnInitialize)
      IPC_MESSAGE_UNHANDLED(handled = false)
    IPC_END_MESSAGE_MAP()

    if (handled)
      return true;

    return GpuChildThread::OnControlMessageReceived(msg);
  }

  void OnInitialize() {
    GpuChildThread::OnInitialize();
    DCHECK(!gpu_channel_manager_->share_group_);
    DCHECK(!gpu_channel_manager_->mailbox_manager_);
    gpu_channel_manager_->share_group_ =
        GLSharedContextEfl::GetShareGroup();
    gpu_channel_manager_->mailbox_manager_ =
        GLSharedContextEfl::GetMailboxManager();
  }
};

struct InProcessGpuThreadEfl : public content::InProcessGpuThread {

  explicit InProcessGpuThreadEfl(const std::string& channel_id)
      : InProcessGpuThread(channel_id) { }

  virtual void Init() OVERRIDE {
    gpu_process_ = new content::GpuProcess();
    // The process object takes ownership of the thread object, so do not
    // save and delete the pointer.
    gpu_process_->set_main_thread(new GpuChildThreadEfl(channel_id_));
  }
};

base::Thread* CreateInProcessGpuThreadEfl(const std::string& channel_id) {
  return new InProcessGpuThreadEfl(channel_id);
}
