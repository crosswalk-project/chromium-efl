// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEB_DATA_SERVICE_FACTORY_H
#define WEB_DATA_SERVICE_FACTORY_H

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/memory/singleton.h"
#include "components/webdata/common/web_database_service.h"

class WebDataService;

namespace autofill {
class AutofillWebDataService;
}  // namespace autofill

class WebDataServiceWrapper{
 public:
  explicit WebDataServiceWrapper();

  virtual ~WebDataServiceWrapper();

  virtual void Shutdown() OVERRIDE;

  virtual scoped_refptr<autofill::AutofillWebDataService> GetAutofillWebData();

  virtual scoped_refptr<WebDataService> GetWebData();

  static WebDataServiceWrapper* GetInstance();
 private:
  scoped_refptr<WebDatabaseService> web_database_;

  scoped_refptr<autofill::AutofillWebDataService> autofill_web_data_;
  scoped_refptr<WebDataService> web_data_;

  DISALLOW_COPY_AND_ASSIGN(WebDataServiceWrapper);
};

// Singleton that owns all WebDataServiceWrappers
class WebDataServiceFactory {
 public:

  static WebDataServiceWrapper* GetDataService();

  static scoped_refptr<autofill::AutofillWebDataService>
      GetAutofillWebDataForProfile();

  static WebDataServiceFactory* GetInstance();

 private:
  friend struct DefaultSingletonTraits<WebDataServiceFactory>;

  WebDataServiceFactory();
  virtual ~WebDataServiceFactory();

  DISALLOW_COPY_AND_ASSIGN(WebDataServiceFactory);
};

#endif // TIZEN_AUTOFILL_SUPPORT

#endif  // WEB_DATA_SERVICE_FACTORY_H

