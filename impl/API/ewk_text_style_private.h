/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef ewk_text_style_private_h
#define ewk_text_style_private_h

#include <string>
#include <vector>

#include <Eina.h>
#include <Evas.h>

#include <ui/gfx/point.h>

struct SelectionColor {
  int r;
  int g;
  int b;
  int a;
};

struct SelectionStylePrams {
  int underline_state;
  int italic_state;
  int bold_state;
  SelectionColor bg_color;
  SelectionColor color;
  std::string font_size;
  int order_list_state;
  int un_order_list_state;
  int text_align_center_state;
  int text_align_left_state;
  int text_align_right_state;
  int text_align_full_state;
  int has_composition;
};

struct _Ewk_Text_Style {
  _Ewk_Text_Style(const SelectionStylePrams& params, const gfx::Point& start_point, const gfx::Point& end_point) {
    selectionParams = params;

    position.startPoint.x = start_point.x();
    position.startPoint.y = start_point.y();
    position.endPoint.x = end_point.x();
    position.endPoint.y = end_point.y();

    bgColor = params.bg_color;
    color = params.color;
    fontSize = eina_stringshare_add(params.font_size.c_str());
    hasComposition = params.has_composition;
  }

  ~_Ewk_Text_Style() {
    eina_stringshare_del(fontSize);
  }

  SelectionStylePrams selectionParams;
  Eina_Stringshare* fontSize;
  Eina_Bool hasComposition;

  struct {
    Evas_Point startPoint;
    Evas_Point endPoint;
  } position;

  SelectionColor bgColor;
  SelectionColor color;
};

#endif
