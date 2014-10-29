// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "ewk_window_features.h"
#include <../impl/API/ewk_window_features_private.h>
#include "eweb_view.h"

void _Ewk_Window_Features::setToolbarVisible(bool toolbar_visible)
{
  toolbar_visible_ = toolbar_visible;
  view_->SmartCallback<EWebViewCallbacks::ToolbarVisible>().call(&toolbar_visible);
}

void _Ewk_Window_Features::setStatusBarVisible(bool statusbar_visible)
{
  statusbar_visible_ = statusbar_visible;
  view_->SmartCallback<EWebViewCallbacks::StatusBarVisible>().call(&statusbar_visible);
}

void _Ewk_Window_Features::setMenuBarVisible(bool menubar_visible)
{
  menubar_visible_ = menubar_visible;
  view_->SmartCallback<EWebViewCallbacks::MenuBarVisible>().call(&menubar_visible);
}

void _Ewk_Window_Features::setResizable(bool resizable)
{
  resizable_ = resizable;
  view_->SmartCallback<EWebViewCallbacks::WindowResizable>().call(&resizable);
}

Eina_Bool ewk_window_features_toolbar_visible_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->toolbar_visible_;
}

Eina_Bool ewk_window_features_statusbar_visible_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->statusbar_visible_;
}

Eina_Bool ewk_window_features_scrollbars_visible_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->scrollbar_visible_;
}

Eina_Bool ewk_window_features_menubar_visible_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->menubar_visible_;
}

Eina_Bool ewk_window_features_locationbar_visible_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->locationbar_visible_;
}

Eina_Bool ewk_window_features_resizable_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->resizable_;
}

Eina_Bool ewk_window_features_fullscreen_get(const Ewk_Window_Features* window_features)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(window_features, false);
  return window_features->fullscreen_;
}

void ewk_window_features_geometry_get(const Ewk_Window_Features* window_features, float* x, float* y, float* width, float* height)
{
  EINA_SAFETY_ON_NULL_RETURN(window_features);

  if (x)
     *x = window_features->x_;
  if (y)
    *y = window_features->y_;
  if (width)
    *width = window_features->width_;
  if (height)
    *height = window_features->height_;
}

