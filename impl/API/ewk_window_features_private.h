// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_window_features_private_h
#define ewk_window_features_private_h

class EWebView;

struct _Ewk_Window_Features{
  _Ewk_Window_Features(EWebView* ewk_view)
    : view_(ewk_view)
      , x_(0), y_(0), width_(0), height_(0), menubar_visible_(true)
      , statusbar_visible_(true), toolbar_visible_(true), locationbar_visible_(true)
      , scrollbar_visible_(true), resizable_(true), fullscreen_(false)
  {
  }

  ~_Ewk_Window_Features()
  {
  }

  void setToolbarVisible(bool toolbar_visible);
  void setStatusBarVisible(bool statusbar_visible);
  void setMenuBarVisible(bool menubar_visible);
  void setResizable(bool resizable);

  EWebView* view_;

  float x_;
  float y_;
  float width_;
  float height_;

  bool menubar_visible_;
  bool statusbar_visible_;
  bool toolbar_visible_;
  bool locationbar_visible_;
  bool scrollbar_visible_;
  bool resizable_;
  bool fullscreen_;

};

#endif //ewk_window_features_private_h
