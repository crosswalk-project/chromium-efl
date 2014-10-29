// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/favicon/favicon_commands.h"
#include "browser/favicon/favicon_database_p.h"
#include "third_party/sqlite/sqlite3.h"
#include "base/message_loop/message_loop_proxy.h"
#include <iostream>

#define CHECK_RESULT(qry, var, res, st, ret) if (var != res) {\
                                          sqlite3_finalize(st);\
                                          if (qry) sqlite3_free(qry);\
                                          return ret;\
                                        }

#define CHECK_RESULT_MSG(qry, var, res, st, ret) if (var != res) {\
                                              std::cerr << "ERROR! " << name() << " : " << sqlite3ErrStr(result) << std::endl;\
                                              sqlite3_finalize(st);\
                                              if (qry) sqlite3_free(qry);\
                                              setError(sqlite3ErrStr(result));\
                                              return ret;\
                                            }

#define CHECK_PREP_MSG(qry, var, res, ret) if (var != res) {\
                                              std::cerr << "ERROR! " << name() << " : " << sqlite3ErrStr(result) << std::endl;\
                                              if (qry) sqlite3_free(qry);\
                                              setError(sqlite3ErrStr(result));\
                                              return ret;\
                                            }


std::string Command::lastError() const {
  return std::string("[") + m_name + "] :: " + m_lastError;
}

std::string Command::name() const {
  return m_name;
}

void Command::setError(const std::string &err) {
  m_lastError = err;
}

scoped_refptr<SerializedBitmap> Command::serialize(const SkBitmap &bitmap) {
  scoped_refptr<SerializedBitmap> res(new SerializedBitmap);
  if (bitmap.isNull()) {
    return res;
  }
  res->alloc(bitmap.getSize() + 4 * sizeof(int));
  static_cast<int *>(res->data)[0] = static_cast<int>(bitmap.colorType());
  static_cast<int *>(res->data)[1] = static_cast<int>(bitmap.alphaType());
  static_cast<int *>(res->data)[2] = bitmap.width();
  static_cast<int *>(res->data)[3] = bitmap.height();
  if (!bitmap.copyPixelsTo(static_cast<int *>(res->data) + 4, bitmap.getSize())) {
    res->free();
  }
  return res;
}

SkBitmap Command::deserialize(const void *data) {
  if (!data) {
    return SkBitmap();
  }

  SkColorType colorType = static_cast<SkColorType>(static_cast<const int *>(data)[0]);
  SkAlphaType alphaType = static_cast<SkAlphaType>(static_cast<const int *>(data)[1]);
  int width = static_cast<const int *>(data)[1];
  int height = static_cast<const int *>(data)[2];
  SkBitmap bitmap;
  bitmap.setInfo(SkImageInfo::Make(width, height, colorType, alphaType), 0);
  bitmap.setPixels(static_cast<int *>(const_cast<void *>(data)) + 4);

  // as the |data| set to the |bitmap| are retrieved from sqlite blob,
  // this |data| would be freed by sqlite on finalize, so to be sure that
  // the deserialized SkBitmap owns and holds it's internal image data
  // for it's whole lifetime, we create a deep copy of the |bitmap|.
  SkBitmap copy;
  bitmap.copyTo(&copy, colorType);
  return copy;
}

const char *Command::sqlite3ErrStr(int rc) {
  static const char* const aMsg[] = {
    /* SQLITE_OK          */ "not an error",
    /* SQLITE_ERROR       */ "SQL logic error or missing database",
    /* SQLITE_INTERNAL    */ 0,
    /* SQLITE_PERM        */ "access permission denied",
    /* SQLITE_ABORT       */ "callback requested query abort",
    /* SQLITE_BUSY        */ "database is locked",
    /* SQLITE_LOCKED      */ "database table is locked",
    /* SQLITE_NOMEM       */ "out of memory",
    /* SQLITE_READONLY    */ "attempt to write a readonly database",
    /* SQLITE_INTERRUPT   */ "interrupted",
    /* SQLITE_IOERR       */ "disk I/O error",
    /* SQLITE_CORRUPT     */ "database disk image is malformed",
    /* SQLITE_NOTFOUND    */ "unknown operation",
    /* SQLITE_FULL        */ "database or disk is full",
    /* SQLITE_CANTOPEN    */ "unable to open database file",
    /* SQLITE_PROTOCOL    */ "locking protocol",
    /* SQLITE_EMPTY       */ "table contains no data",
    /* SQLITE_SCHEMA      */ "database schema has changed",
    /* SQLITE_TOOBIG      */ "string or blob too big",
    /* SQLITE_CONSTRAINT  */ "constraint failed",
    /* SQLITE_MISMATCH    */ "datatype mismatch",
    /* SQLITE_MISUSE      */ "library routine called out of sequence",
    /* SQLITE_NOLFS       */ "large file support is disabled",
    /* SQLITE_AUTH        */ "authorization denied",
    /* SQLITE_FORMAT      */ "auxiliary database format error",
    /* SQLITE_RANGE       */ "bind or column index out of range",
    /* SQLITE_NOTADB      */ "file is encrypted or is not a database",
  };
  rc &= 0xff;
  if (rc>=0 && rc<(int)(sizeof(aMsg)/sizeof(aMsg[0])) && aMsg[rc]!=0 ){
    return aMsg[rc];
  } else {
    return "unknown error";
  }
}


// INIT
InitDatabaseCommand::InitDatabaseCommand(FaviconDatabasePrivate *db)
  : Command("InitDatabase", db) {
}

bool InitDatabaseCommand::sqlExecute() {
  char *query = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %q (%q TEXT PRIMARY KEY, %q TEXT);",
                                db()->pageUrlToFaviconUrlTable,
                                db()->pageUrlColumn, db()->faviconUrlColumn);

  sqlite3_stmt *stmt;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);

  query = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %q (%q TEXT PRIMARY KEY REFERENCES %q (%q), %q BLOB);",
                          db()->faviconUrlToBitmapTable, db()->faviconUrlColumn,
                          db()->pageUrlToFaviconUrlTable, db()->faviconUrlColumn,
                          db()->bitmapColumn);

  result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);
  return true;
}

// LOAD
LoadDatabaseCommand::LoadDatabaseCommand(FaviconDatabasePrivate *db)
  : Command("LoadDatabase", db) {
}

bool LoadDatabaseCommand::sqlExecute()
{
  db()->pageToFaviconUrl.clear();
  db()->faviconUrlToBitmap.clear();

  char *query = sqlite3_mprintf("SELECT %q, %q FROM %q;", db()->pageUrlColumn,
                                db()->faviconUrlColumn, db()->pageUrlToFaviconUrlTable);

  sqlite3_stmt *stmt;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    GURL pageUrl(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    GURL faviconUrl(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
    db()->pageToFaviconUrl[pageUrl] = faviconUrl;
  }

  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);
  sqlite3_finalize(stmt);
  sqlite3_free(query);

  query = sqlite3_mprintf("SELECT %q, %q FROM %q;", db()->faviconUrlColumn,
                          db()->bitmapColumn, db()->faviconUrlToBitmapTable);

  result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
    GURL faviconUrl(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
    const void *data = sqlite3_column_blob(stmt, 1);
    SkBitmap bitmap = deserialize(data);
    db()->faviconUrlToBitmap[faviconUrl] = bitmap;
  }

  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);
  sqlite3_finalize(stmt);
  sqlite3_free(query);

  return true;
}

// CLEAR
ClearDatabaseCommand::ClearDatabaseCommand(FaviconDatabasePrivate *db)
  : Command("ClearDatabase", db) {
}

bool ClearDatabaseCommand::execute() {
  db()->pageToFaviconUrl.clear();
  db()->faviconUrlToBitmap.clear();
  return true;
}

bool ClearDatabaseCommand::sqlExecute() {
  char *query = sqlite3_mprintf("DELETE FROM %q; DELETE FROM %q;",
                                db()->faviconUrlToBitmapTable, db()->pageUrlToFaviconUrlTable);

  int result = sqlite3_exec(db()->sqlite, query, 0, 0, 0);
  sqlite3_free(query);
  if (result != SQLITE_OK) {
    return false;
  }
  return true;
}

// INSERT FAVICON URL
InsertFaviconURLCommand::InsertFaviconURLCommand(FaviconDatabasePrivate *db,
                                                 const GURL &pageUrl,
                                                 const GURL &faviconUrl)
  : Command("InsertFaviconURL", db),
    m_pageUrl(pageUrl.spec()),
    m_faviconUrl(faviconUrl.spec()) {
}

bool InsertFaviconURLCommand::execute() {
  db()->pageToFaviconUrl[GURL(m_pageUrl)] = GURL(m_faviconUrl);
  return true;
}

bool InsertFaviconURLCommand::sqlExecute() {
  char *query = sqlite3_mprintf("INSERT INTO %q VALUES(?, ?);", db()->pageUrlToFaviconUrlTable);

  sqlite3_stmt *stmt = 0;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  result = sqlite3_bind_text(stmt, 1, m_pageUrl.c_str(), m_pageUrl.size(), SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);
  result = sqlite3_bind_text(stmt, 2, m_faviconUrl.c_str(), m_faviconUrl.size(), SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);
  return true;
}

// UPDATE FAVICON URL
UpdateFaviconURLCommand::UpdateFaviconURLCommand(FaviconDatabasePrivate *db,
                                                 const GURL &pageUrl,
                                                 const GURL &faviconUrl)
  : Command("UpdateFaviconURL", db),
    m_pageUrl(pageUrl.spec()),
    m_faviconUrl(faviconUrl.spec()) {
}

bool UpdateFaviconURLCommand::execute() {
  db()->pageToFaviconUrl[GURL(m_pageUrl)] = GURL(m_faviconUrl);
  return true;
}

bool UpdateFaviconURLCommand::sqlExecute() {
  char *query = sqlite3_mprintf("UPDATE %q SET %q = ? WHERE %q = ? ;",
                                db()->pageUrlToFaviconUrlTable, db()->faviconUrlColumn,
                                db()->pageUrlColumn);

  sqlite3_stmt *stmt = 0;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  result = sqlite3_bind_text(stmt, 1, m_pageUrl.c_str(), m_pageUrl.size(), SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);
  result = sqlite3_bind_text(stmt, 2, m_faviconUrl.c_str(), m_faviconUrl.size(), SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);
  return true;
}

// INSERT BITMAP
InsertBitmapCommand::InsertBitmapCommand(FaviconDatabasePrivate *db,
                                         const GURL &faviconUrl,
                                         const SkBitmap &bitmap)
  : Command("InsertBitmap", db),
    m_faviconUrl(faviconUrl.spec()),
    m_bitmap(bitmap) {
}

bool InsertBitmapCommand::execute() {
  db()->faviconUrlToBitmap[GURL(m_faviconUrl)] = m_bitmap;
  return true;
}

bool InsertBitmapCommand::sqlExecute() {
  char *query = sqlite3_mprintf("INSERT INTO %q VALUES (?, ?);", db()->faviconUrlToBitmapTable);

  sqlite3_stmt *stmt = 0;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  result = sqlite3_bind_text(stmt, 1, m_faviconUrl.c_str(), m_faviconUrl.size(), SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);

  scoped_refptr<SerializedBitmap> serializedBitmap = serialize(m_bitmap);
  if (!serializedBitmap->data) {
    sqlite3_finalize(stmt);
    sqlite3_free(query);
    return false;
  }

  result = sqlite3_bind_blob(stmt, 2, serializedBitmap->data, serializedBitmap->size, SQLITE_TRANSIENT);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);
  return true;
}

// UPDATE BITMAP
UpdateBitmapCommand::UpdateBitmapCommand(FaviconDatabasePrivate *db,
                                         const GURL &faviconUrl,
                                         const SkBitmap &bitmap)
  : Command("UpdateBitmap", db),
    m_faviconUrl(faviconUrl.spec()),
    m_bitmap(bitmap) {
}

bool UpdateBitmapCommand::execute() {
  db()->faviconUrlToBitmap[GURL(m_faviconUrl)] = m_bitmap;
  return true;
}

bool UpdateBitmapCommand::sqlExecute() {
  char *query = sqlite3_mprintf("UPDATE %q SET %q = ? WHERE %q = ? ;",
                                db()->faviconUrlToBitmapTable, db()->bitmapColumn,
                                db()->faviconUrlColumn);

  sqlite3_stmt *stmt = 0;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  scoped_refptr<SerializedBitmap> serializedBitmap = serialize(m_bitmap);
  if (!serializedBitmap->data) {
    sqlite3_finalize(stmt);
    return false;
  }

  result = sqlite3_bind_blob(stmt, 1, serializedBitmap->data, serializedBitmap->size, SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);

  result = sqlite3_bind_text(stmt, 2, m_faviconUrl.c_str(), m_faviconUrl.size(), SQLITE_STATIC);
  CHECK_RESULT_MSG(query, result, SQLITE_OK, stmt, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);
  return true;
}

// REMOVE BITMAP
RemoveBitmapCommand::RemoveBitmapCommand(FaviconDatabasePrivate *db, const GURL &faviconUrl)
  : Command("RemoveBitmap", db),
    m_faviconUrl(faviconUrl.spec()) {
}

bool RemoveBitmapCommand::execute() {
  std::map<GURL, SkBitmap>::iterator it = db()->faviconUrlToBitmap.find(GURL(m_faviconUrl));
  if (it != db()->faviconUrlToBitmap.end()) {
    db()->faviconUrlToBitmap.erase(it);
  }
  return true;
}

bool RemoveBitmapCommand::sqlExecute() {
  char *query = sqlite3_mprintf("DELETE FROM %q WHERE %q = ? ;", db()->faviconUrlToBitmapTable, db()->faviconUrlColumn);

  sqlite3_stmt *stmt = 0;
  int result = sqlite3_prepare_v2(db()->sqlite, query, -1, &stmt, 0);
  CHECK_PREP_MSG(query, result, SQLITE_OK, false);

  result = sqlite3_step(stmt);
  CHECK_RESULT_MSG(query, result, SQLITE_DONE, stmt, false);

  sqlite3_finalize(stmt);
  sqlite3_free(query);
  return true;
}
