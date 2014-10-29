// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_TIZEN_WEBAUDIO_DECODER_BROWSER_GSTREAMER_H_
#define CONTENT_BROWSER_MEDIA_TIZEN_WEBAUDIO_DECODER_BROWSER_GSTREAMER_H_

#include "base/basictypes.h"
#include "base/memory/singleton.h"
#include "base/memory/shared_memory.h"
#include "base/threading/thread.h"
#include "content/common/content_export.h"
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

namespace content {

class GSTDecoder;

// GstAppData struct
typedef struct _GstAppData {
  GstElement *pipeline_;
  GstElement *app_src_, *app_sink_;
  GstElement *decoder_, *convert_, *resample_, *capsfilter_;
  GstCaps *caps_;
  GstElement *audioout_;
  GstElement *audio_;
  GstPad *audiopad_;

  gchar audioout_name_[16];

  bool isRunning_;

  guint sourceid_;        // To control the GSource
  guint8 *encodeddata_;
  gsize enc_length_;
  guint64 enc_offset_;

  int pcm_output_;
  bool isEndOfStream_;
  bool isNewRequest_;

  GSTDecoder *gst_decoder_;
} GstAppData;

// BrowserMessageFilterTizen class
class CONTENT_EXPORT BrowserMessageFilterTizen {
 public:
  static BrowserMessageFilterTizen* GetInstance();

  void EncodedDataReceived(base::SharedMemoryHandle foreign_memory_handle,
                           base::FileDescriptor pcm_output,
                           uint32_t data_size);

 private:
  friend struct DefaultSingletonTraits<BrowserMessageFilterTizen>;
  BrowserMessageFilterTizen();
  virtual ~BrowserMessageFilterTizen();
  void DecodeUsingGST(base::SharedMemoryHandle foreign_memory_handle,
                      base::FileDescriptor pcm_output,
                      uint32_t data_size);

  base::Thread gst_thread_;

  DISALLOW_COPY_AND_ASSIGN(BrowserMessageFilterTizen);
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_TIZEN_WEBAUDIO_DECODER_BROWSER_GSTREAMER_H_
