// Copyright 2015 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OPENSSL_SSL_WRAPPER_H_
#define OPENSSL_SSL_WRAPPER_H_

#ifdef GCC_4_6_X
#undef final
#endif
#include <openssl/ssl.h>
#ifdef GCC_4_6_X
#define final
#endif

#endif
