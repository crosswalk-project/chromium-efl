/*
 * Copyright (C) 2012 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// FIXME: need to adapt this code to sensor API changes in Tizen 2.3.
#if !defined(EWK_BRINGUP)

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

Eina_Bool MotionUI::initializeSensor(void*) {
  MotionUI& motionUI(MotionUI::motionUI());

  if (sensor_create(&motionUI.m_sensorHandle) == SENSOR_ERROR_NONE) {
    if (sensor_motion_tilt_set_cb(motionUI.m_sensorHandle, tiltCallback, 0) == SENSOR_ERROR_NONE) {
      int retSensorStart = sensor_start(motionUI.m_sensorHandle, SENSOR_MOTION_TILT);
      if (retSensorStart == SENSOR_ERROR_NONE || retSensorStart == SENSOR_ERROR_NOT_SUPPORTED) {
          motionUI.m_sensorTimer = 0;
          return ECORE_CALLBACK_CANCEL;
      }
    }
  } else {
    motionUI.m_sensorHandle = 0;
    return ECORE_CALLBACK_RENEW;
  }

  sensor_destroy(motionUI.m_sensorHandle);
  motionUI.m_sensorHandle = 0;

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

  if (!m_sensorHandle)
    return;

  sensor_destroy(m_sensorHandle);

  m_sensorHandle = 0;
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
      sensor_motion_tilt_unset_cb(m_sensorHandle);
  }
}

void MotionUI::tiltCallback(unsigned long long, int x, int y, void*) {
  MotionUI& motionUI(MotionUI::motionUI());

  if (!motionUI.m_ewkView)
    return;

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

  if (sensor_motion_tilt_set_cb(m_sensorHandle, tiltCallback, 0))
    startSensor();
}

void MotionUI::stopTiltToZoom() {
  if (!m_ewkView)
    return;

  sensor_motion_tilt_unset_cb(m_sensorHandle);

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

#endif // EWK_BRINGUP
