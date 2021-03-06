/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2016 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_ANDROID_NUNCHUCK_DEVICE_HPP
#define XCSOAR_ANDROID_NUNCHUCK_DEVICE_HPP

#include "NunchuckListener.hpp"
#include "Java/Object.hxx"
#include "Math/SelfTimingKalmanFilter1d.hpp"
#include "Util/Compiler.h"

#include <jni.h>

class NunchuckDevice final : private NunchuckListener {
  unsigned index;
  Java::GlobalObject obj;

  /**
   * Kept as sample, not used.
   */
  SelfTimingKalmanFilter1d kalman_filter;

public:
  static void Initialise(JNIEnv *env);
  static void Deinitialise(JNIEnv *env);

  NunchuckDevice(unsigned index,
               JNIEnv *env, jobject holder,
               unsigned twi_num, unsigned sample_rate);

  ~NunchuckDevice();

private:
  /* virtual methods from class NunchuckListener */
  virtual void onNunchuckValues(int joy_x, int joy_y,
                                int acc_x, int acc_y, int acc_z, int switches) override;
  virtual void onNunchuckError() override;
};

#endif
