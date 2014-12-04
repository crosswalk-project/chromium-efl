// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_frame_h
#define ewk_frame_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Frame _Ewk_Frame;
typedef _Ewk_Frame* Ewk_Frame_Ref;

/**
 * Callback for ewk_frame_source_get
 *
 * @param frame frame object to get the frame source
 * @param source frame source on success, or NULL on failure
 * @param user_data user data
 */
typedef void (*Ewk_Frame_Source_Get_Callback)(Ewk_Frame_Ref frame, const char* source, void* user_data);

/**
 * Gets whether a MIME type can be displayed in the frame.
 *
 * @param frame frame object
 * @param mime_type a mime type
 *
 * @return @c EINA_TRUE if the MIME type can be displayed or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_frame_can_show_mime_type(Ewk_Frame_Ref frame, char* mime_type);

/**
 * Gets whether the frame is main frame.
 *
 * @param frame frame object
 *
 * @return @c EINA_TRUE if the frame is main frame or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_frame_is_main_frame(Ewk_Frame_Ref frame);

#ifdef __cplusplus
}
#endif
#endif // ewk_frame_h
