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

#include "ewk_cookie_manager.h"

// TODO: remove dependency
#include <../impl/API/ewk_cookie_manager_private.h>
#include <../impl/cookie_manager.h>

#include "public/ewk_log.h"
#include "private/chromium_glue.h"

#define EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager, ...)   \
  if (!(manager)) {                                                      \
    CRITICAL("ewk cookie manager is NULL.");                            \
    return __VA_ARGS__;                                                 \
  }                                                                     \
  if (!((manager)->cookieManager())) {                                  \
    CRITICAL("ewk cookie manager->cookieManager() is NULL.");           \
    return __VA_ARGS__;                                                 \
  }                                                                     \
  CookieManager* cookie_manager = (manager)->cookieManager()


void ewk_cookie_manager_persistent_storage_set(Ewk_Cookie_Manager* manager,
                                               const char* file_name,
                                               Ewk_Cookie_Persistent_Storage storage)
{
  EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager);
  EINA_SAFETY_ON_NULL_RETURN(file_name);
  std::string fileName(file_name);
  switch(storage) {
    case EWK_COOKIE_PERSISTENT_STORAGE_SQLITE:
      cookie_manager->SetStoragePath(file_name, true, false);
      break;
    case EWK_COOKIE_PERSISTENT_STORAGE_TEXT:
      cookie_manager->SetStoragePath(file_name, true);
      break;
    default:
      //NOTREACHED() << "ewk_cookie_manager_persistent_storage_set invalid storage type";
      break;
  }
}

void ewk_cookie_manager_accept_policy_set(Ewk_Cookie_Manager* manager,
                                          Ewk_Cookie_Accept_Policy policy)
{
  EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager);
  cookie_manager->SetCookiePolicy(chromium_glue::to(policy));
}

void ewk_cookie_manager_async_accept_policy_get(const Ewk_Cookie_Manager* manager,
                                                Ewk_Cookie_Manager_Async_Policy_Get_Cb callback,
                                                void* data)
{
  EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager);
  EINA_SAFETY_ON_NULL_RETURN(callback);

  // WARNING (sns.park): 
  // reinterpret_cast is potentially unsafe because enum values of ewk and chromium
  // may diverge in the future. In other words, the below code is valid only under
  // the assumption the enum values for Ewk_Cookie_Accept_Policy be aligned with
  // tizen_webview::Cookie_Accept_Policy.
  // TODO: replace reinterpret_cast with better solution
  // assert the above assumption in private/api_static_check.cc
  cookie_manager->GetAcceptPolicyAsync(reinterpret_cast<CookieManager::AsyncPolicyGetCb>(callback), data);
}

void ewk_cookie_manager_async_hostnames_with_cookies_get(const Ewk_Cookie_Manager* manager,
                                                         Ewk_Cookie_Manager_Async_Hostnames_Get_Cb callback,
                                                         void* data)
{
  EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager);
  EINA_SAFETY_ON_NULL_RETURN(callback);
  cookie_manager->GetHostNamesWithCookiesAsync(callback, data);
}

void ewk_cookie_manager_hostname_cookies_clear(Ewk_Cookie_Manager* manager,
                                               const char* host_name)
{
  EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager);
  EINA_SAFETY_ON_NULL_RETURN(host_name);
  std::string url(host_name);
  cookie_manager->DeleteCookiesAsync(url);
}

void ewk_cookie_manager_cookies_clear(Ewk_Cookie_Manager* manager)
{
  EWK_COOKIE_MANAGER_GET_OR_RETURN(manager, cookie_manager);
  cookie_manager->DeleteCookiesAsync();
}

