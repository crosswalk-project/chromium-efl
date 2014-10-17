/*
 * Copyright (C) 2014 Samsung Electronics
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
