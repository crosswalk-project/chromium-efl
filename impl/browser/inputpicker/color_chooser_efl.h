// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ColorChooserEfl_h
#define ColorChooserEfl_h

#include "content/public/browser/color_chooser.h"
#include "third_party/skia/include/core/SkColor.h"

#include <Evas.h>

namespace content {

class ColorChooser;
class WebContents;

class ColorChooserEfl : public ColorChooser {
 public:
  ColorChooserEfl(WebContents& web_contents);
  ~ColorChooserEfl();

  // ColorChooser implementation.
  virtual void SetSelectedColor(SkColor color);
  virtual void End();

 private:
  WebContents& web_contents_;
};

}

#endif // ColorChooserEfl_h
