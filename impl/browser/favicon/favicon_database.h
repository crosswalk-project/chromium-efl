#ifndef FAVICON_DATABASE_H
#define FAVICON_DATABASE_H

#include <string>
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/sqlite/sqlite3.h"
#include "url/gurl.h"
#include "base/memory/weak_ptr.h"
#include "base/macros.h"

struct FaviconDatabasePrivate;
class FaviconService;

// This class implements favicon database, but it should
// not be used directly. If you want to use favicon database,
// then FaviconService is the class you need.
class FaviconDatabase {
 public:
  static FaviconDatabase *Instance();

  virtual ~FaviconDatabase();

  bool SetPath(const std::string &path);
  bool Open();
  void Close();
  bool IsOpen() const;
  void SetPrivateBrowsingEnabled(bool enabled);
  bool IsPrivateBrowsingEnabled() const;

  GURL GetFaviconURLForPageURL(const GURL &pageUrl) const;
  SkBitmap GetBitmapForPageURL(const GURL &pageUrl) const;
  SkBitmap GetBitmapForFaviconURL(const GURL &iconUrl) const;

  void SetFaviconURLForPageURL(const GURL &iconUrl, const GURL &pageUrl);
  void SetBitmapForFaviconURL(const SkBitmap &bitmap, const GURL &iconUrl);
  bool ExistsForPageURL(const GURL &pageUrl) const;
  bool ExistsForFaviconURL(const GURL &iconUrl) const;
  void Clear();

 private:
  FaviconDatabase();
  DISALLOW_COPY_AND_ASSIGN(FaviconDatabase);

  void SyncSQLite();
  void ScheduleSync();
  bool IsDatabaseInitialized();
  bool InitDatabase();
  bool LoadDatabase();

  scoped_refptr<FaviconDatabasePrivate> d;
  static const int SYNC_DELAY;

  friend class FaviconService;
};

#endif // FAVICON_DATABASE_H
