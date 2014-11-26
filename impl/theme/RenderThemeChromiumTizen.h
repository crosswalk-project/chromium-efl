// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RenderThemeChromiumTizen_h
#define RenderThemeChromiumTizen_h

#include "core/rendering/RenderThemeChromiumDefault.h"

namespace blink {

class RenderThemeChromiumTizen final : public RenderThemeChromiumDefault {
public:
    static PassRefPtr<RenderTheme> create();
    virtual String extraDefaultStyleSheet() override;

    virtual bool delegatesMenuListRendering() const override { return true; }

private:
    virtual ~RenderThemeChromiumTizen();
};

} // namespace blink

#endif // RenderThemeChromiumTizen_h
