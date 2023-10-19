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
 * @file task_logger.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Task logger (for times) header file
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include <chrono>

#include "mbed.h"

namespace advembsof {

class TaskLogger {
   public:
    TaskLogger() = default;
    
    void enable(bool enable);
    void logPeriodAndExecutionTime(Timer& timer,  // NOLINT(runtime/references)
                                   int taskIndex,
                                   const std::chrono::microseconds& taskStartTime);

    static constexpr int kNbrOfTasks           = 6;
    static constexpr int kGearTaskIndex        = 0;
    static constexpr int kSpeedTaskIndex       = 1;
    static constexpr int kTemperatureTaskIndex = 2;
    static constexpr int kResetTaskIndex       = 3;
    static constexpr int kDisplayTask1Index    = 4;
    static constexpr int kDisplayTask2Index    = 5;

#if defined(MBED_TEST_MODE)
    std::chrono::microseconds getPeriod(uint8_t taskIndex) const;
    std::chrono::microseconds getComputationTime(uint8_t taskIndex) const;
#endif

   private:
    // constants
    static const char* kTaskDescriptors[TaskLogger::kNbrOfTasks];

    // data members
    bool _isEnabled                                       = true;
    uint32_t _nbrOfCalls[kNbrOfTasks]                     = {0};
    std::chrono::microseconds _taskStartTime[kNbrOfTasks] = {
        std::chrono::microseconds::zero()};
    std::chrono::microseconds _taskPeriod[kNbrOfTasks] = {
        std::chrono::microseconds::zero()};
    std::chrono::microseconds _taskComputationTime[kNbrOfTasks] = {
        std::chrono::microseconds::zero()};
};

}  // namespace advembsof
