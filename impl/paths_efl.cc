// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "paths_efl.h"

#include <cstdlib>

#include "base/environment.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/nix/xdg_util.h"

#if defined(OS_TIZEN)
#include <app.h>
#endif

namespace {
const base::FilePath::CharType kDataPath[] = FILE_PATH_LITERAL(DATA_DIR);
const base::FilePath::CharType kExePath[] = FILE_PATH_LITERAL(EXE_DIR);
const base::FilePath::CharType kLocalePath[] = FILE_PATH_LITERAL(LOCALE_DIR);
#if defined(OS_TIZEN)
const base::FilePath::CharType kEdjeThemesDir[] = FILE_PATH_LITERAL("themes");
const base::FilePath::CharType kApplicationDataDir[] = FILE_PATH_LITERAL("data");
const base::FilePath::CharType kApplicationCacheDir[] = FILE_PATH_LITERAL("cache");

#else
const base::FilePath::CharType kEdjeThemesDir[] = FILE_PATH_LITERAL("resources");
#endif
const base::FilePath::CharType kApplicationName[] = FILE_PATH_LITERAL("chromium-efl");
const base::FilePath::CharType kApplicationDataBaseDir[] = FILE_PATH_LITERAL("db");

#if defined(OS_TIZEN)
bool GetDirAppDataTizen(base::FilePath& result) {
#if defined(TIZEN_V_2_4)
  char* buffer;

  if (!(buffer = app_get_data_path()))
    return false;

  result = base::FilePath(buffer);
  free(buffer);
#else
  const size_t buffer_size = 512;
  char buffer[buffer_size];

  if (!app_get_data_directory(buffer, buffer_size))
    return false;

  result = base::FilePath(buffer);
#endif
  return true;
}

bool GetDirChromiumPrivateTizen(base::FilePath& result) {
#if !defined(TIZEN_LEGACY_V_2_2_1)
  scoped_ptr<char, base::FreeDeleter> data_path(app_get_data_path());
  if (data_path) {
    result = base::FilePath(data_path.get());
  } else if (!GetDirAppDataTizen(result)) {
#else
  if (!GetDirAppDataTizen(result)) {
#endif
    return false;
  }
  result = result.Append(kApplicationName);
  return true;
}

base::FilePath GetDirUserDataTizen() {
  base::FilePath path;
  if (GetDirChromiumPrivateTizen(path))
    path = path.Append(kApplicationDataDir);
  return path;
}

base::FilePath GetDirCacheTizen() {
  base::FilePath path;
  if (GetDirChromiumPrivateTizen(path))
    path = path.Append(kApplicationCacheDir);
  return path;
}
#endif

void GetDirUserData(base::FilePath& result) {
#if defined(OS_TIZEN)
  result = GetDirUserDataTizen();
  if (result.empty()) {
    scoped_ptr<base::Environment> env(base::Environment::Create());
    base::FilePath config_dir(base::nix::GetXDGDirectory(env.get(),
                              base::nix::kXdgConfigHomeEnvVar,
                              base::nix::kDotConfigDir));
    char* app_id = NULL;
    if (APP_ERROR_NONE == app_get_id(&app_id)) {
      scoped_ptr<char, base::FreeDeleter> app_name(app_id);
      result = config_dir.Append(app_name.get());
    } else {
      result = config_dir.Append(kApplicationName);
    }
  }
#else
  scoped_ptr<base::Environment> env(base::Environment::Create());
  base::FilePath config_dir(base::nix::GetXDGDirectory(env.get(),
                            base::nix::kXdgConfigHomeEnvVar,
                            base::nix::kDotConfigDir));
  result = config_dir.Append(kApplicationName);
#endif
}

void GetDirCache(base::FilePath& result) {
#if defined(OS_TIZEN)
  result = GetDirCacheTizen();
  if (result.empty()) {
    scoped_ptr<base::Environment> env(base::Environment::Create());
    base::FilePath cache_dir(base::nix::GetXDGDirectory(env.get(),
                                                        "XDG_CACHE_HOME",
                                                        ".cache"));
    char* app_id = NULL;
    if (APP_ERROR_NONE == app_get_id(&app_id)) {
      scoped_ptr<char, base::FreeDeleter> app_name(app_id);
      result = cache_dir.Append(app_name.get());
    } else {
      result = cache_dir.Append(kApplicationName);
    }
  }
#else
  scoped_ptr<base::Environment> env(base::Environment::Create());
  base::FilePath cache_dir(base::nix::GetXDGDirectory(env.get(),
                                                      "XDG_CACHE_HOME",
                                                      ".cache"));
  result = cache_dir.Append(kApplicationName);
#endif
}
} // namespace

namespace PathsEfl {
bool PathProvider(int key, base::FilePath* result) {
  DCHECK(result);
  base::FilePath cur;
  switch (key) {
#ifdef OS_TIZEN
    case base::DIR_EXE:
      *result = base::FilePath(kExePath);
      return true;
    case DIR_LOCALE:
      *result = base::FilePath(kLocalePath);
      return true;
    case EDJE_RESOURCE_DIR:
      *result = base::FilePath(kDataPath).Append(kEdjeThemesDir);
      return true;
#else
    case EDJE_RESOURCE_DIR:
      PathService::Get(base::DIR_MODULE, result);
      *result = result->Append(kEdjeThemesDir);
      return true;
#endif
    case DIR_USER_DATA:
      GetDirUserData(cur);
      break;
    case base::DIR_CACHE:
      GetDirCache(cur);
      break;
    case WEB_DATABASE_DIR:
      if (!PathService::Get(DIR_USER_DATA, &cur))
        return false;
      cur = cur.Append(kApplicationDataBaseDir);
      break;
    default:
      return false;
  }

  if (!base::DirectoryExists(cur) && !base::CreateDirectory(cur))
    return false;

  *result = cur;
  return true;
}

void Register() {
  PathService::RegisterProvider(PathProvider, PATH_START, PATH_END);
}
} // namespace PathsEfl
