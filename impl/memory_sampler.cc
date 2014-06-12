/*
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2012, 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
 */

#include "memory_sampler.h"

#include <stdio.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

#include "base/file_util.h"
#include "base/time/time.h"

#include "wtf/FastMalloc.h"

namespace {

struct ApplicationMemoryStats {
  size_t totalProgramSize;
  size_t residentSetSize;
  size_t proportionalSetSize;
  size_t peak;
  size_t sharedSize;
  size_t textSize;
  size_t librarySize;
  size_t dataStackSize;
  size_t dirtyPageSize;
  size_t UMPSize;
  size_t graphics3DSize;
  size_t privateCleanSize;
  size_t privateDirtySize;
} applicationStats;

unsigned getPeakRSS(int pid) {
  std::ostringstream filename;
  filename << "/proc/"<< pid << "/status";
  std::ifstream filestr (filename.str().c_str());
  if (!filestr.is_open()) {
    return 0;
  }

  const std::string pattern = "VmHWM";
  unsigned size = 0;
  std::string line;
  int position;

  while (!filestr.eof()) {
    getline(filestr, line);
    if (line.compare(0, pattern.length(), pattern) == 0) {
      position = line.find(":");
      if (position != -1) {
        size = atoi(line.c_str() + position + 1);
        break;
      }
    }
  }
  filestr.close();
  return size;
}

int getSizeValue(int position, std::string line) {
  line.erase(0, position+1);
  position = line.find("kB");
  return atoi(line.c_str() + position+1);
}

void sampleDeviceMemoryMalloc(ApplicationMemoryStats& applicationStats, int pid) {
  std::ostringstream filename;
  filename << "/proc/" << pid << "/smaps";
  std::ifstream filestr (filename.str().c_str());
  if (!filestr.is_open()) {
    return;
  }

  int UMPSize = 0;
  int graphics3DSize = 0;
  int privateCleanSize = 0;
  int privateDirtySize = 0;
  int deviceDriverSize = 0;
  int proportionalSetSize = 0;

  const std::string strUMPZone = "/dev/ump";
  const std::string strGraphicsZone = "/dev/mali";
  const std::string strPrivateClean = "Private_Clean";
  const std::string strPrivateDirty = "Private_Dirty";
  const std::string strDeviceDriver = "/dev/dri/card0";
  const std::string strProportionalSetSize = "Pss";
  std::string line;
  int position;

  while (!filestr.eof()) {
    getline(filestr, line);
    position = line.find(strUMPZone);
    if (position != -1) {
      UMPSize += getSizeValue(position, line);
    }
    position = line.find(strGraphicsZone);
    if (position != -1) {
      graphics3DSize += getSizeValue(position, line);
    }
    position = line.find(strPrivateClean);
    if (position != -1) {
      privateCleanSize += getSizeValue(position, line);
    }
    position = line.find(strPrivateDirty);
    if (position != -1) {
      privateDirtySize += getSizeValue(position, line);
    }
    position = line.find(strDeviceDriver);
    if (position != -1) {
      deviceDriverSize += getSizeValue(position, line);
    }
    position = line.find(strProportionalSetSize);
    if (position != -1) {
      proportionalSetSize += getSizeValue(position, line);
    }
  }

  applicationStats.UMPSize = UMPSize;
  applicationStats.graphics3DSize = graphics3DSize;
  applicationStats.privateCleanSize = privateCleanSize;
  applicationStats.privateDirtySize = privateDirtySize;
  applicationStats.residentSetSize -= deviceDriverSize;
  applicationStats.proportionalSetSize = proportionalSetSize - deviceDriverSize;
  applicationStats.peak = getPeakRSS(pid) - deviceDriverSize;

  filestr.close();
}

void sampleApplicationMalloc(ApplicationMemoryStats& applicationStats, int pid) {
  std::ostringstream filename;
  filename << "/proc/" << pid << "/statm";
  std::ifstream filestr (filename.str().c_str());
  if (filestr.is_open()) {

    std::string appMemStat;
    const char delim = ' ';

    std::getline(filestr, appMemStat, delim);
    applicationStats.totalProgramSize = atoi(appMemStat.c_str());
    std::getline(filestr, appMemStat, delim);
    applicationStats.residentSetSize = atoi(appMemStat.c_str());
    std::getline(filestr, appMemStat, delim);
    applicationStats.sharedSize = atoi(appMemStat.c_str());
    std::getline(filestr, appMemStat, delim);
    applicationStats.textSize = atoi(appMemStat.c_str());
    std::getline(filestr, appMemStat, delim);
    applicationStats.librarySize = atoi(appMemStat.c_str());
    std::getline(filestr, appMemStat, delim);
    applicationStats.dataStackSize = atoi(appMemStat.c_str());
    std::getline(filestr, appMemStat, delim);
    applicationStats.dirtyPageSize = atoi(appMemStat.c_str());

    filestr.close();

    // Get Additional Memory Info: UMP, 3D Graphics, Device Driver, Private Clean/Dirty, RSS, PSS Bytes
    sampleDeviceMemoryMalloc(applicationStats, pid);
  }
}

std::string sampleSystemMemoryInfoKeys() {
  std::string header;
  const std::string filename = "/proc/meminfo";
  std::ifstream filestr (filename.c_str());
  if (!filestr.is_open()) {
    return std::string();
  }
  std::string line;
  while (!filestr.eof()) {
    getline(filestr, line);
    int f = line.find(':');
    if (f != -1) {
      header += line.substr(0, f) + "\t";
    }
  }
  filestr.close();
  return header;
}

void sampleSystemMemoryInfo(std::vector<size_t>& values) {
  const std::string filename = "/proc/meminfo";
  std::ifstream filestr (filename.c_str());
  if (!filestr.is_open()) {
    return;
  }
  std::string line;
  int position;
  while (!filestr.eof()) {
    getline(filestr, line);
    position = line.find(":");
    if (position != -1) {
      line.erase(0, position+1);
    }
    position = line.find("kB");
    if (position != -1) {
      line.erase(position, position+1);
      values.push_back(atoi(line.c_str()));
    }
  }
  filestr.close();
}

} // namespace

WebMemorySampler* WebMemorySampler::shared() {
  static WebMemorySampler* sharedMemorySampler;
  if (!sharedMemorySampler)
    sharedMemorySampler = new WebMemorySampler();
  return sharedMemorySampler;
}

WebMemorySampler::WebMemorySampler()
  : m_separator_("\t"),
    m_is_running_(false),
    m_running_time_(0) {
  m_pid_ = getpid();
  m_sample_log_file_ = CreateAndOpenTemporaryFile(&m_sample_log_file_path_);
}

WebMemorySampler::~WebMemorySampler() {
  stop();
}

void WebMemorySampler::start(const double interval) {
  if (m_is_running_)
    return;

  writeHeaders();
  initializeTimers(interval);
}

void WebMemorySampler::stop() {
  if (!m_is_running_)
    return;
  m_sample_timer_.Stop();

  m_is_running_ = false;

  if (m_stop_timer_.IsRunning())
    m_stop_timer_.Stop();

  fclose(m_sample_log_file_);
}

bool WebMemorySampler::isRunning() const {
  return m_is_running_;
}

void WebMemorySampler::writeHeaders() {
  std::vector<std::string> keys;
  std::string header;

  std::string processDetails("Process: ");
  processDetails.append(processName());
  processDetails.append("\n");
  base::AppendToFile(m_sample_log_file_path_, processDetails.c_str(), processDetails.length());

  header = "\tTimestamp\tTotal Program Size\tRSS\tShared\tText\tLibrary\t"
  "Data/Stack\tDirty\tUMP\tGraphics 3D\tTotal Memory In Use\t"
  "Fast Malloc Zone Bytes\tTotal Committed Memory\tFast Malloc Zone Bytes\t";

  header.append(sampleSystemMemoryInfoKeys());
  header.append("\n");

  base::AppendToFile(m_sample_log_file_path_, header.c_str(), header.length());
}

void WebMemorySampler::initializeTimers(double interval) {
  m_sample_timer_.Start(FROM_HERE, base::TimeDelta::FromSeconds(1), this, &WebMemorySampler::sampleTimerFired);

  if (interval > 0) {
    m_stop_timer_.Start(FROM_HERE, base::TimeDelta::FromSeconds(interval), this, &WebMemorySampler::stopTimerFired);
  }
  m_running_time_ = interval;
  m_is_running_ = true;
}

std::string WebMemorySampler::processName() const {
  std::ostringstream filename;
  filename << "/proc/" << m_pid_ << "/status";
  std::ifstream filestr (filename.str().c_str());
  if (!filestr.is_open()) {
    return std::string();
  }

  std::string processName;
  getline(filestr, processName);
  int position = processName.find(":");
  if (position != -1) {
    processName.erase(0, position+1);
  }
  filestr.close();
  return processName;
}

void WebMemorySampler::sampleTimerFired() {
  std::vector<size_t> values;
  std::string stats;
  ApplicationMemoryStats applicationStats;
  sampleApplicationMalloc(applicationStats, m_pid_);

  double now = base::Time::Now().ToDoubleT();

  size_t totalBytesInUse = 0, totalBytesCommitted = 0;

  WTF::FastMallocStatistics fastMallocStatistics = WTF::fastMallocStatistics();
  size_t fastMallocBytesInUse = fastMallocStatistics.committedVMBytes - fastMallocStatistics.freeListBytes;
  size_t fastMallocBytesCommitted = fastMallocStatistics.committedVMBytes;
  totalBytesInUse += fastMallocBytesInUse;
  totalBytesCommitted += fastMallocBytesCommitted;

  values.push_back(now);

  values.push_back(applicationStats.totalProgramSize);
  values.push_back(applicationStats.residentSetSize);
  values.push_back(applicationStats.sharedSize);
  values.push_back(applicationStats.textSize);
  values.push_back(applicationStats.librarySize);
  values.push_back(applicationStats.dataStackSize);
  values.push_back(applicationStats.dirtyPageSize);
  values.push_back(applicationStats.UMPSize);
  values.push_back(applicationStats.graphics3DSize);

  values.push_back(totalBytesInUse);
  values.push_back(fastMallocBytesInUse);

  values.push_back(totalBytesCommitted);
  values.push_back(fastMallocBytesCommitted);

  sampleSystemMemoryInfo(values);

  if (!values.empty()) {
    for (size_t i = 0; i < values.size(); ++i) {
      stats.append(m_separator_);
      std::stringstream ss;
      ss << values[i];
      stats.append(ss.str());
    }
  }
  stats.append("\n");

  base::AppendToFile(m_sample_log_file_path_, stats.c_str(), stats.length());
}

void WebMemorySampler::stopTimerFired() {
  if (!m_is_running_)
    return;
  stop();
}
