// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cookie_manager.h"

#include "utility"

#include "base/bind.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/stl_util.h"
#include "base/threading/thread_restrictions.h"
#include "base/synchronization/waitable_event.h"
#include "content/public/browser/browser_thread.h"
#include "eweb_context.h"
#include "net/base/net_errors.h"
#include "net/base/static_cookie_policy.h"
#include "net/cookies/canonical_cookie.h"
#include "net/cookies/cookie_util.h"
#include "net/cookies/parsed_cookie.h"
#include "net/cookies/cookie_monster.h"
#include "net/cookies/cookie_options.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "url/gurl.h"

#include <Eina.h>

#include "tizen_webview/public/tw_cookie_accept_policy.h"

using content::BrowserThread;
using net::CookieList;
using net::CookieMonster;
using base::AutoLock;

using namespace tizen_webview;

namespace {

void TriggerHostPolicyGetCallbackAsyncOnUIThread(tizen_webview::Cookie_Accept_Policy policy,
                                                 CookieManager::AsyncPolicyGetCb callback,
                                                 void *data) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  if (callback)
    (*callback)(policy, data);
}

}

class CookieManager::EwkGetHostCallback {
 public:
  EwkGetHostCallback(AsyncHostnamesGetCb callback, void* user_data)
    : callback_(callback),
      user_data_(user_data) {}

  void TriggerCallback(const net::CookieList& cookies) {
    Eina_List* hostnames = 0;
    if (cookies.size()) {
      net::CookieList::const_iterator it = cookies.begin();
      while (it != cookies.end()) {
        hostnames = eina_list_append(hostnames, eina_stringshare_add(it->Name().c_str()));
        ++it;
      }
    }
    (*callback_)(hostnames, NULL, user_data_);
    void* item = 0;
    EINA_LIST_FREE(hostnames, item)
      eina_stringshare_del(static_cast<Eina_Stringshare*>(item));
  }

 private:
    AsyncHostnamesGetCb callback_;
    void* user_data_;
};

CookieManager::CookieManager(content::URLRequestContextGetterEfl* request_context_getter)
    : is_clearing_(false),
      request_context_getter_(request_context_getter),
      cookie_policy_(TW_COOKIE_ACCEPT_POLICY_ALWAYS),
      weak_ptr_factory_(this)
{
}

void CookieManager::DeleteCookiesAsync(const std::string& url,
                                                       const std::string& cookie_name)
{
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                                    base::Bind(&CookieManager::DeleteCookiesOnIOThread,
                                                 GetWeakPtr(),
                                                 url,
                                                 cookie_name));
}

void CookieManager::DeleteCookiesOnIOThread(const std::string& url,
                                              const std::string& cookie_name) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));

  if (!request_context_getter_.get())
    return;

  scoped_refptr<net::CookieMonster> cookie_monster =
      request_context_getter_->GetURLRequestContext()->
              cookie_store()->GetCookieMonster();
  if (url.empty()) { // Delete all cookies.
    cookie_monster->DeleteAllAsync(net::CookieMonster::DeleteCallback());
  }
  GURL gurl(url);
  if (!gurl.is_valid())
    return;
  if (cookie_name.empty()) {
    // Delete all matching host cookies.
    cookie_monster->DeleteAllForHostAsync(gurl,
    net::CookieMonster::DeleteCallback());
  } else {
    // Delete all matching host and domain cookies.
    cookie_monster->DeleteCookieAsync(gurl, cookie_name, base::Closure());
  }
}

void CookieManager::SetStoragePath(const std::string& path,
                                               bool persist_session_cookies,
                                               bool file_storage_type) {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
    BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                                    base::Bind(&CookieManager::SetStoragePathOnIOThread,
                                                 GetWeakPtr(),
                                                 path,
                                                 persist_session_cookies,
                                                 file_storage_type));
}

void CookieManager::GetAcceptPolicyAsync(AsyncPolicyGetCb callback, void *data) {
  AutoLock lock(lock_);
  BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                                  base::Bind(&TriggerHostPolicyGetCallbackAsyncOnUIThread,
                                             cookie_policy_,
                                             callback,
                                             data));
}

void CookieManager::GetHostNamesWithCookiesAsync(AsyncHostnamesGetCb callback, void *data) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  host_callback_queue_.push(new EwkGetHostCallback(callback, data));
  BrowserThread::PostTask(BrowserThread::IO,
                          FROM_HERE,
                          base::Bind(&CookieManager::FetchCookiesOnIOThread,
                                     GetWeakPtr()));
}

void CookieManager::FetchCookiesOnIOThread() {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  if (!request_context_getter_.get()) {
    OnFetchComplete(net::CookieList());
    return;
  }
  scoped_refptr<net::CookieMonster> cookie_monster =
      request_context_getter_->GetURLRequestContext()->
      cookie_store()->GetCookieMonster();
  if (cookie_monster.get()) {
    cookie_monster->GetAllCookiesAsync(
        base::Bind(&CookieManager::OnFetchComplete, GetWeakPtr()));
  } else {
    OnFetchComplete(net::CookieList());
  }
}

void CookieManager::OnFetchComplete(const net::CookieList& cookies) {
  if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
    BrowserThread::PostTask(BrowserThread::UI, FROM_HERE,
                                    base::Bind(&CookieManager::OnFetchComplete,
                                                   GetWeakPtr(),
                                                   cookies));
    return;
  }
  if (!host_callback_queue_.empty()) {
    EwkGetHostCallback* host_callback = host_callback_queue_.front();
    if (host_callback) {
      host_callback->TriggerCallback(cookies);
      delete host_callback;
    }
    host_callback_queue_.pop();
  }
}

void CookieManager::SetStoragePathOnIOThread(const std::string& path,
                                             bool persist_session_cookies,
                                             bool file_storage_type) {  
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  base::FilePath storage_path(path);
  if (request_context_getter_.get())
    request_context_getter_->SetCookieStoragePath(storage_path, persist_session_cookies);
}

bool CookieManager::GetGlobalAllowAccess() {
  AutoLock lock(lock_);
  if (TW_COOKIE_ACCEPT_POLICY_ALWAYS == cookie_policy_)
    return true;
  else
    return false;
}

void CookieManager::SetCookiePolicy(tizen_webview::Cookie_Accept_Policy policy) {
  AutoLock lock(lock_);
  cookie_policy_ = policy;
}

bool CookieManager::ShouldBlockThirdPartyCookies() {
  AutoLock lock(lock_);
  if (TW_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY == cookie_policy_)
    return true;
  else
    return false;
}

bool CookieManager::AllowCookies(const GURL& url,
                                 const GURL& first_party_url,
                                 bool setting_cookie) {
  if (GetGlobalAllowAccess())
    return true;

  if (ShouldBlockThirdPartyCookies()) {
    net::StaticCookiePolicy policy(net::StaticCookiePolicy::BLOCK_ALL_THIRD_PARTY_COOKIES);
    int rv;
    if (setting_cookie)
      rv = policy.CanSetCookie(url, first_party_url);
    else
      rv = policy.CanGetCookies(url, first_party_url);

    if (net::OK == rv)
      return true;
  }

  return false;
}

bool CookieManager::OnCanGetCookies(const net::URLRequest& request,
                                    const net::CookieList& cookie_list) {
  return AllowCookies(request.url(), request.first_party_for_cookies(), false);
}

bool CookieManager::OnCanSetCookie(const net::URLRequest& request,
                                   const std::string& cookie_line,
                                   net::CookieOptions* options) {
  return AllowCookies(request.url(), request.first_party_for_cookies(), true);
}

bool CookieManager::AllowGetCookie(const GURL& url,
                                   const GURL& first_party,
                                   const net::CookieList& cookie_list,
                                   content::ResourceContext* context,
                                   int render_process_id,
                                   int render_frame_id) {
  return AllowCookies(url, first_party, false);
}

bool CookieManager::AllowSetCookie(const GURL& url,
                                   const GURL& first_party,
                                   const std::string& cookie_line,
                                   content::ResourceContext* context,
                                   int render_process_id,
                                   int render_frame_id,
                                   net::CookieOptions* options) {
  return AllowCookies(url, first_party, true);
}

void CookieManager::GetCookieValueOnIOThread(const GURL& host,
                                             std::string* result,
                                             base::WaitableEvent* completion) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
  net::CookieOptions options;
  options.set_include_httponly();

  if (!request_context_getter_.get()) {
    DCHECK(completion);
    completion->Signal();
    return;
  }

  scoped_refptr<net::CookieMonster> cookie_monster =
      request_context_getter_->GetURLRequestContext()->
      cookie_store()->GetCookieMonster();
  if (cookie_monster.get()) {
    cookie_monster->GetCookiesWithOptionsAsync(host,
                                               options,
                                               base::Bind(&CookieManager::GetCookieValueCompleted,
                                                          GetWeakPtr(),
                                                          completion,
                                                          result));
  } else {
    DCHECK(completion);
    completion->Signal();
  }
}

void CookieManager::GetCookieValueCompleted(base::WaitableEvent* completion,
                                            std::string* result,
                                            const std::string& value) {
  *result = value;
  DCHECK(completion);
  completion->Signal();
}

std::string CookieManager::GetCookiesForURL(const std::string& url) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  std::string cookie_value;
  base::WaitableEvent completion(false, false);
  BrowserThread::PostTask(BrowserThread::IO, FROM_HERE,
                                  base::Bind(&CookieManager::GetCookieValueOnIOThread,
                                                 GetWeakPtr(),
                                                 GURL(url),
                                                 &cookie_value,
                                                 &completion));
  //allow wait temporarily
#if !defined(EWK_BRINGUP)
  base::ThreadRestrictions::ScopedAllowWait allow_wait;
#endif
  completion.Wait();
  return cookie_value;
}

base::WeakPtr<CookieManager> CookieManager::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}
