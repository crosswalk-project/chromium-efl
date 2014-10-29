// Copyright (C) 2012 Intel Corporation. All rights reserved.
// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_private_h
#define ewk_private_h

#include "public/ewk_export.h"

#define COMPILE_ASSERT_MATCHING_ENUM(ewkName, webcoreName) \
        COMPILE_ASSERT(int(ewkName) == int(webcoreName), mismatchingEnums)

// Temporarily added in order to track not-yet-implemented ewk api calls. 
#include <stdio.h>
#define LOG_EWK_API_MOCKUP(msg, ...) printf("[EWK_API_MOCKUP] %s:%d %s ", __FILE__, __LINE__, __PRETTY_FUNCTION__); printf(msg "\n", ##__VA_ARGS__)
#endif
