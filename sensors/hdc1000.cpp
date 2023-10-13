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
 * @file hdc1000.cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief HDC1000 implementation
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "hdc1000.hpp"

#include "mbed_error.h"

namespace advembsof {

// definitions
#define HDC1000_TEMPERATURE 0x00
#define HDC1000_HUMIDITY 0x01

HDC1000::HDC1000(PinName sda, PinName scl, PinName dataRdy)
    : I2C(sda, scl), _address(0x40 << 1), _dataReadyPin(dataRdy, PullUp) {}

bool HDC1000::probe(void) {
    char rx_data = 0;
    int rc       = read(_address | 0x1, &rx_data, static_cast<int>(sizeof(rx_data)));
    return (rc == 0);
}

double HDC1000::getTemperature(void) {
    double temperature = getRawTemperature();
    return (temperature / 65536.0) * 165.0 - 40.0;
}

double HDC1000::getHumidity(void) {
    double humidity = getRawHumidity();
    return (humidity / 65536.0) * 100.0;
}

uint16_t HDC1000::getRawTemperature(void) {
    mbed_error_status_t rc = setReadRegister(HDC1000_TEMPERATURE);
    if (rc != MBED_SUCCESS) {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, rc),
                   "HDC1000Device: getRawTemperature() failed");
    }
    return read16();
}

uint16_t HDC1000::getRawHumidity(void) {
    mbed_error_status_t rc = setReadRegister(HDC1000_HUMIDITY);
    if (rc != MBED_SUCCESS) {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, rc),
                   "HDC1000Device: getRawHumidity() failed");
    }
    return read16();
}

mbed_error_status_t HDC1000::setReadRegister(uint8_t reg) {
    // write register, device will thus read data and signal data ready
    const char txData[1] = {reg};
    int rc = write(_address, txData, static_cast<int>(sizeof(txData)), true);
    if (rc != 0) {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_WRITE_FAILED),
                   "HDC1000Device: Could not write to register");
    }

    // busy wait for data ready
    uint16_t iteration = 0;
    while (_dataReadyPin != kDataReadyActive && iteration++ < 100) {
        ThisThread::sleep_for(10ms);
    }

    return MBED_SUCCESS;
}

uint16_t HDC1000::read16(void) {
    char rxData[2] = {0};
    int rc         = read(_address, rxData, sizeof(rxData));
    if (rc != 0) {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_READ_FAILED),
                   "HDC1000Device: Could not read data");
    }

    uint16_t value = (((uint16_t)rxData[0]) << 8) + (uint16_t)rxData[1];
    // m_logger.log("Value read is %d (%d %d)\r\n", value, rxData[1], rxData[0]);

    return value;
}

}  // namespace advembsof
