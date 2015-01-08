// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_CERTIFICATE_PRIVATE_H_
#define EWK_CERTIFICATE_PRIVATE_H_

#include <string>

#include "ewk_suspendable_object.h"
#include "url/gurl.h"

class _Ewk_Certificate_Policy_Decision : public Ewk_Suspendable_Object {
 public:
  _Ewk_Certificate_Policy_Decision(const GURL& url, const std::string& cert, int error_code, const base::Callback<void(bool)>& result_callback)
      : Ewk_Suspendable_Object(result_callback),
        error_(error_code) {
    url_ = eina_stringshare_add(url.spec().c_str());
    certificatePem_ = eina_stringshare_add(cert.c_str());
  }

  ~_Ewk_Certificate_Policy_Decision() {
    eina_stringshare_del(url_);
    eina_stringshare_del(certificatePem_);
  }

  Eina_Stringshare* url() const { return url_; }
  Eina_Stringshare* certificatePem() const { return certificatePem_; }
  int error() const { return error_; }

 private:
  Eina_Stringshare* url_ = nullptr;
  Eina_Stringshare* certificatePem_ = nullptr;
  int error_;
};

#endif /* EWK_CERTIFICATE_PRIVATE_H_ */
