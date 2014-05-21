#include "ewk_settings_private.h"

#include "net/http/http_stream_factory.h"

void Ewk_Settings::setSpdyEnabled(bool flag)
{
  net::HttpStreamFactory::set_spdy_enabled(flag);
}

