// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/favicon/favicon_service.h"
#include "browser/favicon/favicon_database.h"
#include "browser/favicon/favicon_database_p.h"
#include "base/synchronization/lock.h"

#define CHECK_OPEN if (!m_database->Open()) {\
                     return;\
                   }

#define CHECK_OPEN_RET(ret) if (!m_database->Open()) {\
                          return ret;\
                        }

FaviconService::FaviconService()
  : m_database(FaviconDatabase::Instance()) {
}

bool FaviconService::SetDatabasePath(const std::string& path) {
  return m_database->SetPath(path);
}

void FaviconService::SetPrivateBrowsingEnabled(bool enabled) {
  m_database->SetPrivateBrowsingEnabled(enabled);
}

bool FaviconService::IsPrivateBrowsingEnabled() const {
  return m_database->IsPrivateBrowsingEnabled();
}

GURL FaviconService::GetFaviconURLForPageURL(const GURL& pageUrl) const {
  CHECK_OPEN_RET(GURL());
  return m_database->GetFaviconURLForPageURL(pageUrl);
}

SkBitmap FaviconService::GetBitmapForPageURL(const GURL& pageUrl) const {
  CHECK_OPEN_RET(SkBitmap());
  return m_database->GetBitmapForPageURL(pageUrl);
}

SkBitmap FaviconService::GetBitmapForFaviconURL(const GURL& iconUrl) const {
  CHECK_OPEN_RET(SkBitmap());
  return m_database->GetBitmapForFaviconURL(iconUrl);
}

void FaviconService::SetFaviconURLForPageURL(const GURL& iconUrl, const GURL& pageUrl) {
  CHECK_OPEN;
  m_database->SetFaviconURLForPageURL(iconUrl, pageUrl);
}

void FaviconService::SetBitmapForFaviconURL(const SkBitmap& bitmap, const GURL& iconUrl) {
  CHECK_OPEN;
  m_database->SetBitmapForFaviconURL(bitmap, iconUrl);
}

bool FaviconService::ExistsForPageURL(const GURL& pageUrl) const {
  CHECK_OPEN_RET(false);
  return m_database->ExistsForPageURL(pageUrl);
}

bool FaviconService::ExistsForFaviconURL(const GURL& iconUrl) const {
  CHECK_OPEN_RET(false);
  return m_database->ExistsForFaviconURL(iconUrl);
}

void FaviconService::Clear() {
  CHECK_OPEN;
  m_database->Clear();
}

#if defined(OS_TIZEN_TV)
FaviconDatabase* FaviconService::GetFaviconDatabase() {
  return FaviconDatabase::Instance();
}
#endif
