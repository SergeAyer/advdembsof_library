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
 * @file bike_display.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Bike Display header file
 *
 * @date 2023-07-05
 * @version 0.0.1
 ***************************************************************************/

#pragma once

#include "mbed.h"

// from DISCO_H747I
#include "lcd_display.hpp"

namespace advembsof {

class BikeDisplay {
   public:
    // constructor
    BikeDisplay();

    // to be called prior to any other method
    disco::ReturnCode init();

    void displayGear(uint8_t gear);
    void displaySpeed(float speed);
    void displayDistance(float distance);
    void displayTemperature(float temperature);
    void reset();

   private:
    // private methods
    void displayInfo();
    void displayIcons();
    void drawLines();
    void computePositions();

#if defined(SHOW_WELCOME)
    void onJoystickLeft();
    void onJoystickRight();
    void onJoystickDown();
    void onJoystickUp();
    void onJoystickSel();
    void onWakeUp();
    void displayWelcome();
#endif

    static constexpr uint32_t kLineWidth       = 2;
    static constexpr uint32_t kIconXMargin     = 20;
    static constexpr uint32_t kTextXMargin     = 40;
    static constexpr uint8_t kSpeedometerIndex = 0;
    static constexpr uint8_t kGearIndex        = 1;
    static constexpr uint8_t kTemperatureIndex = 2;
    static constexpr uint8_t kDistanceIndex    = 3;
#if defined(SHOW_WELCOME)
    static constexpr uint8_t kLogoIndex = 4;
    static constexpr uint16_t kLogoXPos = 280;
    static constexpr uint16_t kLogoYPos = 350;
    static const char* szWelcomeString  = "TSM_AdvEmbSof";
#endif
    uint32_t _horLineYPos            = 0;
    uint32_t _vertLineXPos           = 0;
    uint32_t _infoBoxHeight          = 0;
    uint32_t _speedometerIconXPos    = 0;
    uint32_t _speedometerIconYPos    = 0;
    uint32_t _speedometerTextMidXPos = 0;
    uint32_t _speedometerTextYPos    = 0;
    uint32_t _gearIconXPos           = 0;
    uint32_t _gearIconYPos           = 0;
    uint32_t _gearTextMidXPos        = 0;
    uint32_t _gearTextYPos           = 0;
    uint32_t _temperatureIconXPos    = 0;
    uint32_t _temperatureIconYPos    = 0;
    uint32_t _temperatureTextMidXPos = 0;
    uint32_t _temperatureTextYPos    = 0;
    uint32_t _distanceIconXPos       = 0;
    uint32_t _distanceIconYPos       = 0;
    uint32_t _distanceTextMidXPos    = 0;
    uint32_t _distanceTextYPos       = 0;

#if defined(SHOW_WELCOME)
    bool _showPicture                       = true;
    bool _showWelcome                       = true;
    disco::LCDDisplay::AlignMode _alignMode = disco::LCDDisplay::AlignMode::CENTER_MODE;
    uint32_t _logoXPos;
#endif
};

}  // namespace advembsof
