/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef ewk_hit_test_h
#define ewk_hit_test_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Ewk_Hit_Test_Mode:
 * @EWK_HIT_TEST_MODE_DEFAULT: link data.
 * @EWK_HIT_TEST_MODE_NODE_DATA: extra node data(tag name, node value, attribute infomation, etc).
 * @EWK_HIT_TEST_MODE_IMAGE_DATA: extra image data(image data, image data length, image file name exteionsion, etc).
 * @EWK_HIT_TEST_MODE_ALL: all data.
 *
 * Enum values with flags representing the hit test mode.
 */

typedef enum {
    EWK_HIT_TEST_MODE_DEFAULT = 1 << 1,
    EWK_HIT_TEST_MODE_NODE_DATA = 1 << 2,
    EWK_HIT_TEST_MODE_IMAGE_DATA = 1 << 3,
    EWK_HIT_TEST_MODE_ALL = EWK_HIT_TEST_MODE_DEFAULT | EWK_HIT_TEST_MODE_NODE_DATA | EWK_HIT_TEST_MODE_IMAGE_DATA
} Ewk_Hit_Test_Mode;

/**
 * Ewk_Hit_Test_Result_Context:
 * @EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT: anywhere in the document.
 * @EWK_HIT_TEST_RESULT_CONTEXT_LINK: a hyperlink element.
 * @EWK_HIT_TEST_RESULT_CONTEXT_IMAGE: an image element.
 * @EWK_HIT_TEST_RESULT_CONTEXT_MEDIA: a video or audio element.
 * @EWK_HIT_TEST_RESULT_CONTEXT_SELECTION: the area is selected by the user.
 * @EWK_HIT_TEST_RESULT_CONTEXT_EDITABLE: the area is editable by the user.
 * @EWK_HIT_TEST_RESULT_CONTEXT_TEXT: the area is text.
 *
 * Enum values with flags representing the context of a #Ewk_Hit_Test.
 */
typedef enum {
    EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT = 1 << 1,
    EWK_HIT_TEST_RESULT_CONTEXT_LINK = 1 << 2,
    EWK_HIT_TEST_RESULT_CONTEXT_IMAGE = 1 << 3,
    EWK_HIT_TEST_RESULT_CONTEXT_MEDIA = 1 << 4,
    EWK_HIT_TEST_RESULT_CONTEXT_SELECTION = 1 << 5,
    EWK_HIT_TEST_RESULT_CONTEXT_EDITABLE = 1 << 6,
    EWK_HIT_TEST_RESULT_CONTEXT_TEXT = 1 << 7
} Ewk_Hit_Test_Result_Context;

/** Creates a type name for _Ewk_Hit_Test */
typedef struct _Ewk_Hit_Test Ewk_Hit_Test;


#ifdef __cplusplus
}
#endif

#endif // ewk_hit_test_h
