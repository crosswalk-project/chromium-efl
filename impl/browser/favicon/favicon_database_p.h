#ifndef FAVICON_DATABASE_P_H
#define FAVICON_DATABASE_P_H

#include <map>
#include <queue>
#include "url/gurl.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/sqlite/sqlite3.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/timer/timer.h"
#include "base/memory/weak_ptr.h"
#include "content/public/browser/browser_thread.h"
#include "favicon_database.h"

class Command;

struct FaviconDatabasePrivate : public base::RefCountedThreadSafe<FaviconDatabasePrivate> {
  FaviconDatabasePrivate()
    : path("/tmp/favicon_database.db"),
      privateBrowsing(false),
      sqlite(0),
      weakPtrFactory(this)
  {}

  GURL faviconUrlForPageUrl(const GURL &pageUrl) const;
  SkBitmap bitmapForFaviconUrl(const GURL &faviconUrl) const;

  bool existsForFaviconURL(const GURL &faviconUrl) const;

  scoped_refptr<base::SingleThreadTaskRunner> taskRunner() const;
  void performSync();

  std::map<GURL, GURL> pageToFaviconUrl;
  std::map<GURL, SkBitmap> faviconUrlToBitmap;
  std::string path;
  bool privateBrowsing;

  sqlite3 *sqlite;
  base::Lock mutex;
  base::RepeatingTimer<FaviconDatabase> timer;
  std::queue<Command *> commands;
  base::WeakPtrFactory<FaviconDatabasePrivate> weakPtrFactory;

  static const char *pageUrlToFaviconUrlTable;
  static const char *faviconUrlToBitmapTable;
  static const char *pageUrlColumn;
  static const char *faviconUrlColumn;
  static const char *bitmapColumn;
};

#endif // FAVICON_DATABASE_P_H
