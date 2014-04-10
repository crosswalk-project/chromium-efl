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

#ifndef MotionUI_h
#define MotionUI_h

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <sensors.h>

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
  static void tiltCallback(unsigned long long, int, int, void*);
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
