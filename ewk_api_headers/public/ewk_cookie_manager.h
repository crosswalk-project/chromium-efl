/*
 * Copyright (C) 2009-2013 Samsung Electronics
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

/**
 * @file    ewk_cookie_manager.h
 * @brief   Describes the Ewk Cookie Manager API.
 */

#ifndef ewk_cookie_manager_h
#define ewk_cookie_manager_h

#ifdef __cplusplus
extern "C" {
#endif

// sns.park> used-by : impl/cookie_manager.cc|h (efl), ewk_cookie_manager (ewk)

enum Ewk_Cookie_Accept_Policy {
    /// Accepts every cookie sent from any page.
    EWK_COOKIE_ACCEPT_POLICY_ALWAYS,
    /// Rejects all cookies.
    EWK_COOKIE_ACCEPT_POLICY_NEVER,
    /// Accepts only cookies set by the main document loaded.
    EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY
};

#ifdef __cplusplus
}
#endif

#endif // ewk_cookie_manager_h
