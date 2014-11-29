/*
 * Copyright (C) 2014 Samsung Electronics
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ewk_quota_permission_request_private_h
#define ewk_quota_permission_request_private_h

#if !defined(EWK_BRINGUP)

#include <tizen_webview/public/tw_quota_permission_request.h>

// chromium-efl implementation for _Ewk_Quota_Permission_Request
struct _Ewk_Quota_Permission_Request : public tizen_webview::Quota_Permission_Request {
};

#endif // EWK_BRINGUP
#endif // ewk_quota_permission_request_private_h
