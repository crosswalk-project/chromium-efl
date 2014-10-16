// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FAVICON_SERVICE_H
#define FAVICON_SERVICE_H

#include "third_party/skia/include/core/SkBitmap.h"
#include "url/gurl.h"

class FaviconDatabase;

// Use this class to store and retrieve icons in/from
// favicon database.
// It's methods can hang on mutex.
class FaviconService {
 public:
  FaviconService();

  bool SetDatabasePath(const std::string &path);
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
#if defined(OS_TIZEN_TV)
  static FaviconDatabase* GetFaviconDatabase();
#endif

 private:
  FaviconDatabase *m_database;
};

#endif // FAVICON_SERVICE_H
