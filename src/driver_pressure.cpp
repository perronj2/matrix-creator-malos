/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator MALOS
 *
 * MATRIX Creator MALOS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "./driver_pressure.h"
#include <matrix_io/malos/v1/driver.pb.h>
#include <string>
#include "matrix_hal/pressure_data.h"

namespace pb = matrix_io::malos::v1;

namespace matrix_malos {

bool PressureDriver::SendUpdate() {
  matrix_hal::PressureData data;
  if (!reader_->Read(&data)) {
    return false;
  }

  pb::sense::Pressure pressure_pb;
  pressure_pb.set_pressure(data.pressure);
  pressure_pb.set_altitude(data.altitude);
  pressure_pb.set_temperature(data.temperature);

  std::string buffer;
  pressure_pb.SerializeToString(&buffer);
  zqm_push_update_->Send(buffer);

  return true;
}

}  // namespace matrix_malos
