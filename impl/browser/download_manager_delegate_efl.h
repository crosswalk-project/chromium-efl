// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOWNLOAD_MANAGER_DELEGATE_EFL_H
#define DOWNLOAD_MANAGER_DELEGATE_EFL_H

#include "content/public/browser/download_manager_delegate.h"

// EFL WebView does not use Chromium downloads, so implement methods here to
// unconditionally cancel the download.
class DownloadManagerDelegateEfl : public content::DownloadManagerDelegate {
public:
    virtual ~DownloadManagerDelegateEfl() { }

    // content::DownloadManagerDelegate implementation.
    virtual bool DetermineDownloadTarget(
        content::DownloadItem*,
        const content::DownloadTargetCallback&) OVERRIDE;
    virtual bool ShouldCompleteDownload(
        content::DownloadItem*,
        const base::Closure&) OVERRIDE;
    virtual bool ShouldOpenDownload(
        content::DownloadItem*,
        const content::DownloadOpenDelayedCallback&) OVERRIDE;
    virtual void GetNextId(const content::DownloadIdCallback&) OVERRIDE;
};

#endif // DOWNLOAD_MANAGER_DELEGATE_EFL_H
