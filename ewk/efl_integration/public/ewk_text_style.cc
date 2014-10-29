// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_text_style.h"

#include <Eina.h>

// TODO: remove dependency
#include <../impl/API/ewk_text_style_private.h>

Ewk_Text_Style_State ewk_text_style_underline_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.underline_state);
}

Ewk_Text_Style_State ewk_text_style_italic_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.italic_state);
}

Ewk_Text_Style_State ewk_text_style_bold_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.bold_state);
}

Ewk_Text_Style_State ewk_text_style_ordered_list_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.order_list_state);
}

Ewk_Text_Style_State ewk_text_style_unordered_list_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.un_order_list_state);
}

Eina_Bool ewk_text_style_position_get(Ewk_Text_Style* textStyle, Evas_Point* startPoint, Evas_Point* endPoint)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(startPoint, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(endPoint, false);
  startPoint->x = textStyle->position.startPoint.x;
  startPoint->y = textStyle->position.startPoint.y;

  endPoint->x = textStyle->position.endPoint.x;
  endPoint->y = textStyle->position.endPoint.y;

  return true;
}

Eina_Bool ewk_text_style_bg_color_get(Ewk_Text_Style* textStyle, int* r, int* g, int* b, int* a)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, false);
  if (r)
    *r = textStyle->bgColor.r;
  if (g)
    *g = textStyle->bgColor.g;
  if (b)
    *b = textStyle->bgColor.b;
  if (a)
    *a = textStyle->bgColor.a;

   return true;
}

Eina_Bool ewk_text_style_color_get(Ewk_Text_Style* textStyle, int* r, int* g, int* b, int* a)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, false);
  if (r)
    *r = textStyle->color.r;
  if (g)
    *g = textStyle->color.g;
  if (b)
    *b = textStyle->color.b;
  if (a)
    *a = textStyle->color.a;

  return true;
}

Eina_Stringshare* ewk_text_style_font_size_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, 0);
  return textStyle->fontSize;
}

Eina_Bool ewk_text_style_has_composition_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, false);
  return textStyle->hasComposition;
}

Ewk_Text_Style_State ewk_text_style_align_center_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.text_align_center_state);
}

Ewk_Text_Style_State ewk_text_style_align_left_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.text_align_left_state);
}

Ewk_Text_Style_State ewk_text_style_align_right_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.text_align_right_state);
}

Ewk_Text_Style_State ewk_text_style_align_full_get(Ewk_Text_Style* textStyle)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(textStyle, EWK_TEXT_STYLE_STATE_FALSE);
  return static_cast<Ewk_Text_Style_State>(textStyle->selectionParams.text_align_full_state);
}
