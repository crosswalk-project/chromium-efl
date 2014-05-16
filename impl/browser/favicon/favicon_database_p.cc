#include "favicon_database_p.h"
#include "favicon_commands.h"
#include <iostream>

const char *FaviconDatabasePrivate::pageUrlToFaviconUrlTable = "url_to_favicon_url";
const char *FaviconDatabasePrivate::faviconUrlToBitmapTable = "favicon_url_to_bitmap";
const char *FaviconDatabasePrivate::pageUrlColumn = "page_url";
const char *FaviconDatabasePrivate::faviconUrlColumn = "favicon_url";
const char *FaviconDatabasePrivate::bitmapColumn = "bitmap";

GURL FaviconDatabasePrivate::faviconUrlForPageUrl(const GURL &pageUrl) const {
  std::map<GURL, GURL>::const_iterator it = pageToFaviconUrl.find(pageUrl);
  if (it != pageToFaviconUrl.end()) {
    return it->second;
  }
  return GURL();
}

SkBitmap FaviconDatabasePrivate::bitmapForFaviconUrl(const GURL &faviconUrl) const {
  std::map<GURL, SkBitmap>::const_iterator it = faviconUrlToBitmap.find(faviconUrl);
  if (it != faviconUrlToBitmap.end()) {

    return it->second;
  }
  return SkBitmap();
}

bool FaviconDatabasePrivate::existsForFaviconURL(const GURL &faviconUrl) const {
  std::map<GURL, SkBitmap>::const_iterator it = faviconUrlToBitmap.find(faviconUrl);
  return it != faviconUrlToBitmap.end();
}

scoped_refptr<base::SingleThreadTaskRunner> FaviconDatabasePrivate::taskRunner() const {
  return content::BrowserThread::GetMessageLoopProxyForThread(content::BrowserThread::DB);
}

void FaviconDatabasePrivate::performSync() {
  std::cerr << "[FaviconDatabasePrivate::performSync()]" << std::endl;
  base::AutoLock locker(mutex);
  timer.Stop();
  while (!commands.empty()) {
    Command *cmd = commands.front();
    if (!cmd->sqlExecute()) {
      std::cerr << "[FaviconDatabasePrivate::performSync] :: " << "Error while executing command:\n\t" << cmd->lastError() << std::endl;
      // abort or what?
    }
    commands.pop();
    delete cmd;
  }
}
