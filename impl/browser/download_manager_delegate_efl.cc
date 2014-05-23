#include "browser/download_manager_delegate_efl.h"

#include "base/files/file_path.h"
#include "content/public/browser/download_danger_type.h"
#include "content/public/browser/download_item.h"


bool DownloadManagerDelegateEfl::DetermineDownloadTarget(content::DownloadItem*,
                                                         const content::DownloadTargetCallback& callback) {
  callback.Run(base::FilePath() /* Empty file path for cancel */,
               content::DownloadItem::TARGET_DISPOSITION_OVERWRITE,
               content::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
               base::FilePath());
  return true;
}

bool DownloadManagerDelegateEfl::ShouldCompleteDownload(content::DownloadItem*,
                                                        const base::Closure&) {
  NOTREACHED();
  return true;
}

bool DownloadManagerDelegateEfl::ShouldOpenDownload(content::DownloadItem*,
                                                    const content::DownloadOpenDelayedCallback&) {
  NOTREACHED();
  return true;
}

void DownloadManagerDelegateEfl::GetNextId(const content::DownloadIdCallback& callback) {
  static uint32 next_id = content::DownloadItem::kInvalidId + 1;
  callback.Run(next_id++);
}
