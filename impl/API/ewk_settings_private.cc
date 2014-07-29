#include "ewk_settings_private.h"

#include "net/http/http_stream_factory.h"

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
