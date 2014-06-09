#include "ewk_notification_private.h"
#include "tizen_webview/public/tw_security_origin.h"

_Ewk_Notification::_Ewk_Notification(
    const content::ShowDesktopNotificationHostMsgParams& params)
  : body(eina_stringshare_add((base::UTF16ToUTF8(params.body)).c_str()))
  , iconURL(eina_stringshare_add((params.icon_url).spec().c_str()))
  , replaceID(eina_stringshare_add((base::UTF16ToUTF8(params.replace_id)).c_str()))
  , title(eina_stringshare_add((base::UTF16ToUTF8(params.title)).c_str()))
#if 0
  , notificationID(params.notification_id)
#else
#warning "[M37] params.notification_id does not exist"
  , notificationID(0)
#endif
  , securityOrigin(new tizen_webview::Security_Origin(
      params.origin.host().c_str(), params.origin.scheme().c_str(),
      atoi(params.origin.port().c_str()))) {
}

_Ewk_Notification::~_Ewk_Notification() {
  eina_stringshare_del(body);
  eina_stringshare_del(iconURL);
  eina_stringshare_del(replaceID);
  eina_stringshare_del(title);
  delete securityOrigin;
}

_Ewk_Notification_Permission_Request::_Ewk_Notification_Permission_Request(
    Evas_Object* ewkView, int callback, const GURL& source_origin)
  : ewkView(ewkView)
  , origin(new tizen_webview::Security_Origin(source_origin.host().c_str(),
       source_origin.scheme().c_str(), atoi(source_origin.port().c_str())))
  , isDecided(false)
  , isSuspended(false)
  , callback_context(callback) {
}

_Ewk_Notification_Permission_Request::~_Ewk_Notification_Permission_Request() {
  delete origin;
}
