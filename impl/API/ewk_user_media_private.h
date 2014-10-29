// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_user_media_private_h
#define ewk_user_media_private_h

#include "base/callback.h"
#include "content/public/common/media_stream_request.h"
#include "web_contents_delegate_efl.h"

#include <Evas.h>

class EWebView;

struct _Ewk_User_Media_Permission_Request {
  _Ewk_User_Media_Permission_Request(
    EWebView* ewkView,
    const content::MediaStreamRequest& media_request,
    content::WebContentsDelegateEfl* web_contents)
    : view(ewkView)
    , request(media_request)
    , web_contents(web_contents)
    , isDecided(false)
    , isSuspended(false) { }

  EWebView* view;
  const content::MediaStreamRequest& request;
  content::WebContentsDelegateEfl* web_contents;
  bool isDecided;
  bool isSuspended;
};

#endif // ewk_user_media_private_h
