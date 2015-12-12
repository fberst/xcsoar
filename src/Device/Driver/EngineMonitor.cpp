/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2015 The XCSoar Project
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

#include "Device/Driver/EngineMonitor.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "NMEA/Checksum.hpp"

#include <stdint.h>

static bool
ParseRBEV1(NMEAInputLine &line, NMEAInfo &info)
{
  fixed value;

  // cylinderhead1 temperature [K]
  if (line.ReadChecked(value)) {
    info.ProvideCHT1(value);
  }

  // cylinderhead2 temperature [K]
  if (line.ReadChecked(value)) {
     info.ProvideCHT2(value);
  }

  // cylinderhead3 temperature [K]
  if (line.ReadChecked(value)) {
     info.ProvideCHT3(value);
  }

  // cylinderhead4 temperature [K]
  if (line.ReadChecked(value)) {
    info.ProvideCHT4(value);
  }

  // exhaust temperature [K]
  if (line.ReadChecked(value)) {
    info.ProvideExhaustTemperature(value);
  }

  return true;
}


static bool
ParseRBEV2(NMEAInputLine &line, NMEAInfo &info)
{
  fixed value;

  // oil temperature [K]
  if (line.ReadChecked(value)) {
    info.ProvideOilTemperature(value);
  }

  // cooling water temperature [K]
  if (line.ReadChecked(value)) {
    info.ProvideWaterTemperature(value);
  }

  // oil pressure [Pa]
  if (line.ReadChecked(value)) {
    info.ProvideOilPressure(value);
   
  }

  // engine speed in rpm
  if (line.ReadChecked(value)) {
     info.ProvideEngineSpeed(value);
  }

  // full gauge [%]
  if (line.ReadChecked(value)) {
     info.ProvideFullGauge(value);
  }

  return true;
}

static bool
ParseRBEV3(NMEAInputLine &line, NMEAInfo &info)
{
  fixed value;

  // full flow [l/h]
  if (line.ReadChecked(value)) {
    info.ProvideFullFlow(value);
  }

  // voltage1 [V]
  if (line.ReadChecked(value)) {
    info.ProvideVoltage1(value);
  }

  // voltage2 [V]
  if (line.ReadChecked(value)) {
    info.ProvideVoltage2(value);
  }

  // State of switch1 [0,1]
  if (line.ReadChecked(value)) {
    info.ProvideSwitch1(value);
  }

  // State of switch2 [0,1]
  if (line.ReadChecked(value)) {
    info.ProvideSwitch2(value);
  }

  return true;
}


static bool
ParseRBEV4(NMEAInputLine &line, NMEAInfo &info)
{
  fixed value;

  // State of switch3 [0,1]
  if (line.ReadChecked(value)) {
    info.ProvideSwitch3(value);
  }

  // State of switch4 [0,1]
  if (line.ReadChecked(value)) {
    info.ProvideSwitch4(value);
  }

  // Counter for operating hours
  if (line.ReadChecked(value)) {
    fixed hours=value;
    if (line.ReadChecked(value)) {
      info.ProvideOperatinghours(hours, value);
    }
  }

  return true;
}

static bool
ParseRBEV5(NMEAInputLine &line, NMEAInfo &info)
{
  fixed value;

  // bank angle [degrees, positive right]
  if (line.ReadChecked(value)) {
    info.attitude.bank_angle_available.Update(info.clock);
    info.attitude.bank_angle = Angle::Degrees(value);
  }

  // pitch angle [degrees, positive up]
  if (line.ReadChecked(value)) {
    info.attitude.pitch_angle_available.Update(info.clock);
    info.attitude.pitch_angle = Angle::Degrees(value);
  }

  // heading [degrees]
  if (line.ReadChecked(value)) {
    info.attitude.heading_available.Update(info.clock);
    info.attitude.heading = Angle::Degrees(value);
  }

  // Load factor [g]
  if (line.ReadChecked(value)) {
    info.acceleration.ProvideGLoad(value, true);
  }

  return true;
}

class EngineMonitorDevice : public AbstractDevice {
  Port &port;

public:
  EngineMonitorDevice(Port &_port):port(_port) {}

  /* virtual methods from class Device */
  bool ParseNMEA(const char *line, struct NMEAInfo &info) override;
};

bool
EngineMonitorDevice::ParseNMEA(const char *_line, NMEAInfo &info)
{
  if (!VerifyNMEAChecksum(_line))
    return false;

  NMEAInputLine line(_line);
  char type[16];
  line.Read(type, 16);

  if (StringIsEqual(type, "$RBEV1"))
    return ParseRBEV1(line, info);
  else if (StringIsEqual(type, "$RBEV2"))
    return ParseRBEV2(line, info);
  else if (StringIsEqual(type, "$RBEV3"))
    return ParseRBEV3(line, info);
  else if (StringIsEqual(type, "$RBEV4"))
    return ParseRBEV4(line, info);
  else if (StringIsEqual(type, "$RBEV5"))
    return ParseRBEV5(line, info);
  else
    return false;
}


static Device *
EngineCreateOnPort(const DeviceConfig &config, Port &com_port)
{
  return new EngineMonitorDevice(com_port);
}

const struct DeviceRegister engine_monitor_driver = {
  _T("Engine Monitor"),
  _T("RBE Engine Monitor"),
  0,
  EngineCreateOnPort,
};
