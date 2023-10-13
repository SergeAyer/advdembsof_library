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
 * @file hdc1000.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief HDC1000 header file
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace advembsof {

class HDC1000 : public I2C {
   public:
    // constructor
    HDC1000(PinName sda, PinName scl, PinName dataRdy);

    // method for checking device presence
    bool probe(void);

    // method for getting the different measurements
    double getTemperature(void);
    double getHumidity(void);

   private:
    // private methods
    uint16_t getRawTemperature(void);
    uint16_t getRawHumidity(void);
    mbed_error_status_t setReadRegister(uint8_t reg);
    uint16_t read16(void);

    // constants
    // data ready pin is active low
    static constexpr uint8_t kDataReadyActive = 0;

    // data members
    int _address;
    DigitalIn _dataReadyPin;
};

}  // namespace advembsof
