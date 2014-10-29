// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef WEB_DATA_SERVICE_H
#define WEB_DATA_SERVICE_H

#ifdef TIZEN_AUTOFILL_SUPPORT

#include <map>
#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/files/file_path.h"
#include "base/location.h"
#include "base/memory/ref_counted.h"
#include "base/sequenced_task_runner_helpers.h"
#include "components/webdata/common/web_data_results.h"
#include "components/webdata/common/web_data_service_base.h"
#include "components/webdata/common/web_data_service_consumer.h"
#include "components/webdata/common/web_database.h"

class GURL;
class WebDatabaseService;

namespace base {
class Thread;
}

namespace content {
class BrowserContext;
}

////////////////////////////////////////////////////////////////////////////////
//
// WebDataService is a generic data repository for meta data associated with
// web pages. All data is retrieved and archived in an asynchronous way.
//
// All requests return a handle. The handle can be used to cancel the request.
//
////////////////////////////////////////////////////////////////////////////////

typedef base::Callback<scoped_ptr<WDTypedResult>(void)> ResultTask;

class WebDataServiceConsumer;

class WebDataService : public WebDataServiceBase {
 public:
  // Retrieve a WebDataService for the given context.
  static scoped_refptr<WebDataService> FromBrowserContext(
      content::BrowserContext* context);

  WebDataService(scoped_refptr<WebDatabaseService> wdbs,
                 const ProfileErrorCallback& callback);

 protected:
  WebDataService();
  virtual ~WebDataService();

 private:
  DISALLOW_COPY_AND_ASSIGN(WebDataService);
};

#endif // TIZEN_AUTOFILL_SUPPORT

#endif  // WEB_DATA_SERVICE_H
