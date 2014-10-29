// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "EdgeEffect.h"

#include <assert.h>
#include <Edje.h>

#include "API/ewk_settings_private.h"

#ifdef TIZEN_EDGE_EFFECT

EdgeEffect::EdgeEffect(Evas_Object* view)
    : enabled(true)
    , m_view(view)
    , m_edgeObject(NULL)
    , ewk_view_settings_get_(NULL) {
  assert(m_view);

  evas_object_event_callback_add(m_view, EVAS_CALLBACK_RESIZE, viewResizeCallback, this);
  evas_object_event_callback_add(m_view, EVAS_CALLBACK_MOVE, viewMoveCallback, this);
  evas_object_event_callback_add(m_view, EVAS_CALLBACK_SHOW, viewShowCallback, this);
  evas_object_event_callback_add(m_view, EVAS_CALLBACK_HIDE, viewHideCallback, this);
}

EdgeEffect::~EdgeEffect() {
  if (m_edgeObject)
    evas_object_del(m_edgeObject);

  evas_object_event_callback_del(m_view, EVAS_CALLBACK_RESIZE, viewResizeCallback);
  evas_object_event_callback_del(m_view, EVAS_CALLBACK_MOVE, viewMoveCallback);
  evas_object_event_callback_del(m_view, EVAS_CALLBACK_SHOW, viewShowCallback);
  evas_object_event_callback_del(m_view, EVAS_CALLBACK_HIDE, viewHideCallback);
}

void EdgeEffect::viewResizeCallback(void* data, Evas*, Evas_Object* view, void*) {
  int width;
  int height;
  evas_object_geometry_get(view, 0, 0, &width, &height);
  static_cast<EdgeEffect*>(data)->resizeObject(width, height);
}

void EdgeEffect::viewMoveCallback(void* data, Evas*, Evas_Object* view, void*) {
  int x;
  int y;
  evas_object_geometry_get(view, &x, &y, 0, 0);
  static_cast<EdgeEffect*>(data)->moveObject(x, y);
}

void EdgeEffect::viewShowCallback(void* data, Evas*, Evas_Object* view, void*) {
  static_cast<EdgeEffect*>(data)->showObject();
}

void EdgeEffect::viewHideCallback(void* data, Evas*, Evas_Object* view, void*) {
  static_cast<EdgeEffect*>(data)->hideObject();
}

Evas_Object* EdgeEffect::edgeObject() {
  if (!m_edgeObject) {
    m_edgeObject = edje_object_add(evas_object_evas_get(m_view));
    if (!m_edgeObject)
      return 0;

    if (!edje_object_file_set(m_edgeObject, EDGE_EDJE_FILE, EDJE_EFFECT_GROUP))
      return 0;

    evas_object_smart_member_add(m_edgeObject, m_view);

    int x;
    int y;
    int width;
    int height;
    evas_object_geometry_get(m_view, &x, &y, &width, &height);
    evas_object_move(m_edgeObject, x, y);
    evas_object_resize(m_edgeObject, width, height);
    evas_object_show(m_edgeObject);
    evas_object_pass_events_set(m_edgeObject, EINA_TRUE);
  }

  return m_edgeObject;
}

void EdgeEffect::show(const std::string& source) {
  if (!enabled)
    return;

  if (!ewk_view_settings_get_)
    return;

  Ewk_Settings* settings = ewk_view_settings_get_(m_view);
  if (settings && !settings->edgeEffectEnabled())
    return;

  Evas_Object* object = edgeObject();
  if (!object)
    return;

  const char* state = 0;
  double ret = 0;
  if (source.find("top")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_top", &ret);
  else if (source.find("bottom")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_bottom", &ret);
  else if (source.find("left")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_left", &ret);
  else if (source.find("right")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_right", &ret);

  if (!strncmp(state, "visible", 7))
    return;

  edje_object_signal_emit(object, "edge,show", source.c_str());
}

void EdgeEffect::hide(const std::string& source) {
  if (!ewk_view_settings_get_)
    return;

  Ewk_Settings* settings = ewk_view_settings_get_(m_view);
  if (settings && !settings->edgeEffectEnabled())
    return;

  Evas_Object* object = edgeObject();
  if (!object)
    return;

  const char* state = 0;
  double ret = 0;
  if (source.find("top")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_top", &ret);
  else if (source.find("bottom")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_bottom", &ret);
  else if (source.find("left")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_left", &ret);
  else if (source.find("right")!=std::string::npos)
    state = edje_object_part_state_get(object, "edge_effect_right", &ret);

  if (strncmp(state, "visible", 7))
    return;

  edje_object_signal_emit(object, "edge,hide", source.c_str());
}

void EdgeEffect::hide() {
  if (!ewk_view_settings_get_)
    return;

  Ewk_Settings* settings = ewk_view_settings_get_(m_view);
  if (settings && !settings->edgeEffectEnabled())
    return;

  Evas_Object* object = edgeObject();
  if (!object)
    return;

  edje_object_signal_emit(object, "edge,hide", "edge,top");
  edje_object_signal_emit(object, "edge,hide", "edge,bottom");
  edje_object_signal_emit(object, "edge,hide", "edge,left");
  edje_object_signal_emit(object, "edge,hide", "edge,right");
}

void EdgeEffect::setEwkSettingsGetCallback(Ewk_View_Settings_Get callback, void* user_date) {
  ewk_view_settings_get_ = callback;
}

void EdgeEffect::resizeObject(int width, int height) {
  if (!m_edgeObject)
    return;

  evas_object_resize(m_edgeObject, width, height);
}

void EdgeEffect::moveObject(int x, int y) {
  if (!m_edgeObject)
    return;

  evas_object_move(m_edgeObject, x, y);
}

void EdgeEffect::showObject() {
  if (!m_edgeObject)
    return;

  evas_object_show(m_edgeObject);
}

void EdgeEffect::hideObject() {
  if (!m_edgeObject)
    return;

  evas_object_hide(m_edgeObject);
}

void EdgeEffect::disable() {
  hide();
  enabled = false;
}

#endif // TIZEN_EDGE_EFFECT
