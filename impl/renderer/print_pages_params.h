// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PRINT_PAGES_PARAMS_H_
#define PRINT_PAGES_PARAMS_H_

#include <vector>

#include "base/basictypes.h"
#include "base/files/file_path.h"
#include "base/memory/shared_memory.h"
#include "printing/printing_export.h"
#include "third_party/WebKit/public/web/WebPrintScalingOption.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/size.h"

struct PrintParams {
  PrintParams();

  gfx::Size page_size;
  gfx::Size content_size;
  gfx::Rect printable_area;
  double dpi;
  int desired_dpi;
  bool is_first_request;
  bool print_to_pdf;
  blink::WebPrintScalingOption print_scaling_option;
  int document_cookie;
};

// collection of printable pages
struct PrintPagesParams {
  PrintPagesParams();

  PrintParams params;
  std::vector<int> pages;
};

// single page
struct PrintPageParams {
  PrintParams params;
  int page_number;
};

struct DidPrintPagesParams {
  base::SharedMemoryHandle metafile_data_handle;
  uint32 data_size;
  int document_cookie;
  base::FilePath filename;
};

#endif // PRINT_PAGES_PARAMS_H_
