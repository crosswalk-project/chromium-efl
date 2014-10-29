/*
 * Copyright (C) 2009-2013 Samsung Electronics. All rights reserved.
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

/**
 * @file    ewk_cookie_manager.h
 * @brief   Describes the Ewk Cookie Manager API.
 */

#ifndef ewk_cookie_manager_h
#define ewk_cookie_manager_h

#include <Eina.h>
#include "ewk_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for Ewk_Cookie_Manager */
typedef struct Ewk_Cookie_Manager Ewk_Cookie_Manager;

/**
 * \enum    Ewk_Cookie_Accept_Policy
 *
 * @brief   Contains accept policies for the cookies.
 */
enum Ewk_Cookie_Accept_Policy {
    /// Accepts every cookie sent from any page.
    EWK_COOKIE_ACCEPT_POLICY_ALWAYS,
    /// Rejects all cookies.
    EWK_COOKIE_ACCEPT_POLICY_NEVER,
    /// Accepts only cookies set by the main document loaded.
    EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY
};

/// Creates a type name for the Ewk_Cookie_Accept_Policy.
typedef enum Ewk_Cookie_Accept_Policy Ewk_Cookie_Accept_Policy;

/**
 * \enum    Ewk_Cookie_Persistent_Storage
 *
 * @brief   Enum values to denote cookies persistent storage type.
 */
enum Ewk_Cookie_Persistent_Storage {
    /// Cookies are stored in a text file in the Mozilla "cookies.txt" format.
    EWK_COOKIE_PERSISTENT_STORAGE_TEXT,
    /// Cookies are stored in a SQLite file in the current Mozilla format.
    EWK_COOKIE_PERSISTENT_STORAGE_SQLITE
};

/// Creates a type name for the Ewk_Cookie_Persistent_Storage.
typedef enum Ewk_Cookie_Persistent_Storage Ewk_Cookie_Persistent_Storage;

/**
 * @typedef Ewk_Cookie_Manager_Async_Policy_Get_Cb Ewk_Cookie_Manager_Async_Policy_Get_Cb
 * @brief Callback type for use with ewk_cookie_manager_async_accept_policy_get
 */
typedef void (*Ewk_Cookie_Manager_Async_Policy_Get_Cb)(Ewk_Cookie_Accept_Policy policy, Ewk_Error *error, void *event_info);

/**
 * @typedef Ewk_Cookie_Manager_Async_Hostnames_Get_Cb Ewk_Cookie_Manager_Async_Hostnames_Get_Cb
 * @brief Callback type for use with ewk_cookie_manager_async_hostnames_with_cookies_get
 *
 * @note The @a hostnames list items are guaranteed to be eina_stringshare. Whenever possible
 * save yourself some cpu cycles and use eina_stringshare_ref() instead of eina_stringshare_add()
 * or strdup().
 */
typedef void (*Ewk_Cookie_Manager_Async_Hostnames_Get_Cb)(Eina_List *hostnames, Ewk_Error *error, void *event_info);


/**
 * Set the @a filename where non-session cookies are stored persistently using @a storage as the format to read/write the cookies.
 *
 * Cookies are initially read from @filename to create an initial set of cookies.
 * Then, non-session cookies will be written to @filename.
 *
 * By default, @a manager doesn't store the cookies persistenly, so you need to call this
 * method to keep cookies saved across sessions.
 *
 * @param cookie_manager The cookie manager to update.
 * @param filename the filename to read to/write from.
 * @param storage the type of storage.
 */
EAPI void ewk_cookie_manager_persistent_storage_set(Ewk_Cookie_Manager *manager, const char *filename, Ewk_Cookie_Persistent_Storage storage);

/**
 * Set @a policy as the cookie acceptance policy for @a manager.
 *
 * By default, only cookies set by the main document loaded are accepted.
 *
 * @param manager The cookie manager to update.
 * @param policy a #Ewk_Cookie_Accept_Policy
 */

EAPI void ewk_cookie_manager_accept_policy_set(Ewk_Cookie_Manager *manager, Ewk_Cookie_Accept_Policy policy);

/**
 * Asynchronously get the cookie acceptance policy of @a manager.
 *
 * By default, only cookies set by the main document loaded are accepted.
 *
 * @param manager The cookie manager to query.
 * @param callback The function to call when the policy is received or an error occured.
 * @param data User data (may be @c NULL).
 */
EAPI void ewk_cookie_manager_async_accept_policy_get(const Ewk_Cookie_Manager *manager, Ewk_Cookie_Manager_Async_Policy_Get_Cb callback, void *data);

/**
 * Asynchronously get the list of host names for which @a manager contains cookies.
 *
 * @param manager The cookie manager to query.
 * @param callback The function to call when the host names have been received.
 * @param data User data (may be @c NULL).
 */
EAPI void ewk_cookie_manager_async_hostnames_with_cookies_get(const Ewk_Cookie_Manager *manager, Ewk_Cookie_Manager_Async_Hostnames_Get_Cb callback, void *data);

/**
 * Remove all cookies of @a manager for the given @a hostname.
 *
 * @param manager The cookie manager to update.
 * @param hostname A host name.
 */
EAPI void ewk_cookie_manager_hostname_cookies_clear(Ewk_Cookie_Manager *manager, const char *hostname);

/**
 * Delete all cookies of @a manager.
 *
 * @param manager The cookie manager to update.
 */
EAPI void ewk_cookie_manager_cookies_clear(Ewk_Cookie_Manager *manager);

#ifdef __cplusplus
}
#endif

#endif // ewk_cookie_manager_h
