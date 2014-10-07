// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MotionUI_h
#define MotionUI_h

#include <Ecore.h>
#include <Ecore_Evas.h>
#if !defined(TIZEN_LEGACY_V_2_2_1)
#include <sensor/sensor.h>
#else
#include <sensors.h>
#endif


class MotionUI {
 public:
  static MotionUI& motionUI();
  ~MotionUI();

  void setTiltToZoom(Evas_Object*, bool, unsigned int);

  void initializeTiltToZoom(Evas_Object*, const Evas_Coord_Point*);
  void stopTiltToZoom();
  bool isTiltToZoomStarted();

  void updateTiltPosition(Evas_Coord_Point* position);
  void scaleInitialize(double scale);
  double syncScale(double scale, bool isTiltMode);

 private:
  static Eina_Bool initializeSensor(void*);
#if !defined(TIZEN_LEGACY_V_2_2_1)
  static void tiltCallback(sensor_h sensor, sensor_event_s *event, void*);
#else
  static void tiltCallback(unsigned long long, int, int, void*);
#endif
  MotionUI();

  void startSensor();
  void stopSensor();

  void tiltToZoom();

  static unsigned int s_tiltToZoomStartAngle;

  Eina_Hash* m_tiltViewHash;

  sensor_h m_sensorHandle;
  Ecore_Timer* m_sensorTimer;

  Evas_Object* m_ewkView;
  Evas_Coord_Point m_tiltPosition;
  bool m_isTiltToZoomStarted;
  int m_tiltAngle;

  //Sync for pinch zoom
  double m_syncScale;
  double m_lastTiltScale;
  double m_lastPinchScale;
  double tiltBaseScaleFactor;
  double tiltScaleFactorUnit;
};

#endif // MotionUI_h
