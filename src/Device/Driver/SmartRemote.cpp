/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2018 The XCSoar Project
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
 
#include "Device/Driver/SmartRemote.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "NMEA/Checksum.hpp"

#include <stdint.h>

static bool
ParseRBEV6(NMEAInputLine &line, NMEAInfo &info)
{
  // climb/cruise switch
  switch (line.Read(-1)) {
  case 1:
    info.switch_state.flight_mode = SwitchState::FlightMode::CRUISE;
    break;
  case 0:
    info.switch_state.flight_mode = SwitchState::FlightMode::CIRCLING;
    break;
  }

  return true;
}

static bool
ParseRBEV7(NMEAInputLine &line, NMEAInfo &info)
{
  int value;

  // Pulse
  if (line.ReadChecked(value)) {
    info.ProvidePulse(value);
  }

  // SPO2 Saturation
  if (line.ReadChecked(value)) {
     info.ProvideSPO2Saturation(value);
  }

  return true;
}

class RemoteDevice : public AbstractDevice {
  Port &port;

public:
  RemoteDevice(Port &_port):port(_port) {}

  /* virtual methods from class Device */
  bool ParseNMEA(const char *line, struct NMEAInfo &info) override;
};

bool
RemoteDevice::ParseNMEA(const char *_line, NMEAInfo &info)
{
  if (!VerifyNMEAChecksum(_line))
    return false;

  NMEAInputLine line(_line);
  char type[16];
  line.Read(type, 16);

  if (StringIsEqual(type, "$RBEV6"))
    return ParseRBEV6(line, info);
  else if (StringIsEqual(type, "$RBEV7"))
    return ParseRBEV7(line, info);
  else
    return false;
}

static Device *
RemoteCreateOnPort(const DeviceConfig &config, Port &com_port)
{
  return new RemoteDevice(com_port);
}
 
const struct DeviceRegister remote_driver = {
  _T("Smart-Remote"),
  _T("RBE Smart-Remote"),
  0,
  RemoteCreateOnPort,
};
 

