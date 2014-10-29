// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "favicon_database.h"
#include "favicon_database_p.h"
#include "favicon_commands.h"
#include <iostream>
#include <cstdio>

const int FaviconDatabase::SYNC_DELAY = 3;

FaviconDatabase *FaviconDatabase::Instance() {
  static FaviconDatabase database;
  return &database;
}

FaviconDatabase::~FaviconDatabase() {
  if (IsOpen()) {
    Close();
  }
}

FaviconDatabase::FaviconDatabase()
  : d(new FaviconDatabasePrivate) {
}

bool FaviconDatabase::SetPath(const std::string &path) {
  base::AutoLock locker(d->mutex);
  if (d->sqlite) {
    return false;
  }
  d->path = path;
  return true;
}

bool FaviconDatabase::Open() {
  base::AutoLock locker(d->mutex);
  if (d->sqlite) {
    return true;
  }
  int result = sqlite3_open(d->path.c_str(), &d->sqlite);
  if (result != SQLITE_OK) {
    LOG(ERROR) << "[FaviconDatabase] :: Error opening SQLite database ("
               << result << ")!";
    return false;
  }
  if (!IsDatabaseInitialized()) {
    if (!InitDatabase())
      return false;
  }
  if (!LoadDatabase()) {
    return false;
  }
  return true;
}

void FaviconDatabase::Close() {
  base::AutoLock locker(d->mutex);
  if (d->sqlite) {
    sqlite3_close(d->sqlite);
    d->sqlite = 0;
  }
}

bool FaviconDatabase::IsOpen() const {
  base::AutoLock locker(d->mutex);
  return d->sqlite != 0;
}

void FaviconDatabase::SetPrivateBrowsingEnabled(bool enabled) {
  base::AutoLock locker(d->mutex);
  d->privateBrowsing = enabled;
}

bool FaviconDatabase::IsPrivateBrowsingEnabled() const {
  base::AutoLock locker(d->mutex);
  return d->privateBrowsing;
}

GURL FaviconDatabase::GetFaviconURLForPageURL(const GURL &pageUrl) const {
  base::AutoLock locker(d->mutex);
  return d->faviconUrlForPageUrl(pageUrl);
}

SkBitmap FaviconDatabase::GetBitmapForPageURL(const GURL &pageUrl) const {
  base::AutoLock locker(d->mutex);
  GURL faviconUrl = d->faviconUrlForPageUrl(pageUrl);
  if (!faviconUrl.is_valid()) {
    return SkBitmap();
  }

  return d->bitmapForFaviconUrl(faviconUrl);
}

SkBitmap FaviconDatabase::GetBitmapForFaviconURL(const GURL &iconUrl) const {
  base::AutoLock locker(d->mutex);
  return d->bitmapForFaviconUrl(iconUrl);
}

void FaviconDatabase::SetFaviconURLForPageURL(const GURL &iconUrl, const GURL &pageUrl) {
  base::AutoLock locker(d->mutex);
  if (d->privateBrowsing) {
    return;
  }
  GURL &old = d->pageToFaviconUrl[pageUrl];
  if (old == iconUrl) {
    // nothing's gonna change, so just return
    return;
  }
  if (old.is_empty()) {
    // |old| is empty when it was just inserted by operator []
    // so we just assign new value to it and return
    Command *cmd = new InsertFaviconURLCommand(d.get(), pageUrl, iconUrl);
    cmd->execute();
    d->commands.push(cmd);
    ScheduleSync();
    return;
  }

  // |old| is not empty, so we have to remove it and its bitmap
  // from 'favicon url to bitmap'
  Command *cmd = new RemoveBitmapCommand(d.get(), iconUrl);
  cmd->execute();
  d->commands.push(cmd);
  // and update it in 'page url to favicon url'
  cmd = new UpdateFaviconURLCommand(d.get(), pageUrl, iconUrl);
  cmd->execute();
  d->commands.push(cmd);

  ScheduleSync();
}

void FaviconDatabase::SetBitmapForFaviconURL(const SkBitmap &bitmap, const GURL &iconUrl) {
  base::AutoLock locker(d->mutex);
  if (d->privateBrowsing) {
    return;
  }
  if (d->faviconUrlToBitmap.find(iconUrl) != d->faviconUrlToBitmap.end()) {
    Command *cmd = new UpdateBitmapCommand(d.get(), iconUrl, bitmap);
    cmd->execute();
    d->commands.push(cmd);
    ScheduleSync();
    return;
  }
  Command *cmd = new InsertBitmapCommand(d.get(), iconUrl, bitmap);
  cmd->execute();
  d->commands.push(cmd);

  ScheduleSync();
}

bool FaviconDatabase::ExistsForPageURL(const GURL &pageUrl) const {
  base::AutoLock locker(d->mutex);
  std::map<GURL, GURL>::const_iterator it = d->pageToFaviconUrl.find(pageUrl);
  if (it == d->pageToFaviconUrl.end()) {
    return false;
  }
  return d->existsForFaviconURL(it->second);
}

bool FaviconDatabase::ExistsForFaviconURL(const GURL &iconUrl) const {
  base::AutoLock locker(d->mutex);
  return d->existsForFaviconURL(iconUrl);
}

void FaviconDatabase::Clear() {
  Command *cmd = new ClearDatabaseCommand(d.get());
  cmd->execute();
  d->commands.push(cmd);

  ScheduleSync();
}

void FaviconDatabase::SyncSQLite() {
  scoped_refptr<base::SingleThreadTaskRunner> ptr = content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::DB);
  ptr->PostTask(FROM_HERE, base::Bind(&FaviconDatabasePrivate::performSync, d->weakPtrFactory.GetWeakPtr()));
}

void FaviconDatabase::ScheduleSync() {
  if (!d->timer.IsRunning()) {
    d->timer.Start(FROM_HERE, base::TimeDelta::FromSeconds(SYNC_DELAY), this, &FaviconDatabase::SyncSQLite);
  }
}

bool FaviconDatabase::IsDatabaseInitialized() {
  std::string query("SELECT name FROM sqlite_master WHERE type='table' AND (name = ? OR name = ?);");

  sqlite3_stmt *stmt;
  int result = sqlite3_prepare_v2(d->sqlite, query.c_str(), query.size(), &stmt, 0);
  if (result != SQLITE_OK) {
    return false;
  }

  result = sqlite3_bind_text(stmt, 1, d->pageUrlToFaviconUrlTable, strlen(d->pageUrlToFaviconUrlTable), SQLITE_STATIC);
  if (result != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }
  result = sqlite3_bind_text(stmt, 2, d->faviconUrlToBitmapTable, strlen(d->faviconUrlColumn), SQLITE_STATIC);
  if (result != SQLITE_OK) {
    sqlite3_finalize(stmt);
    return false;
  }

  int count = 0;
  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    ++count;
  }

  if (result != SQLITE_DONE || count != 2) {
    sqlite3_finalize(stmt);
    return false;
  }

  sqlite3_finalize(stmt);
  return true;
}

bool FaviconDatabase::InitDatabase() {
  InitDatabaseCommand initCmd(d.get());
  return initCmd.execute();
}

bool FaviconDatabase::LoadDatabase() {
  LoadDatabaseCommand loadCmd(d.get());
  return loadCmd.execute();
}
