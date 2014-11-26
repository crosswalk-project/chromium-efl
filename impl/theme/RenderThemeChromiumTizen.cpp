// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "RenderThemeChromiumTizen.h"

#include "TizenUserAgentStyleSheets.h"

namespace blink {

PassRefPtr<RenderTheme> RenderThemeChromiumTizen::create()
{
    return adoptRef(new RenderThemeChromiumTizen());
}

RenderTheme& RenderTheme::theme()
{
    DEFINE_STATIC_REF(RenderTheme, renderTheme, (RenderThemeChromiumTizen::create()));
    return *renderTheme;
}

RenderThemeChromiumTizen::~RenderThemeChromiumTizen()
{
}

String RenderThemeChromiumTizen::extraDefaultStyleSheet()
{
    return RenderThemeChromiumDefault::extraDefaultStyleSheet() +
        String(themeChromiumTizenCss, sizeof(themeChromiumTizenCss));
}

} // namespace blink
