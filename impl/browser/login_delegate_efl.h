// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LOGIN_DELEGATE_EFL_H_
#define LOGIN_DELEGATE_EFL_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "content/public/browser/resource_dispatcher_host_login_delegate.h"

namespace net {
class AuthChallengeInfo;
class URLRequest;
}

class LoginDelegateEfl: public content::ResourceDispatcherHostLoginDelegate {
 public:
  LoginDelegateEfl(net::AuthChallengeInfo* auth_info, net::URLRequest* request);

   virtual void Proceed(const char* user, const char* password);
   virtual void Cancel();

   // from ResourceDispatcherHostLoginDelegate
   virtual void OnRequestCancelled() OVERRIDE;

 private:
  virtual ~LoginDelegateEfl();
  void HandleHttpAuthRequestOnUIThread();
  void CancelOnIOThread();
  void ProceedOnIOThread(const base::string16& user, const base::string16& password);
  void DeleteAuthHandlerSoon();

   scoped_refptr<net::AuthChallengeInfo> auth_info_;
   net::URLRequest* request_;
   int render_process_id_;
   int render_frame_id_;
};

#endif /* LOGIN_DELEGATE_EFL_H_ */
