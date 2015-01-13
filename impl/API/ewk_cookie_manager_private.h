// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_cookie_manager_private_h
#define ewk_cookie_manager_private_h

#include "cookie_manager.h"

namespace content {
class URLRequestCOntextGetterEfl;
}

class Ewk_Cookie_Manager {
 public:
  static Ewk_Cookie_Manager* create(content::URLRequestContextGetterEfl* request_context_getter) {
    return (new Ewk_Cookie_Manager(request_context_getter));
  }

  ~Ewk_Cookie_Manager() { }

  base::WeakPtr<CookieManager> cookieManager() const {
    return cookie_manager_->GetWeakPtr();
  }

 private:
  explicit Ewk_Cookie_Manager(content::URLRequestContextGetterEfl* request_context_getter)
    : cookie_manager_(new CookieManager(request_context_getter)) {
  }

  scoped_ptr<CookieManager> cookie_manager_;
};

#endif // ewk_cookie_manager_private_h
