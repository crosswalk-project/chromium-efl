/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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

  CookieManager* cookieManager() const {
    return cookie_manager_.get();
  }

 private:
  explicit Ewk_Cookie_Manager(content::URLRequestContextGetterEfl* request_context_getter)
    : cookie_manager_(new CookieManager(request_context_getter)) {
  }

  scoped_refptr<CookieManager> cookie_manager_;
};

#endif // ewk_cookie_manager_private_h
