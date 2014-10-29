// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_log.h
 * @brief   flags related to logging functions.
 */

#ifndef ewk_log_h
#define ewk_log_h

#include <eina_log.h>

extern int _ewk_log_dom;

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_ewk_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_ewk_log_dom, __VA_ARGS__)
#define WARN(...) EINA_LOG_DOM_WARN(_ewk_log_dom, __VA_ARGS__)
#define INFO(...) EINA_LOG_DOM_INFO(_ewk_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_ewk_log_dom, __VA_ARGS__)

#endif //ewk_log.h
