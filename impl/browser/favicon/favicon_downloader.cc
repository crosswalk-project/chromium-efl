// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "favicon_downloader.h"
#include "base/bind.h"
#include "content/public/browser/web_contents.h"
#include "third_party/skia/include/core/SkBitmap.h"

FaviconDownloader::FaviconDownloader(content::WebContents *webContents,
                                     const GURL &faviconUrl,
                                     FaviconDownloader::FaviconDownloaderCallback callback)
  : content::WebContentsObserver(webContents),
    m_faviconUrl(faviconUrl),
    m_callback(callback),
    m_weakPtrFactory(this) {
}

void FaviconDownloader::Start() {
  DownloadFavicon(m_faviconUrl);
}

int FaviconDownloader::DownloadFavicon(const GURL &faviconUrl) {
  return web_contents()->DownloadImage(faviconUrl,
                                       true,
                                       0,
                                       base::Bind(&FaviconDownloader::DidDownloadFavicon,
                                                  m_weakPtrFactory.GetWeakPtr()));
}

void FaviconDownloader::DidDownloadFavicon(int id,
                                           int httpStatusCode,
                                           const GURL &imageUrl,
                                           const std::vector<SkBitmap> &bitmaps,
                                           const std::vector<gfx::Size> &originalBitmapSizes) {
  if (bitmaps.empty()) {
    m_callback.Run(false, GURL(), SkBitmap());
    return;
  }
  m_callback.Run(true, imageUrl, bitmaps.front());
}
