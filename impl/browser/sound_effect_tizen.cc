// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/sound_effect.h"

#include <feedback.h>

namespace {

class Player {
 public:
  Player() : feedback_library_initialized_(false) { }

  ~Player() {
    if (feedback_library_initialized_)
      feedback_deinitialize();
  }

  void play(feedback_pattern_e id) {
    if (!feedback_library_initialized_)
      feedback_library_initialized_ =
          feedback_initialize() == FEEDBACK_ERROR_NONE;

    feedback_play(id);
  }

 private:
  bool feedback_library_initialized_;
};

void playSound(feedback_pattern_e id) {
  static Player player;

  player.play(id);
}

}  // namespace

namespace sound_effect {

void playLinkEffect() {
  playSound(FEEDBACK_PATTERN_TAP);
}

} // namespace sound_effect
