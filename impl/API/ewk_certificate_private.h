// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
