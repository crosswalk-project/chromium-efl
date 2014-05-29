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

#ifndef PRINT_WEB_VIEW_HELPER_H_
#define PRINT_WEB_VIEW_HELPER_H_

#include "base/files/file_path.h"
#include "base/memory/scoped_ptr.h"
#include "renderer/print_pages_params.h"
#include "third_party/WebKit/public/platform/WebCanvas.h"

namespace content {
class RenderView;
}

namespace blink {
class WebFrame;
}

namespace printing {
class Metafile;
}

struct PrintPagesParams;

class PrintWebViewHelperEfl {
 public:
  PrintWebViewHelperEfl(content::RenderView* view, const base::FilePath& filename);
  virtual ~PrintWebViewHelperEfl();
  void PrintToPdf(int width, int height);
  void InitPrintSettings(int width, int height, bool fit_to_paper_size);

 private:
  bool PrintPagesToPdf(blink::WebFrame* frame, int page_count,
      const gfx::Size& canvas_size);
  bool PrintPageInternal(const PrintPageParams& params,
      const gfx::Size& canvas_size, blink::WebFrame* frame, printing::Metafile* metafile);
  bool RenderPagesForPrint(blink::WebFrame* frame);

  scoped_ptr<PrintPagesParams> print_pages_params_;
  content::RenderView* view_;
  base::FilePath filename_;
};

#endif // PRINT_WEB_VIEW_HELPER_H_
