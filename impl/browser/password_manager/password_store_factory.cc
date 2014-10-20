// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/password_manager/password_store_factory.h"

#include "base/command_line.h"
#include "base/environment.h"
#include "base/prefs/pref_service.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "paths_efl.h"
#include "browser/webdata/web_data_service.h"
#include "browser/webdata/web_data_service_factory.h"
#include "components/browser_context_keyed_service/browser_context_dependency_manager.h"
#include "components/password_manager/core/browser/login_database.h"
#include "components/password_manager/core/browser/password_store.h"
#include "components/password_manager/core/browser/password_store_default.h"
#include "components/user_prefs/pref_registry_syncable.h"
#include "content/public/browser/browser_thread.h"

PasswordStoreService::PasswordStoreService(
    scoped_refptr<PasswordStore> password_store)
    : password_store_(password_store)
{
}

PasswordStoreService::~PasswordStoreService()
{
}

scoped_refptr<PasswordStore> PasswordStoreService::GetPasswordStore()
{
  return password_store_;
}

void PasswordStoreService::Shutdown()
{
  if (password_store_)
    password_store_->Shutdown();
}

// static
scoped_refptr<PasswordStore> PasswordStoreFactory::GetPasswordStore()
{
  PasswordStoreService* service = GetInstance()->GetService();
  if (!service)
    return NULL;
  return service->GetPasswordStore();
}

// static
PasswordStoreFactory* PasswordStoreFactory::GetInstance()
{
  return Singleton<PasswordStoreFactory>::get();
}

PasswordStoreFactory::PasswordStoreFactory()
    : service_(NULL) {
  WebDataServiceFactory::GetInstance();
  Init();
}

PasswordStoreFactory::~PasswordStoreFactory()
{
}

void PasswordStoreFactory::Init()
{
  base::FilePath db_path;
  PathService::Get(PathsEfl::WEB_DATABSE_DIR, &db_path);

#ifndef OS_TIZEN
  if(PathService::Override(base::DIR_MODULE, db_path) == false) {
    LOG(ERROR)<<"Could not access login database path.";
    return;
  }
#endif

  base::FilePath login_db_file_path = db_path.Append(FILE_PATH_LITERAL("LoginData.db"));
  LOG(ERROR) <<login_db_file_path.value();
  scoped_ptr<LoginDatabase> login_db(new LoginDatabase());
  {
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    if (!login_db->Init(login_db_file_path)) {
      LOG(ERROR) << "Could not initialize login database.";
      return;
    }
  }

  scoped_refptr<base::SingleThreadTaskRunner> main_thread_runner(
      base::MessageLoopProxy::current());
  scoped_refptr<base::SingleThreadTaskRunner> db_thread_runner(
      content::BrowserThread::GetMessageLoopProxyForThread(
          content::BrowserThread::DB));

  scoped_refptr<PasswordStore> ps;

  ps = new PasswordStoreDefault(
      main_thread_runner, db_thread_runner, login_db.release());

  if (!ps || !ps->Init()) {
    NOTREACHED() << "Could not initialize password manager.";
    return;
  }

  service_ = new PasswordStoreService(ps);
}

bool PasswordStoreFactory::ServiceIsNULLWhileTesting() const
{
  return true;
}
#endif // TIZEN_AUTOFILL_SUPPORT
