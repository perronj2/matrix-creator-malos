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

#include <iostream>
#include <string>

#include <matrix_io/malos/v1/driver.pb.h>
#include "./driver_gpio.h"

namespace pb = matrix_io::malos::v1;

namespace matrix_malos {

const bool kGpioDriverDebugEnabled = false;

bool GpioDriver::ProcessConfig(const pb::driver::DriverConfig& config) {
  pb::io::GpioParams gpio_config(config.gpio());
  const int16_t pin = static_cast<int16_t>(gpio_config.pin());
  const int16_t mode = static_cast<int16_t>(gpio_config.mode());

  if (mode == pb::io::GpioParams::OUTPUT) {
    gpio_->SetGPIOValue(pin, gpio_config.value());
  } else if (!(mode == pb::io::GpioParams::OUTPUT ||
               mode == pb::io::GpioParams::INPUT)) {
    zmq_push_error_->Send(
        "invalid gpio mode. check the proto GpioParams (file "
        "driver.proto)");
    return false;
  }

  gpio_->SetMode(pin, mode);

  return true;
}

bool GpioDriver::SendUpdate() {
  pb::io::GpioParams gpiopb;
  gpiopb.set_values(gpio_->GetGPIOValues());
  std::string buffer;
  gpiopb.SerializeToString(&buffer);
  zqm_push_update_->Send(buffer);

  return true;
}

}  // namespace matrix_malos
