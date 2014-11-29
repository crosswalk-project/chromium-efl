/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
