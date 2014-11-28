// Copyright 2014 Samsung Electronics. All rights reserved.
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

GeolocationPermissionContextEfl::GeolocationPermissionContextEfl()
    : weak_ptr_factory_(this) {
}

void GeolocationPermissionContextEfl::RequestPermissionOnUIThread(
    int render_process_id,
    int render_view_id,
    const GURL& requesting_frame,
    base::Callback<void(bool)> callback) const {
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

  // Permission request is sent before even creating location provider -
  // it's the only place where we can clearly ask certain view if
  // we can enable it.

  // We treat geolocation_valid as opt-out not opt-in - default behaviour is to allow
  // geolocation permission callbacks. It may be blocked by geolocation,valid smart
  // callback handlers.
  Eina_Bool geolocation_valid = EINA_TRUE;
  web_view->SmartCallback<EWebViewCallbacks::GeoLocationValid>().call(
      &geolocation_valid);

  if (geolocation_valid) {
    std::string host = requesting_frame.host();
    std::string port = requesting_frame.port();
    std::string protocol = requesting_frame.scheme();

    scoped_ptr<_Ewk_Geolocation_Permission_Request> request(
        new _Ewk_Geolocation_Permission_Request(
          host.c_str(),
          protocol.c_str(),
          atoi(port.c_str()),
          callback)
      );

    // 'callback_result' is currently unused in webkit-efl implementation.
    Eina_Bool callback_result = EINA_FALSE;
    if (!web_view->InvokeViewGeolocationPermissionCallback(request.get(), &callback_result)) {
      web_view->SmartCallback<EWebViewCallbacks::GeoLocationPermissionRequest>().call(
          request.get());
    }

    if (request->isSuspended()) {
      request.release();
      return;
    }
  }

  // Otherwise reject permission.
  callback.Run(false);
}

void GeolocationPermissionContextEfl::RequestPermission(
    int render_process_id,
    int render_view_id,
    const GURL& requesting_frame,
    base::Callback<void(bool)> callback) const {
  content::BrowserThread::PostTask(
      content::BrowserThread::UI,
      FROM_HERE,
      base::Bind(
          &GeolocationPermissionContextEfl::RequestPermissionOnUIThread,
          weak_ptr_factory_.GetWeakPtr(),
          render_process_id,
          render_view_id,
          requesting_frame,
          callback));
}

void GeolocationPermissionContextEfl::CancelPermissionRequest(
    int /*render_process_id*/,
    int /*render_view_id*/,
    int /*bridge_id*/,
    const GURL& /*requesting_frame*/) const {
  // There is currently no mechanism to inform the application
  // that a permission request should be canceled.
  // To be implemented in the future.
}

}  // namespace content
