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

#define EDGE_EDJE_FILE EDJE_DIR"/Edge.edj"
#define EDJE_EFFECT_GROUP "edge_effect"

// [sns.park] The below #ifdef block is temporarily copied from ewk_view.h
// and ewk_settings.h
// Ewk_View_Settings_Get, ewk_view_settings_get_callback_set(),
// EWebView::SetSettingsGetCallback() were introduced by commit 9f717f8 (chromium-ewk)
// and 30c328a4 (chromium-efl) for implementing Edge Effect WCS feature.
// The only purpose of the functions is to allow access to Ewk_Settings inside this class.
// But it is questionable whether adding those api is really needed.
// I believe EdgeEffect can be implemented to directly query settings without them.
// TODO: remove below #ifdef block by refactoring EdgeEffect.
#ifdef __cplusplus 
extern "C" {
#endif
typedef struct Ewk_Settings Ewk_Settings;
typedef Ewk_Settings* (*Ewk_View_Settings_Get)(const Evas_Object* o);
#ifdef __cplusplus
}
#endif
// End of TODO

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
