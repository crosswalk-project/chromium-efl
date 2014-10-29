// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_GPU_THREAD_OVERRIDE_EFL_H
#define GPU_GPU_THREAD_OVERRIDE_EFL_H

#include <string>

namespace base {
class Thread;
}

base::Thread* CreateInProcessGpuThreadEfl(const std::string& channel_id);

#endif // GPU_GPU_THREAD_OVERRIDE_EFL_H
