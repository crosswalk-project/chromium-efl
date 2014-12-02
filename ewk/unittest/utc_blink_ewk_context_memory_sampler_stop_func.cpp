// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_memory_sampler_stop: public utc_blink_ewk_base
{
};

/**
 * @brief Checking whether memory sampler is stopped.
 */
TEST_F(utc_blink_ewk_context_memory_sampler_stop, POS_TEST)
{
    ewk_context_memory_sampler_start(ewk_view_context_get(GetEwkWebView()), 1);
    ewk_context_memory_sampler_stop(ewk_view_context_get(GetEwkWebView()));

    utc_pass();
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_memory_sampler_stop, NEG_TEST)
{
    ewk_context_memory_sampler_stop(NULL);

    utc_pass();
}
