// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_SUSPENDABLE_OBJECT_H_
#define EWK_SUSPENDABLE_OBJECT_H_

#include <string>

#include "base/callback.h"

class Ewk_Suspendable_Object {
 public:
  Ewk_Suspendable_Object(const base::Callback<void(bool)>& result_callback)
      : isDecided_(false),
        isSuspended_(false),
        callback_(result_callback) { }
  virtual ~Ewk_Suspendable_Object() { }

  bool setDecision(bool allowed);
  void ignore();
  bool suspend();

  bool isDecided() const { return isDecided_; }
  bool isSuspended() const { return isSuspended_; }

 private:
  bool isDecided_;
  bool isSuspended_;
  base::Callback<void(bool)> callback_; //run when policy is set by app
};

#endif /* EWK_SUSPENDABLE_OBJECT_H_ */
