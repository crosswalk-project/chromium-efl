// Copyright (C) 2012 Intel Corporation.
// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_error_private_h
#define ewk_error_private_h

#include <Eina.h>

struct _Ewk_Error {
  int error_code;
  Eina_Stringshare* url;
  Eina_Stringshare* description;
  Eina_Stringshare* domain;

  _Ewk_Error(int error_code_in, const char* url_in, const char* description_in, const char* domain_in = NULL)
      : error_code(error_code_in),
        url(eina_stringshare_add(url_in)),
        description(eina_stringshare_add(description_in)),
        domain(eina_stringshare_add(domain_in)) {
  }

  ~_Ewk_Error() {
    eina_stringshare_del(url);
    eina_stringshare_del(description);
    eina_stringshare_del(domain);
  }
};

#endif // ewk_error_private_h
