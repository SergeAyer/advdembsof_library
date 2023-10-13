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
 * @file bme.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief BME280 header file
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace advembsof {

#define _debug 0  // '1' to enable prinf BME's registers

class BME {
   public:
    BME(PinName sda, PinName scl, char slave_adr);

    int init(void);
    int chipID(void);
    float getTemperature(void);
    float getPressure(void);
    float getHumidity(void);

   private:
    void initialize(void);

    I2C bme;
    uint16_t chip_id = 0;
    char address     = 0;
    uint16_t dig_T1  = 0;
    int16_t dig_T2 = 0, dig_T3 = 0;
    uint16_t dig_P1 = 0;
    int16_t dig_P2 = 0, dig_P3 = 0, dig_P4 = 0, dig_P5 = 0, dig_P6 = 0, dig_P7 = 0,
            dig_P8 = 0, dig_P9 = 0;
    uint16_t dig_H1 = 0, dig_H3 = 0;
    int16_t dig_H2 = 0, dig_H4 = 0, dig_H5 = 0, dig_H6 = 0;
    int32_t t_fine = 0;
    bool dbg_on    = false;
};

}  // namespace advembsof
