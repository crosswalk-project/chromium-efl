// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef cookie_manager_h
#define cookie_manager_h

#include "base/synchronization/lock.h"
#include "base/basictypes.h"
#include "url_request_context_getter_efl.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_getter.h"
#include "net/cookies/canonical_cookie.h"
#include "net/cookies/cookie_options.h"
#include "net/cookies/cookie_util.h"
#include "url/gurl.h"

#include <queue>
#include <Eina.h>

#include "tizen_webview/public/tw_cookie_accept_policy.h"

namespace content {
class ResourceContext;
}

struct _Ewk_Error;

class CookieManager 
  : public base::RefCountedThreadSafe<CookieManager> {
 public:
  typedef void (*AsyncPolicyGetCb)(tizen_webview::Cookie_Accept_Policy,
                                   void *);
  typedef void (*AsyncHostnamesGetCb)(Eina_List*, _Ewk_Error*, void *);

  explicit CookieManager(
      content::URLRequestContextGetterEfl* request_context_getter);
  // Delete all cookies that match the specified parameters. If both |url| and
  // values |cookie_name| are specified all host and domain cookies matching
  // both will be deleted. If only |url| is specified all host cookies (but not
  // domain cookies) irrespective of path will be deleted. If |url| is empty
  // all cookies for all hosts and domains will be deleted. This method must be
  // called on the IO thread.
  void DeleteCookiesAsync(const std::string& url = std::string(),
                          const std::string& cookie_name = std::string());
  // Sets the directory path that will be used for storing cookie data. If
  // |path| is empty data will be stored in memory only. Otherwise, data will
  // be stored at the specified |path|. To persist session cookies (cookies
  // without an expiry date or validity interval) set |persist_session_cookies|
  // to true. Session cookies are generally intended to be transient and most
  // Web browsers do not persist them. Returns false if cookies cannot be
  // accessed.
  void SetStoragePath(const std::string& path,
                      bool persist_session_cookies,
                      bool file_storage_type=true);
  //get the accept policy asynchronous
  void GetAcceptPolicyAsync(AsyncPolicyGetCb callback, void *data);
  //get host name asynchronous
  void GetHostNamesWithCookiesAsync(AsyncHostnamesGetCb callback, void *data);

  // These manage the global access state shared across requests regardless of
  // source (i.e. network or JavaScript).
  bool GetGlobalAllowAccess();
  void SetCookiePolicy(tizen_webview::Cookie_Accept_Policy policy);
  // These are the functions called when operating over cookies from the
  // network. See NetworkDelegate for further descriptions.
  bool OnCanGetCookies(const net::URLRequest& request,
                       const net::CookieList& cookie_list);
  bool OnCanSetCookie(const net::URLRequest& request,
                      const std::string& cookie_line,
                      net::CookieOptions* options);

  // These are the functions called when operating over cookies from the
  // renderer. See ContentBrowserClient for further descriptions.
  bool AllowGetCookie(const GURL& url,
                      const GURL& first_party,
                      const net::CookieList& cookie_list,
                      content::ResourceContext* context,
                      int render_process_id,
                      int render_frame_id);
  bool AllowSetCookie(const GURL& url,
                      const GURL& first_party,
                      const std::string& cookie_line,
                      content::ResourceContext* context,
                      int render_process_id,
                      int render_frame_id,
                      net::CookieOptions* options);
  bool ShouldBlockThirdPartyCookies();
  //This is synchronous call
  std::string GetCookiesForURL(const std::string& url);

 protected:
  friend class base::RefCountedThreadSafe<CookieManager>;

  ~CookieManager()
  {}

 private:
  struct EwkGetHostCallback;

  // Deletes cookie having host name. This must be called in IO thread.
  void DeleteCookiesOnIOThread(const std::string& url,
                               const std::string& cookie_name) ;
  void SetStoragePathOnIOThread(const std::string& path,
                                bool persist_session_cookies,
                                bool file_storage_type);
  bool AllowCookies(const GURL& url,
                    const GURL& first_party_url,
                    bool setting_cookie);
  // Fetch the cookies. This must be called in the IO thread.
  void FetchCookiesOnIOThread();
  void OnFetchComplete(const net::CookieList& cookies);

  void GetCookieValueOnIOThread(const GURL& host,
                                std::string* result,
                                base::WaitableEvent* completion);
  void GetCookieValueCompleted(base::WaitableEvent* completion,
                               std::string* result,
                               const std::string& value);
  // Indicates whether or not we're currently clearing information:
  // it's true when ClearCookie() is called in the UI thread, and it's reset
  // after we notify the callback in the UI thread.
  // This only mutates on the UI thread.
  bool is_clearing_;
  scoped_refptr<content::URLRequestContextGetterEfl> request_context_getter_;
  //cookie policy information
  base::Lock lock_;
  tizen_webview::Cookie_Accept_Policy cookie_policy_;
  // This only mutates on the UI thread.
  std::queue< EwkGetHostCallback* > host_callback_queue_;

  DISALLOW_COPY_AND_ASSIGN(CookieManager);
};

#endif //cookie_manager_h
