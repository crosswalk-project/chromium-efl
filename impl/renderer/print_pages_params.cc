// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/print_pages_params.h"

PrintParams::PrintParams()
  : page_size(),
    content_size(),
    printable_area(),
    dpi(0),
    desired_dpi(0),
    is_first_request(true),
    print_to_pdf(true),
    print_scaling_option(blink::WebPrintScalingOptionSourceSize),
    document_cookie(0) {
}

PrintPagesParams::PrintPagesParams()
  : pages() {
}
