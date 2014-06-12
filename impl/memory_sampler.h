/*
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  The MemorySampler class samples a number of internal and external memory
 *  metrics every second while running. Sample data is written to a log file.
 *  Sampling occurs over a duration specified when started. If duration is set
 *  to 0 (default), the memory sampler will run indefinitely until the stop
 *  function is called. MemorySampler allows the option of sampling "in use"
 *  memory (committed memory minus free list byte count) or committed memory for
 *  any allocator which keeps a free list. This includes FastMalloc and the
 *  JavaScriptCore heap at this time.
 *  The following memory metrics are recorded:
 *
 *      WebCore
 *          - FastMalloc allocations bytes              (in use or committed)
 *      JavaScriptCore
 *          - Garbage collector heap bytes              (in use or committed)
 *          - Stack bytes                               (committed only!)
 *          - JIT Code bytes                            (committed only!)
 *      Malloc zones
 *          - In use bytes for the following zones:
 *              * Default zone                          (in use or committed)
 *              * DispCon zone                          (in use or committed)
 *              * Purgable zone                         (in use or committed)
 *      Task Info
 *          - Resident size memory (RSIZE)
 */

#ifndef MEMORY_SAMPLER_H_
#define MEMORY_SAMPLER_H_

#include "base/timer/timer.h"
#include "base/file_util.h"

class WebMemorySampler {
public:
  static WebMemorySampler* shared();
  void start(const double interval=0);
  void stop();
  bool isRunning() const;

private:
  WebMemorySampler();
  ~WebMemorySampler();

  void writeHeaders();
  void initializeTimers(double);
  std::string processName() const;
  void sampleTimerFired();
  void stopTimerFired();

private:
  std::string m_separator_;
  bool m_is_running_;
  double m_running_time_;
  int m_pid_;

  FILE* m_sample_log_file_;
  base::FilePath m_sample_log_file_path_;
  base::RepeatingTimer<WebMemorySampler> m_sample_timer_;
  base::OneShotTimer<WebMemorySampler> m_stop_timer_;
};

#endif  // MEMORY_SAMPLER_H_
