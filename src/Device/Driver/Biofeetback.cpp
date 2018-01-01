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

#include "Device/Driver/Biofeetback.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "NMEA/Checksum.hpp"
#include <stdint.h>

static bool
ParseBFV1(NMEAInputLine &line, NMEAInfo &info)
{
  int value;
  // pressure of left pedal [g]
  if (line.ReadChecked(value)) {
    
  }

  // pressure of right pedal [g]
  if (line.ReadChecked(value)) {
    
  }

  // set reference pressure [g]
  if (line.ReadChecked(value)) {
    
  }

  // vibration motor status, 0: inactive, 1: active
  if (line.ReadChecked(value)) {
    
  }
 
  return true;
}
 

class Biofeetbackdevice : public AbstractDevice {
  Port &port;

public:
  Biofeetbackdevice(Port &_port):port(_port) {}

  /* virtual methods from class Device */
  bool ParseNMEA(const char *line, struct NMEAInfo &info) override;
};

bool Biofeetbackdevice::ParseNMEA(const char *_line, NMEAInfo &info) {
  if (!VerifyNMEAChecksum(_line))
    return false;

  NMEAInputLine line(_line);
  char type[16];
  line.Read(type, 16);
 
  if (StringIsEqual(type, "$BFV1"))
    ParseBFV1(line, info);
  else
    return false;

  return true;
}

static Device *
BiofeetCreateOnPort(const DeviceConfig &config, Port &com_port) {
  return new Biofeetbackdevice(com_port);
}

const struct DeviceRegister biofeetback_driver  = {
  _T("Biofeetbacksystem"),
  _T("RBE Biofeetbacksystem"),
  0,
  BiofeetCreateOnPort,
};
