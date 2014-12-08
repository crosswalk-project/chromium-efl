// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EdgeEffect_h
#define EdgeEffect_h

#ifdef TIZEN_EDGE_EFFECT
#include <string>
#include <Evas.h>
#include "base/memory/ref_counted.h"

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

  static void viewResizeCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void viewMoveCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void viewShowCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void viewHideCallback(void* data, Evas*, Evas_Object*, void* eventInfo);
  static void setSettingsEnabled(bool enable) { m_settingsEnable = enable; };

 private:
  EdgeEffect(Evas_Object* view);

  Evas_Object* ensureEdgeObject();
  void resizeObject(int width, int height);
  void moveObject(int x, int y);
  void showObject();
  void hideObject();
  bool enabled;

  Evas_Object* m_view;
  Evas_Object* m_edgeObject;
  static bool m_settingsEnable;
};

#endif // TIZEN_EDGE_EFFECT

#endif // EdgeEffect_h
