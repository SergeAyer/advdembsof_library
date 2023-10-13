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
 * @file bme.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief BME280 implementation
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "bme.hpp"

#include "mbed_debug.h"

namespace advembsof {

BME::BME(PinName sda, PinName scl, char slave_adr) : bme(sda, scl) {
    dbg_on  = _debug;
    address = slave_adr;
}

void BME::initialize() {
    char cmd[18];

    debug_if(dbg_on, "\033[0m\033[2J\033[H ++++ BME-P register's ++++\r\n\n");

    cmd[0] = 0xF2;  // ctrl_hum
    cmd[1] = 0x01;  // Humidity oversampling x1
    bme.write(address, cmd, 2);

    cmd[0] = 0xF4;  // ctrl_meas
    cmd[1] = 0x27;  // Temparature oversampling x1, Pressure oversampling x1, Normal mode
    bme.write(address, cmd, 2);

    cmd[0] = 0xF5;  // config
    cmd[1] = 0xa0;  // Standby 1000ms, Filter off
    bme.write(address, cmd, 2);

    // sensor registers
    debug_if(dbg_on, "chip_id = 0x%x\n\n", chip_id);

    cmd[0] = 0x88;  // read dig_T calibration regs
    bme.write(address, cmd, 1);
    bme.read(address, cmd, 6);
    dig_T1 = (cmd[1] << 8) | cmd[0];
    dig_T2 = (cmd[3] << 8) | cmd[2];
    dig_T3 = (cmd[5] << 8) | cmd[4];
    debug_if(dbg_on,
             "Temp Cal reg's:\nT1 = 0x%x\nT2 = 0x%x\nT3 = 0x%x\n\n",
             dig_T1,
             dig_T2,
             dig_T3);

    cmd[0] = 0x8E;  // read dig_P calibration regs
    bme.write(address, cmd, 1);
    bme.read(address, cmd, 18);
    dig_P1 = (cmd[1] << 8) | cmd[0];
    dig_P2 = (cmd[3] << 8) | cmd[2];
    dig_P3 = (cmd[5] << 8) | cmd[4];
    dig_P4 = (cmd[7] << 8) | cmd[6];
    dig_P5 = (cmd[9] << 8) | cmd[8];
    dig_P6 = (cmd[11] << 8) | cmd[10];
    dig_P7 = (cmd[13] << 8) | cmd[12];
    dig_P8 = (cmd[15] << 8) | cmd[14];
    dig_P9 = (cmd[17] << 8) | cmd[16];
    debug_if(dbg_on,
             "Pressure Cal reg's:\nP1 = 0x%x\nP2 = 0x%x\nP3 = 0x%x\nP4 = 0x%x\n",
             dig_P1,
             dig_P2,
             dig_P3,
             dig_P4);
    debug_if(dbg_on,
             "P5 = 0x%x\nP6 = 0x%x\nP7 = 0x%x\nP8 = 0x%x\nP9 = 0x%x\n\n",
             dig_P5,
             dig_P6,
             dig_P7,
             dig_P8,
             dig_P9);

    if (chip_id == 0x60) {  // Only BME280 has Humidity
        cmd[0] = 0xA1;      // read dig_H calibration LSB regs
        bme.write(address, cmd, 1);
        bme.read(address, cmd, 1);
        cmd[1] = 0xE1;  // read dig_H calibration MSB regs
        bme.write(address, &cmd[1], 1);
        bme.read(address, &cmd[1], 7);
        dig_H1 = cmd[0];
        dig_H2 = (cmd[2] << 8) | cmd[1];
        dig_H3 = cmd[3];
        dig_H4 = (cmd[4] << 4) | (cmd[5] & 0x0f);
        dig_H5 = (cmd[6] << 4) | ((cmd[5] >> 4) & 0x0f);
        dig_H6 = cmd[7];
        debug_if(dbg_on,
                 "Humidity Cal reg's:\nH1 = 0x%x\nH2 = 0x%x\nH3 = 0x%x\n",
                 dig_H1,
                 dig_H2,
                 dig_H3);
        debug_if(dbg_on, "H4 = 0x%x\nH5 = 0x%x\nH6 = 0x%x\n", dig_H4, dig_H5, dig_H6);
    }
}

int BME::init() {
    char cmd[2];
    cmd[0] = 0xE0;  // reset reg
    cmd[1] = 0xB6;
    bme.write(address, cmd, 2);
    if (chipID()) {
        initialize();
        return chip_id;
    } else {
        return 0;
    }
}

int BME::chipID() {
    char cmd[1];
    cmd[0] = 0xD0;  // chip_id
    bme.write(address, cmd, 1);
    cmd[0] = 0x00;
    bme.read(address, cmd, 1);
    chip_id = cmd[0];
    return chip_id;
}

float BME::getTemperature() {
    if (!chipID()) {
        init();
    }  // check if live sensor

    int32_t var1, var2, T, adc_T;
    float temp;
    char cmd[4];
    cmd[0] = 0xFA;  // temp_msb
    bme.write(address, cmd, 1);
    bme.read(address, &cmd[1], 3);

    adc_T = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >>
             12) *
            ((int32_t)dig_T3)) >>
           14;
    t_fine = var1 + var2;
    T      = (t_fine * 5 + 128) >> 8;
    temp   = T / 100.0;
    if (temp > -41 && temp < 86) {  // return temperature if within device limits.
        return temp;
    } else {
        return 99.99;  // error value
    }
}

float BME::getPressure() {
    if (!chipID()) {
        init();
    }  // check if live sensor

    uint32_t adc_P;
    int64_t var1, var2, p;
    float press;
    char cmd[4];
    cmd[0] = 0xF7;  // press_msb
    bme.write(address, cmd, 1);
    bme.read(address, &cmd[1], 3);

    adc_P = (cmd[1] << 12) | (cmd[2] << 4) | (cmd[3] >> 4);

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) {
        return 0;
    }
    p     = 1048576 - adc_P;
    p     = (((p << 31) - var2) * 3125) / var1;
    var1  = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2  = (((int64_t)dig_P8) * p) >> 19;
    p     = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
    press = (static_cast<float>(p) / 256) / 100.0f;
    if (press > 300 && press < 1100) {  // return temperature if within device limits.
        return press;
    } else {
        return 9999;  // error value
    }
}

float BME::getHumidity() {
    if (!chipID()) {
        init();
    }  // check if live sensor

    uint32_t humid_raw;
    int32_t v_x1r;
    float humid;
    char cmd[4];
    cmd[0] = 0xfd;  // hum_msb
    bme.write(address, cmd, 1);
    bme.read(address, &cmd[1], 2);

    humid_raw = (cmd[1] << 8) | cmd[2];

    v_x1r = (t_fine - 76800);
    v_x1r =
        (((((humid_raw << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1r)) +
           ((int32_t)16384)) >>
          15) *
         (((((((v_x1r * (int32_t)dig_H6) >> 10) *
              (((v_x1r * ((int32_t)dig_H3)) >> 11) + 32768)) >>
             10) +
            2097152) *
               (int32_t)dig_H2 +
           8192) >>
          14));
    v_x1r = (v_x1r - (((((v_x1r >> 15) * (v_x1r >> 15)) >> 7) * (int32_t)dig_H1) >> 4));
    v_x1r = (v_x1r < 0 ? 0 : v_x1r);
    v_x1r = (v_x1r > 419430400 ? 419430400 : v_x1r);

    humid = (static_cast<float>(v_x1r >> 12)) / 1024.0f;

    return humid;
}

}  // namespace advembsof
