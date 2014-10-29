// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SCOPED_WAIT_FOR_EWK_H_
#define SCOPED_WAIT_FOR_EWK_H_

#include "base/threading/thread_restrictions.h"
// For Ewk API that require synchronous execution
// Rather than allowing all classes to use ScopedWait, we can use this class.
// Similar to Android Legacy WebView API
class ScopedAllowWaitForEwkApi {
 private:
  base::ThreadRestrictions::ScopedAllowWait wait;
};

#endif // SCOPED_WAIT_FOR_EWK_H_
