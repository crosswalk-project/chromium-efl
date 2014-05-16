#ifndef FAVICON_DOWNLOADER_H
#define FAVICON_DOWNLOADER_H

#include "content/public/browser/web_contents_observer.h"
#include "base/callback.h"
#include "base/memory/weak_ptr.h"

class SkBitmap;

namespace gfx {
class Size;
}

// Class used for downloading favicons
class FaviconDownloader : public content::WebContentsObserver {
 public:
  typedef base::Callback<void(
          bool success,
          const GURL &faviconUrl,
          const SkBitmap &bitmap)>
      FaviconDownloaderCallback;
  FaviconDownloader(content::WebContents *webContents,
                    const GURL &faviconUrl,
                    FaviconDownloaderCallback callback);
  virtual ~FaviconDownloader() {}

  void Start();

 private:
  int DownloadFavicon(const GURL &faviconUrl);
  // DownloadImage callback
  void DidDownloadFavicon(int id,
                          int httpStatusCode,
                          const GURL &imageUrl,
                          const std::vector<SkBitmap> &bitmaps,
                          const std::vector<gfx::Size> &originalBitmapSizes);

  GURL m_faviconUrl;
  FaviconDownloaderCallback m_callback;
  base::WeakPtrFactory<FaviconDownloader> m_weakPtrFactory;
  DISALLOW_COPY_AND_ASSIGN(FaviconDownloader);
};

#endif // FAVICON_DOWNLOADER_H
