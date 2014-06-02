/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef POLICY_NAVIGATION_PARAMS_H_
#define POLICY_NAVIGATION_PARAMS_H_

#include "content/public/common/referrer.h"
#include "third_party/WebKit/public/web/WebNavigationPolicy.h"
#include "third_party/WebKit/public/web/WebNavigationType.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "url/gurl.h"

struct NavigationPolicyParams {
  int render_view_id;
  GURL url;
  content::Referrer referrer;
  blink::WebNavigationPolicy policy;
  blink::WebNavigationType type;
  blink::WebString auth;
  bool should_replace_current_entry;
  bool is_main_frame;
  bool is_redirect;
};

#endif /* POLICY_NAVIGATION_PARAMS_H_ */
