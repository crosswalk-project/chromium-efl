// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/geolocation/geolocation_permission_context_efl.h"

#include "common/web_contents_utils.h"
#include "content/public/browser/browser_thread.h"
#include "eweb_view.h"
#include "eweb_view_callbacks.h"
#include "web_contents_delegate_efl.h"
#include "API/ewk_geolocation_private.h"

using web_contents_utils::WebContentsFromViewID;

namespace content {

class WebContents;

void GeolocationPermissionContextEfl::RequestGeolocationPermissionOnUIThread(int render_process_id,
                                                                             int render_view_id,
                                                                             int /*bridge_id*/,
                                                                             const GURL& requesting_frame,
                                                                             base::Callback<void(bool)> callback) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  WebContents* web_contents = WebContentsFromViewID(render_process_id, render_view_id);

  if (!web_contents)
    return;

  WebContentsDelegate* delegate = web_contents->GetDelegate();
  if (!delegate)
    return;

  WebContentsDelegateEfl* delegat_efl = static_cast<WebContentsDelegateEfl*>(delegate);
  EWebView* web_view = delegat_efl->web_view();
  if (!web_view)
    return;

  std::string host = requesting_frame.host();
  std::string port = requesting_frame.port();
  std::string protocol = requesting_frame.scheme();
  _Ewk_Geolocation_Permission_Request *request = new _Ewk_Geolocation_Permission_Request(web_view->evas_object(),
                                                                                         host.c_str(),
                                                                                         protocol.c_str(),
                                                                                         atoi(port.c_str()),
                                                                                         callback);
  web_view->SmartCallback<EWebViewCallbacks::GeoLocationPermissionRequest>().call(request);
}

void GeolocationPermissionContextEfl::RequestGeolocationPermission(int render_process_id,
                                                                   int render_view_id,
                                                                   int bridge_id,
                                                                   const GURL& requesting_frame,
                                                                   base::Callback<void(bool)> callback) {
  content::BrowserThread::PostTask(content::BrowserThread::UI,
                                   FROM_HERE,
                                   base::Bind(&GeolocationPermissionContextEfl::RequestGeolocationPermissionOnUIThread,
                                              base::Unretained(this),
                                              render_process_id,
                                              render_view_id,
                                              bridge_id,
                                              requesting_frame,
                                              callback));
}

void GeolocationPermissionContextEfl::CancelGeolocationPermissionRequest(int /*render_process_id*/,
                                                                         int /*render_view_id*/,
                                                                         int /*bridge_id*/,
                                                                         const GURL& /*requesting_frame*/) {
  // There is currently no mechanism to inform the application that a permission request should be canceled.
  // To be implemented in the future.
}

}//namespace
