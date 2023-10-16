// Copyright 2022 Haute école d'ingénierie et d'architecture de Fribourg
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/****************************************************************************
 * @file display_device.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Display device implementation (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "display_device.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "LCDDisplay"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace advembsof {

DisplayDevice::DisplayDevice() {}

disco::ReturnCode DisplayDevice::init() {
    disco::ReturnCode rc = _bikeDisplay.init();
    if (rc != disco::ReturnCode::Ok) {
        tr_error("Failed to initialize display: %d", static_cast<int>(rc));
    }
    return rc;
}

void DisplayDevice::displayGear(uint8_t currentGear) {
    _bikeDisplay.displayGear(currentGear);
}

void DisplayDevice::displaySpeed(float speed) { _bikeDisplay.displaySpeed(speed); }

void DisplayDevice::displayDistance(float distance) {
    _bikeDisplay.displayDistance(distance);
}

void DisplayDevice::displayTemperature(float temperature) {
    _bikeDisplay.displayTemperature(temperature);
}

}  // namespace advembsof
