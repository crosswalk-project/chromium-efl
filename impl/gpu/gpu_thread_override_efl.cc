// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  virtual bool OnControlMessageReceived(const IPC::Message& msg) override {
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
    DCHECK(!gpu_channel_manager_->share_group_.get());
    DCHECK(!gpu_channel_manager_->mailbox_manager_.get());
    gpu_channel_manager_->share_group_ =
        GLSharedContextEfl::GetShareGroup();
    gpu_channel_manager_->mailbox_manager_ =
        GLSharedContextEfl::GetMailboxManager();
  }
};

struct InProcessGpuThreadEfl : public content::InProcessGpuThread {

  explicit InProcessGpuThreadEfl(const std::string& channel_id)
      : InProcessGpuThread(channel_id) { }

  virtual void Init() override {
    gpu_process_ = new content::GpuProcess();
    // The process object takes ownership of the thread object, so do not
    // save and delete the pointer.
    gpu_process_->set_main_thread(new GpuChildThreadEfl(channel_id_));
  }
};

base::Thread* CreateInProcessGpuThreadEfl(const std::string& channel_id) {
  return new InProcessGpuThreadEfl(channel_id);
}
