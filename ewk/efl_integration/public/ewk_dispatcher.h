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

#ifndef ewk_chromium_h
#define ewk_chromium_h

#include "ewk_export.h"

typedef void (*ewk_dispatch_callback)(void *);

/**
 * Call once from the thread you want the dispatcher work on.
 * It should be main thread usually.
 *
 */
EAPI void ewk_dispatcher_touch();

EAPI void ewk_dispatcher_dispatch(ewk_dispatch_callback cb, void *user_data, unsigned delay);

#endif
