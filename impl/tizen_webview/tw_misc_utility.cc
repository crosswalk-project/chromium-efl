// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_misc_utility.h"

#include <sstream>

namespace tizen_webview {

GURL GetGURL(const URL& url) {
  std::ostringstream ss;
  ss << url.getProtocol() << "://" << url.getHost();
  if (!url.isPortUnspecified())
    ss << ":" << url.getPort();
  GURL result(ss.str());
  assert(result.is_valid());
  return result;
}

URL GetURL(const GURL& url) {
  return URL(url.scheme(), url.host(), url.IntPort());
}

} // namespace tizen_webview
