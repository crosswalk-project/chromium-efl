// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/download_manager_delegate_efl.h"

#include "base/files/file_path.h"
#include "content/public/browser/download_danger_type.h"
#include "content/public/browser/download_item.h"


bool DownloadManagerDelegateEfl::DetermineDownloadTarget(content::DownloadItem* item,
                                                         const content::DownloadTargetCallback& callback) {
  callback.Run(item->GetForcedFilePath(),
               content::DownloadItem::TARGET_DISPOSITION_OVERWRITE,
               content::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS,
               item->GetForcedFilePath());
  return true;
}

bool DownloadManagerDelegateEfl::ShouldCompleteDownload(content::DownloadItem*,
                                                        const base::Closure&) {
  return true;
}

bool DownloadManagerDelegateEfl::ShouldOpenDownload(content::DownloadItem*,
                                                    const content::DownloadOpenDelayedCallback&) {
  return true;
}

void DownloadManagerDelegateEfl::GetNextId(const content::DownloadIdCallback& callback) {
  static uint32 next_id = content::DownloadItem::kInvalidId + 1;
  callback.Run(next_id++);
}
