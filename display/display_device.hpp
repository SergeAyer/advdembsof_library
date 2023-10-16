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
 * @file display_device.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Display device header file (static scheduling)
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "bike_display.hpp"
#include "mbed.h"

namespace advembsof {

class DisplayDevice {
   public:
    DisplayDevice();

    // to be called for initializing the LCD display
    disco::ReturnCode init();

    // methods called for displaying information
    void displayGear(uint8_t currentGear);
    void displaySpeed(float speed);
    void displayDistance(float distance);
    void displayTemperature(float temperature);

   private:
    // data members
    // definition of task period time
    static constexpr std::chrono::milliseconds kTaskPeriod = 1600ms;
    // definition of task execution time
    static constexpr std::chrono::microseconds kTaskRunTime    = 300000us;
    static constexpr std::chrono::microseconds kSubTaskRunTime = 100000us;
    // nbr of subtasks for static cyclic scheduling
    static constexpr uint8_t kNbrOfSubTasks = 3;

    // BikeDisplay instance used for display information on the device screen
    advembsof::BikeDisplay _bikeDisplay;
};

}  // namespace advembsof
