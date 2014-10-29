// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "color_chooser_efl.h"
#include "base/logging.h"
#include "web_contents_delegate_efl.h"

namespace content {

ColorChooserEfl::ColorChooserEfl(WebContents& web_contents)
    : web_contents_(web_contents) {
}

ColorChooserEfl::~ColorChooserEfl() {
}

void ColorChooserEfl::SetSelectedColor(SkColor color) {
  web_contents_.DidChooseColorInColorChooser(color);
}

void ColorChooserEfl::End() {
  web_contents_.DidEndColorChooser();
}
}
