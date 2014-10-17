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

#include "browser/login_delegate_efl.h"

#include <assert.h>

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/resource_request_info.h"
#include "content/public/browser/web_contents.h"
#include "net/base/auth.h"
#include "net/url_request/url_request.h"

#include "web_contents_delegate_efl.h"
#include "common/web_contents_utils.h"

using content::BrowserThread;
using content::RenderViewHost;
using content::RenderFrameHost;
using content::ResourceDispatcherHost;
using content::ResourceRequestInfo;
using content::WebContents;

LoginDelegateEfl::LoginDelegateEfl(net::AuthChallengeInfo* auth_info, net::URLRequest* request)
    : auth_info_(auth_info),
      request_(request),
      render_process_id_(-1),
      render_frame_id_(-1) {
  bool result = ResourceRequestInfo::GetRenderFrameForRequest(request, &render_process_id_, &render_frame_id_);

  DCHECK(result);
  DCHECK(render_process_id_ != -1);
  DCHECK(render_frame_id_ != -1);

  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
      base::Bind(&LoginDelegateEfl::HandleHttpAuthRequestOnUIThread, this));
}

LoginDelegateEfl::~LoginDelegateEfl() {
}

void LoginDelegateEfl::Proceed(const char* user, const char* password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
      base::Bind(&LoginDelegateEfl::ProceedOnIOThread,
                 this, base::UTF8ToUTF16(user), base::UTF8ToUTF16(password)));
}

void LoginDelegateEfl::Cancel() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
      base::Bind(&LoginDelegateEfl::CancelOnIOThread, this));
}

void LoginDelegateEfl::HandleHttpAuthRequestOnUIThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  WebContents* web_contents = web_contents_utils::WebContentsFromFrameID(render_process_id_, render_frame_id_);
  if (!web_contents) {
    Cancel();
    return;
  }

  content::WebContentsDelegateEfl* delegate =
          static_cast<content::WebContentsDelegateEfl*>(web_contents->GetDelegate());
  DCHECK(delegate);
  delegate->OnAuthRequired(request_, auth_info_->realm, this);
}

void LoginDelegateEfl::CancelOnIOThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (request_) {
    request_->CancelAuth();
    ResourceDispatcherHost::Get()->ClearLoginDelegateForRequest(request_);
    request_ = NULL;
  }
}

void LoginDelegateEfl::ProceedOnIOThread(const base::string16& user,
                                          const base::string16& password) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (request_) {
    request_->SetAuth(net::AuthCredentials(user, password));
    ResourceDispatcherHost::Get()->ClearLoginDelegateForRequest(request_);
    request_ = NULL;
  }
}

void LoginDelegateEfl::OnRequestCancelled() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  request_ = NULL;
}

