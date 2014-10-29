// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/webdata/web_data_service_factory.h"

#include "eweb_view.h"
#include "base/bind.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "paths_efl.h"
#include "browser/webdata/web_data_service.h"
#include "components/autofill/core/browser/autofill_country.h"
#include "components/autofill/core/browser/webdata/autofill_table.h"
#include "components/autofill/core/browser/webdata/autofill_webdata_service.h"
#include "components/webdata/common/webdata_constants.h"
#include "content/public/browser/browser_thread.h"

using autofill::AutofillWebDataService;
using content::BrowserThread;

namespace {

// Callback to show error dialog on profile load error.
void ProfileErrorCallback(int type, sql::InitStatus status) {
  //TODO:Need to check what type of error to show
}

void InitSyncableServicesOnDBThread(
    scoped_refptr<AutofillWebDataService> autofill_web_data,
    const base::FilePath& profile_path,
    const std::string& app_locale,
    autofill::AutofillWebDataBackend* autofill_backend) {
  //TODO:Need to check if syncable service is needed
}

}  // namespace

WebDataServiceWrapper* WebDataServiceWrapper::GetInstance(){
  return Singleton<WebDataServiceWrapper>::get();
}

WebDataServiceWrapper::WebDataServiceWrapper() {
  base::FilePath db_path;
  PathService::Get(PathsEfl::WEB_DATABSE_DIR, &db_path);
#ifndef OS_TIZEN
  if(PathService::Override(base::DIR_MODULE, db_path) == false) {
    LOG(ERROR)<<"Could not access web database path.";
    return;
  }
#endif
  base::FilePath path = db_path.Append(FILE_PATH_LITERAL("FormData.db"));

  scoped_refptr<base::MessageLoopProxy> ui_thread =
      BrowserThread::GetMessageLoopProxyForThread(BrowserThread::UI);
  scoped_refptr<base::MessageLoopProxy> db_thread =
      BrowserThread::GetMessageLoopProxyForThread(BrowserThread::DB);
  web_database_ = new WebDatabaseService(path, ui_thread, db_thread);

  // All tables objects that participate in managing the database must
  // be added here.
  web_database_->AddTable(
      scoped_ptr<WebDatabaseTable>(new autofill::AutofillTable(
          EWebView::GetPlatformLocale())));

  web_database_->LoadDatabase();

  autofill_web_data_ = new AutofillWebDataService(
      web_database_, ui_thread, db_thread, base::Bind(
          &ProfileErrorCallback, 0));
  autofill_web_data_->Init();

  web_data_ = new WebDataService(
      web_database_, base::Bind(&ProfileErrorCallback,0));
  web_data_->Init();

  autofill_web_data_->GetAutofillBackend(
         base::Bind(&InitSyncableServicesOnDBThread,
                    autofill_web_data_,
                    db_path,
                    EWebView::GetPlatformLocale()));
}

WebDataServiceWrapper::~WebDataServiceWrapper() {
}

void WebDataServiceWrapper::Shutdown() {
  autofill_web_data_->ShutdownOnUIThread();
  web_data_->ShutdownOnUIThread();
  web_database_->ShutdownDatabase();
}

scoped_refptr<AutofillWebDataService>
WebDataServiceWrapper::GetAutofillWebData() {
  return autofill_web_data_.get();
}

scoped_refptr<WebDataService> WebDataServiceWrapper::GetWebData() {
  return web_data_.get();
}


// static
scoped_refptr<WebDataService> WebDataService::FromBrowserContext(
    content::BrowserContext* context) {
  WebDataServiceWrapper* wrapper = WebDataServiceFactory::GetDataService();
  if (wrapper)
    return wrapper->GetWebData();
  // |wrapper| can be NULL in Incognito mode.
  return scoped_refptr<WebDataService>(NULL);
}

WebDataServiceFactory::WebDataServiceFactory(){
  // WebDataServiceFactory has no dependecies.
}

WebDataServiceFactory::~WebDataServiceFactory() {}

// static

WebDataServiceWrapper* WebDataServiceFactory::GetDataService() {
  return WebDataServiceWrapper::GetInstance();
}

// static
scoped_refptr<AutofillWebDataService>
WebDataServiceFactory::GetAutofillWebDataForProfile() {
  WebDataServiceWrapper* wrapper =
      WebDataServiceFactory::GetDataService();
  // |wrapper| can be NULL in Incognito mode.
  return wrapper ?
      wrapper->GetAutofillWebData() :
      scoped_refptr<AutofillWebDataService>(NULL);
}

// static
WebDataServiceFactory* WebDataServiceFactory::GetInstance() {
  return Singleton<WebDataServiceFactory>::get();
}

#endif // TIZEN_AUTOFILL_SUPPORT
