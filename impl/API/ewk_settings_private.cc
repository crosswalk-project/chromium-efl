// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_settings_private.h"

#include "net/http/http_stream_factory.h"
#include "EdgeEffect.h"

void Ewk_Settings::setSpdyEnabled(bool flag)
{
  net::HttpStreamFactory::set_spdy_enabled(flag);
}

void Ewk_Settings::setCurrentLegacyFontSizeMode(tizen_webview::Legacy_Font_Size_Mode mode) {
  m_currentLegacyFontSizeMode = mode;
#if !defined(EWK_BRINGUP)
  m_preferences.current_legacy_font_size_mode = static_cast<content::LegacyFontSizeMode>(mode);
#endif
}

void Ewk_Settings::setDefaultTextEncoding(const char* encoding) {
  if (encoding)
    m_preferences.default_encoding = encoding;
}

void Ewk_Settings::setEdgeEffectEnabled(bool enable) {
  m_edgeEffectEnabled = enable;
#ifdef TIZEN_EDGE_EFFECT
  EdgeEffect::setSettingsEnabled(enable);
#endif
}
