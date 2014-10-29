// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FAVICON_COMMANDS_H
#define FAVICON_COMMANDS_H

#include "url/gurl.h"
#include "base/memory/ref_counted.h"
#include "third_party/skia/include/core/SkBitmap.h"

class FaviconDatabasePrivate;

struct SerializedBitmap : public base::RefCounted<SerializedBitmap> {
  SerializedBitmap() : data(0), size(0) {}
  ~SerializedBitmap() {
    free();
  }

  void alloc(size_t n) {
    free();
    data = ::operator new(n);
    size = n;
  }

  void free() {
    if (data) {
      ::operator delete(data);
      data = 0;
      size = 0;
    }
  }

  void *data;
  size_t size;
};

class Command {
 public:
  Command(const std::string &cmdName, FaviconDatabasePrivate *db)
    : m_db(db), m_name(cmdName)
  {}
  virtual ~Command() {}
  virtual bool execute() = 0;
  virtual bool sqlExecute() = 0;
  virtual std::string lastError() const;
  std::string name() const;

 protected:
  static scoped_refptr<SerializedBitmap> serialize(const SkBitmap &bitmap);
  static SkBitmap deserialize(const void *data);
  static const char *sqlite3ErrStr(int rc);

  FaviconDatabasePrivate *db() const {
    return m_db;
  }

  void setError(const std::string &err);

 private:
  FaviconDatabasePrivate *m_db;
  std::string m_name;
  std::string m_lastError;
};

class InitDatabaseCommand : public Command {
 public:
  InitDatabaseCommand(FaviconDatabasePrivate *db);
  bool execute() { return sqlExecute(); }
  bool sqlExecute();
};

class LoadDatabaseCommand : public Command {
 public:
  LoadDatabaseCommand(FaviconDatabasePrivate *db);
  bool execute() { return sqlExecute(); }
  bool sqlExecute();
};

class ClearDatabaseCommand : public Command {
 public:
  ClearDatabaseCommand(FaviconDatabasePrivate *db);
  bool execute();
  bool sqlExecute();
};

class InsertFaviconURLCommand : public Command {
 public:
  InsertFaviconURLCommand(FaviconDatabasePrivate *db, const GURL &pageUrl, const GURL &faviconUrl);
  bool execute();
  bool sqlExecute();
 private:
  std::string m_pageUrl;
  std::string m_faviconUrl;
};

class UpdateFaviconURLCommand : public Command {
 public:
  UpdateFaviconURLCommand(FaviconDatabasePrivate *db, const GURL &pageUrl, const GURL &faviconUrl);
  bool execute();
  bool sqlExecute();
 private:
  std::string m_pageUrl;
  std::string m_faviconUrl;
};

class InsertBitmapCommand : public Command {
 public:
  InsertBitmapCommand(FaviconDatabasePrivate *db, const GURL &faviconUrl, const SkBitmap &bitmap);
  bool execute();
  bool sqlExecute();
 private:
  std::string m_faviconUrl;
  SkBitmap m_bitmap;
};

class UpdateBitmapCommand : public Command {
 public:
  UpdateBitmapCommand(FaviconDatabasePrivate *db, const GURL &faviconUrl, const SkBitmap &bitmap);
  bool execute();
  bool sqlExecute();
 private:
  std::string m_faviconUrl;
  SkBitmap m_bitmap;
};

class RemoveBitmapCommand : public Command {
 public:
  RemoveBitmapCommand(FaviconDatabasePrivate *db, const GURL &faviconUrl);
  bool execute();
  bool sqlExecute();
 private:
  std::string m_faviconUrl;
};

#endif // FAVICON_COMMANDS_H
