#include "ewk_notification_private.h"
#include "tizen_webview/public/tw_url.h"
#include "tizen_webview/public/tw_security_origin.h"

using namespace tizen_webview;

_Ewk_Notification::_Ewk_Notification(
    const std::string& body,
    const std::string& iconURL,
    const std::string& replaceID,
    const std::string& title,
    uint64_t notificationID,
    const tizen_webview::URL& origin)
    : body_(body),
      iconURL_(iconURL),
      replaceID_(replaceID),
      title_(title),
#if 0
      notificationID_(notificationID),
#else
#warning "[M37] params.notification_id does not exist"
      notificationID_(0),
#endif
      securityOrigin_(new tizen_webview::Security_Origin(origin.getHost(),
                                                         origin.getProtocol(),
                                                         origin.getPort())) {
}

_Ewk_Notification::~_Ewk_Notification() {
  delete securityOrigin_;
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
