/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

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
