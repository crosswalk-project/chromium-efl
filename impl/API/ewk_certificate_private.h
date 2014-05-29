/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef EWK_CERTIFICATE_PRIVATE_H_
#define EWK_CERTIFICATE_PRIVATE_H_

#include <string>
#include "eina_stringshare.h"

#include "base/callback.h"
#include "url/gurl.h"

struct _Ewk_Certificate_Policy_Decision {
  _Ewk_Certificate_Policy_Decision(const GURL& url, const std::string& cert, int error_code, const base::Callback<void(bool)>& result_callback)
      : url(eina_stringshare_add(url.spec().c_str())),
        certificatePem(eina_stringshare_add(cert.c_str())),
        error(error_code),
        isDecided(false),
        isSuspended(false),
        callback(result_callback) { }

  _Ewk_Certificate_Policy_Decision()
      : url(NULL),
        certificatePem(NULL),
        error(0),
        isDecided(false),
        isSuspended(false) { }

  ~_Ewk_Certificate_Policy_Decision() {
    eina_stringshare_del(url);
    eina_stringshare_del(certificatePem);
  }

  const char* url;
  const char* certificatePem;
  int error;
  bool isDecided;
  bool isSuspended;
  base::Callback<void(bool)> callback; //run when policy is set by app
};

#endif /* EWK_CERTIFICATE_PRIVATE_H_ */
