/*
 * Copyright (C) 2013 Samsung Electronics
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

#ifndef EdgeEffect_h
#define EdgeEffect_h

#ifdef TIZEN_EDGE_EFFECT
#include <string>
#include <Evas.h>
#include "base/memory/ref_counted.h"
#include "public/ewk_view.h"

#define EDGE_EDJE_FILE EDJE_DIR"/Edge.edj"
#define EDJE_EFFECT_GROUP "edge_effect"

class EdgeEffect : public base::RefCounted<EdgeEffect> {
 public:
  static scoped_refptr<EdgeEffect> create(Evas_Object* view)
  {
    return make_scoped_refptr(new EdgeEffect(view));
  }
  ~EdgeEffect();

  void show(const std::string& source);
  void hide(const std::string& source);
  void hide();
  void enable() { enabled = true; }
  void disable();

  void setEwkSettingsGetCallback(Ewk_View_Settings_Get, void*);

  static void viewResizeCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void viewMoveCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void viewShowCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void viewHideCallback(void* data, Evas*, Evas_Object*, void* eventInfo);

 private:
  EdgeEffect(Evas_Object* view);

  Evas_Object* edgeObject();
  void resizeObject(int width, int height);
  void moveObject(int x, int y);
  void showObject();
  void hideObject();
  bool enabled;

  Evas_Object* m_view;
  Evas_Object* m_edgeObject;

  Ewk_View_Settings_Get ewk_view_settings_get_;
};

#endif // TIZEN_EDGE_EFFECT

#endif // EdgeEffect_h
