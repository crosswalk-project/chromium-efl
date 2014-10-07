// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "MotionUI.h"
#include "wkext_motion.h"
#include "base/logging.h"

unsigned int MotionUI::s_tiltToZoomStartAngle = 3;

static void freeTitleViewHashEntry(void *data) {
   free(data);
}

MotionUI& MotionUI::motionUI() {
  static MotionUI& motionUI = *new MotionUI();
  return motionUI;
}

MotionUI::MotionUI()
  : m_sensorHandle(0)
  , m_sensorTimer(0)
  , m_ewkView(0)
  , m_isTiltToZoomStarted(false)
  , m_tiltAngle(0)
  , m_lastTiltScale(0)
  , m_lastPinchScale(0)
  , tiltBaseScaleFactor(0.01)
  , tiltScaleFactorUnit(0.0005) {
  m_tiltViewHash = eina_hash_pointer_new(freeTitleViewHashEntry);
}

MotionUI::~MotionUI() {
  eina_hash_free(m_tiltViewHash);
  stopSensor();
}

#if !defined(TIZEN_LEGACY_V_2_2_1)
static sensor_listener_h listener_;
#endif

Eina_Bool MotionUI::initializeSensor(void*) {
  MotionUI& motionUI(MotionUI::motionUI());

#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_get_default_sensor(SENSOR_GRAVITY, &motionUI.m_sensorHandle);

  if (sensor_create_listener(motionUI.m_sensorHandle, &listener_) == SENSOR_ERROR_NONE) {
    if (sensor_listener_set_event_cb(listener_, 100, tiltCallback, 0) == SENSOR_ERROR_NONE) {
      int retSensorStart = sensor_listener_start(listener_);
#else
  if (sensor_create(&motionUI.m_sensorHandle) == SENSOR_ERROR_NONE) {
    if (sensor_motion_tilt_set_cb(motionUI.m_sensorHandle, tiltCallback, 0) == SENSOR_ERROR_NONE) {
      int retSensorStart = sensor_start(motionUI.m_sensorHandle, SENSOR_MOTION_TILT);
#endif
      if (retSensorStart == SENSOR_ERROR_NONE || retSensorStart == SENSOR_ERROR_NOT_SUPPORTED) {
          motionUI.m_sensorTimer = 0;
          return ECORE_CALLBACK_CANCEL;
      }
    }
  } else {
#if !defined(TIZEN_LEGACY_V_2_2_1)
    motionUI.m_sensorHandle = 0;
#endif
    return ECORE_CALLBACK_RENEW;
  }

#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_listener_stop(listener_);
  sensor_destroy_listener(listener_);
#else
  sensor_destroy(motionUI.m_sensorHandle);
  motionUI.m_sensorHandle = 0;
#endif

  return ECORE_CALLBACK_RENEW;
}

void MotionUI::startSensor() {
  if (m_sensorTimer)
    return;

  if (initializeSensor(0) == ECORE_CALLBACK_RENEW)
    m_sensorTimer = ecore_timer_add(.1, initializeSensor, 0);
}

void MotionUI::stopSensor() {
  if (m_sensorTimer) {
    ecore_timer_del(m_sensorTimer);
    m_sensorTimer = 0;
  }

#if !defined(TIZEN_LEGACY_V_2_2_1)
  if (!listener_)
#else
  if (!m_sensorHandle)
#endif
    return;

#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_destroy_listener(listener_);
#else
  sensor_destroy(m_sensorHandle);

  m_sensorHandle = 0;
#endif
}

void MotionUI::setTiltToZoom(Evas_Object* view, bool enable, unsigned int sensitivity) {
  LOG(ERROR)<<"***************** MotionUI::setTiltToZoom " << enable << ",sen " << sensitivity;

  if (enable) {
    unsigned* data = new unsigned(sensitivity);
    eina_hash_add(m_tiltViewHash, &view, data);
  }
  else {
    eina_hash_del(m_tiltViewHash, &view, NULL);

    if (!eina_hash_population(m_tiltViewHash))
#if !defined(TIZEN_LEGACY_V_2_2_1)
      sensor_listener_unset_event_cb(listener_);
#else
      sensor_motion_tilt_unset_cb(m_sensorHandle);
#endif
  }
}

#if !defined(TIZEN_LEGACY_V_2_2_1)
void MotionUI::tiltCallback(sensor_h sensor, sensor_event_s *event, void*) {
#else
void MotionUI::tiltCallback(unsigned long long, int x, int y, void*) {
#endif
  MotionUI& motionUI(MotionUI::motionUI());

  if (!motionUI.m_ewkView)
    return;

#if !defined(TIZEN_LEGACY_V_2_2_1)
  int x = event->values[0];
  int y = event->values[1];
#endif

  Ecore_Evas* ee = ecore_evas_ecore_evas_get(evas_object_evas_get(motionUI.m_ewkView));
  int angle;
  switch (ecore_evas_rotation_get(ee)) {
  case 90:
    angle = -x;
    break;
  case 180:
    angle = -y;
    break;
  case 270:
    angle = x;
    break;
  default:
    angle = y;
    break;
  }
  if (motionUI.m_tiltAngle == 0) {
    motionUI.m_tiltAngle += angle;
    unsigned sensitivity = 0;
    void* data = eina_hash_find(motionUI.m_tiltViewHash, motionUI.m_ewkView);
    if (data)
      sensitivity = *static_cast<unsigned*>(data);

    motionUI.m_lastTiltScale = 1.0f + (motionUI.m_tiltAngle * (motionUI.tiltBaseScaleFactor + sensitivity * motionUI.tiltScaleFactorUnit));
  }

  motionUI.m_tiltAngle += angle;
  if (!motionUI.m_isTiltToZoomStarted && abs(motionUI.m_tiltAngle) < MotionUI::s_tiltToZoomStartAngle)
    return;

  motionUI.tiltToZoom();
}

void MotionUI::initializeTiltToZoom(Evas_Object* view, const Evas_Coord_Point* position) {
  if (!eina_hash_find(m_tiltViewHash, &view))
    return;

  m_ewkView = view;

  if (position)
    memcpy(&m_tiltPosition, position, sizeof(Evas_Coord_Point));

  m_isTiltToZoomStarted = false;
  m_tiltAngle = 0;

#if !defined(TIZEN_LEGACY_V_2_2_1)
  if (sensor_listener_set_event_cb(listener_, 100, tiltCallback, 0))
#else
  if (sensor_motion_tilt_set_cb(m_sensorHandle, tiltCallback, 0))
#endif
    startSensor();
}

void MotionUI::stopTiltToZoom() {
  if (!m_ewkView)
    return;

#if !defined(TIZEN_LEGACY_V_2_2_1)
  sensor_listener_unset_event_cb(listener_);
#else
  sensor_motion_tilt_unset_cb(m_sensorHandle);
#endif

  stopSensor();
  m_ewkView = 0;
}

bool MotionUI::isTiltToZoomStarted() {
  return m_isTiltToZoomStarted;
}

void MotionUI::tiltToZoom() {
  if (!m_isTiltToZoomStarted) {
    m_isTiltToZoomStarted = true;
    evas_object_smart_callback_call(m_ewkView, "motion,move" , 0);
  }

  Wkext_Motion_Event motionEvent;
  motionEvent.position.x = m_tiltPosition.x;
  motionEvent.position.y = m_tiltPosition.y;

  unsigned sensitivity = 0;
  void* data = eina_hash_find(m_tiltViewHash, m_ewkView);
  if (data)
    sensitivity = *static_cast<unsigned*>(data);

  double lastTiltScale = 1.0f + (m_tiltAngle * (tiltBaseScaleFactor + sensitivity * tiltScaleFactorUnit));

  if (m_lastTiltScale != -10.0) {
    motionEvent.scale =  1.0f + (lastTiltScale - m_lastTiltScale);

    LOG(ERROR) << "###m_tiltAngle :" << m_tiltAngle << " scale " << motionEvent.scale << ", m_lastTiltScale" << m_lastTiltScale;
    // motionEvent.scale = syncScale(lastTiltScale, true);
    //  motionEvent.scale = lastTiltScale;
    evas_object_smart_callback_call(m_ewkView, "motion,zoom", &motionEvent);
  }
  m_lastTiltScale = lastTiltScale;
}

void MotionUI::scaleInitialize(double scale) {
  m_syncScale = scale;
  if (m_syncScale < 0)
    m_syncScale = 0;
  m_lastTiltScale = scale;
  m_lastPinchScale = scale;
}

void MotionUI::updateTiltPosition(Evas_Coord_Point* position) {
  m_tiltPosition.x = position->x;
  m_tiltPosition.y = position->y;
}

double MotionUI::syncScale(double scale, bool isTiltMode) {
  if (isTiltMode)
    m_syncScale = 1.0 + (scale - m_lastTiltScale);
  else
    m_syncScale = 1.0 + (scale - m_lastPinchScale);

  if (m_syncScale < 0)
    m_syncScale = 0;
  else if (m_syncScale > 5)
    m_syncScale = 5;
  if (isTiltMode)
    m_lastTiltScale = scale;
  else
    m_lastPinchScale = scale;

  return m_syncScale;
}
