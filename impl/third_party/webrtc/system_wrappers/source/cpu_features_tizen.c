/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <inttypes.h>

// defined in third_party/webrtc/system_wrappers/interface/cpu_features_wrapper.h
enum {
  kCPUFeatureARMv7       = (1 << 0),
  kCPUFeatureVFPv3       = (1 << 1),
  kCPUFeatureNEON        = (1 << 2),
  kCPUFeatureLDREXSTREX  = (1 << 3)
};

// TODO: Need proper implementation.
uint64_t WebRtc_GetCPUFeaturesARM(void) {
  return kCPUFeatureARMv7 | kCPUFeatureNEON;
}
