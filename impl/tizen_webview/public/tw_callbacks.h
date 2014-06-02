// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_CALLBACKS_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_CALLBACKS_H_

#include <Evas.h>

namespace tizen_webview {

// from ewk_context.h
typedef void (*Local_File_System_Origins_Get_Callback)(Eina_List *origins, void *user_data);
typedef void (*Web_Application_Cache_Origins_Get_Callback)(Eina_List* origins, void* user_data);
typedef void (*Web_Application_Cache_Usage_For_Origin_Get_Callback)(int64_t usage, void* user_data);
typedef void (*Web_Database_Origins_Get_Callback)(Eina_List* origins, void* user_data);
typedef void (*Web_Storage_Origins_Get_Callback)(Eina_List* origins, void* user_data);
typedef void (*Context_Did_Start_Download_Callback)(const char* download_url, void* user_data);
typedef void (*Vibration_Client_Vibrate_Cb)(uint64_t vibration_time, void *user_data);
typedef void (*Vibration_Client_Vibration_Cancel_Cb)(void *user_data);

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_CALLBACKS_H_
