// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/webdata/web_data_service.h"

#include "base/bind.h"
#include "base/stl_util.h"
#include "components/webdata/common/web_database_service.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/notification_source.h"

using base::Bind;
using base::Time;
using content::BrowserThread;


WebDataService::WebDataService(scoped_refptr<WebDatabaseService> wdbs,
                               const ProfileErrorCallback& callback)
    : WebDataServiceBase(wdbs, callback,
          BrowserThread::GetMessageLoopProxyForThread(BrowserThread::UI)) {
}

WebDataService::WebDataService()
    : WebDataServiceBase(NULL, ProfileErrorCallback(),
          BrowserThread::GetMessageLoopProxyForThread(BrowserThread::UI)) {
}

WebDataService::~WebDataService() {
}

#endif // TIZEN_AUTOFILL_SUPPORT
