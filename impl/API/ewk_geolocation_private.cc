#include "ewk_geolocation_private.h"
#include "tizen_webview/public/tw_security_origin.h"


_Ewk_Geolocation_Permission_Request::_Ewk_Geolocation_Permission_Request(
    Evas_Object* ewkView, const char* host, const char* protocol, uint16_t port,
    base::Callback<void(bool)> inCallback)
  : ewkView(ewkView)
  , origin(new tizen_webview::Security_Origin(host, protocol, port))
  , isDecided(false)
  , isSuspended(false)
  , callback(inCallback) {
}

_Ewk_Geolocation_Permission_Request::~_Ewk_Geolocation_Permission_Request() {
  delete origin;
}
