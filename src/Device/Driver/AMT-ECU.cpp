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

#include "Device/Driver/AMT-ECU.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "NMEA/Checksum.hpp"

static bool
Parse0FF(NMEAInputLine &line, NMEAInfo &info)
{
  bool b[8];
  for(int i=0; i<8; i++) b[i]=false;

  int value;

  // data1
  if (line.ReadChecked(value)) {
    for(int i=0; i<8; i++) {
      if(value & (1<<i)) b[i]=true;
    }
  }

  if(value==0) return false;  // Error
  if(value==5) return false;  // ECU-setup
  if((b[0]==true) || (b[1]==true) || (b[2]==true)) return false;  // Alternate sentence

  // data2
  if (line.ReadChecked(value)) {
    double rpm = value*500;
    info.ProvideRPM(rpm);
  }

  // data3
  if (line.ReadChecked(value)) {
    double temp = (value*4.6)-50;  // temp in °C
    temp+= 273.15; // temp in Kelvin
    info.ProvideExhaustTemperature(temp);
  }

  // data4
  if (line.ReadChecked(value)) {
    // Throttle setting: value/2
    info.ProvideThrottleSetting(value/2);
  }

  // data5
  if (line.ReadChecked(value)) {
    // Voltage: value*8,30/255
  }

  return true;
}


class EcuDevice : public AbstractDevice {
  Port &port;

public:
  EcuDevice(Port &_port):port(_port) {}

  /* virtual methods from class Device */
  bool ParseNMEA(const char *line, struct NMEAInfo &info) override;

};

bool
EcuDevice::ParseNMEA(const char *_line, NMEAInfo &info)
{
  /*
  if (!VerifyNMEAChecksum(_line))
    return false;
  */

  NMEAInputLine line(_line);
  char type[16];
  line.Read(type, 16);

  if (StringIsEqual(type, "255"))
    return Parse0FF(line, info);
  else
    return false;
}

static Device *
AmtEcuCreateOnPort(const DeviceConfig &config, Port &com_port)
{
  return new EcuDevice(com_port);
}

const struct DeviceRegister amt_ecu_driver = {
  _T("ECU"),
  _T("AMT ECU"),
  0,
  AmtEcuCreateOnPort,
};
