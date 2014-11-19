// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gesture_detection/gesture_configuration.h"

#include "base/command_line.h"
#include "ui/events/event_switches.h"

namespace ui {
namespace {

class GestureConfigurationEfl : public GestureConfiguration {
 public:
  ~GestureConfigurationEfl() override {
  }

  static GestureConfigurationEfl* GetInstance() {
    return Singleton<GestureConfigurationEfl>::get();
  }

 private:
  GestureConfigurationEfl() : GestureConfiguration() {
    set_gesture_begin_end_types_enabled(true);
  }

  friend struct DefaultSingletonTraits<GestureConfigurationEfl>;
  DISALLOW_COPY_AND_ASSIGN(GestureConfigurationEfl);
};

}  // namespace

// Create a GestureConfigurationEfl singleton instance when using aura.
GestureConfiguration* GestureConfiguration::GetInstance() {
  return GestureConfigurationEfl::GetInstance();
}

}  // namespace ui
