/*
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

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
