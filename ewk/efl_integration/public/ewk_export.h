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

#ifndef EWK_EXPORT_H_
#define EWK_EXPORT_H_

#ifdef EAPI
# undef EAPI
#endif

#if defined(WIN32)
#if defined(EFLWEBVIEW_IMPLEMENTATION)
#define EAPI __declspec(dllexport)
#else
#define EAPI __declspec(dllimport)
#endif  // defined(CONTENT_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(EFLWEBVIEW_IMPLEMENTATION)
#define EAPI __attribute__((visibility("default")))
#else
#define EAPI
#endif
#endif

#endif  // CONTENT_COMMON_CONTENT_EXPORT_H_
