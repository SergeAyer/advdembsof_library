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
 * @file bike_system.hpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief Task logger (for times) implementation
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "task_logger.hpp"

#include <chrono>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "TaskLogger"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace advembsof {

const char* TaskLogger::kTaskDescriptors[TaskLogger::kNbrOfTasks] = {
    const_cast<char*>("Gear"),
    const_cast<char*>("Speed"),
    const_cast<char*>("Temperature"),
    const_cast<char*>("Reset"),
    const_cast<char*>("Display(1)"),
    const_cast<char*>("Display(2)")};

void TaskLogger::enable(bool enable) { _isEnabled = enable; }

#if defined(MBED_TEST_MODE)
std::chrono::microseconds TaskLogger::getPeriod(uint8_t taskIndex) const {
    if (taskIndex >= kNbrOfTasks) {
        return std::chrono::microseconds::zero();
    }
    return _taskPeriod[taskIndex];
}

std::chrono::microseconds TaskLogger::getComputationTime(uint8_t taskIndex) const {
    if (taskIndex >= kNbrOfTasks) {
        return std::chrono::microseconds::zero();
    }
    return _taskComputationTime[taskIndex];
}
#endif  // defined(MBED_TEST_MODE)

void TaskLogger::logPeriodAndExecutionTime(
    Timer& timer, int taskIndex, const std::chrono::microseconds& taskStartTime) {
    bool firstCall = _taskStartTime[taskIndex] == std::chrono::microseconds::zero();
    std::chrono::microseconds taskPeriod = taskStartTime - _taskStartTime[taskIndex];
    _taskStartTime[taskIndex]            = taskStartTime;
    if (!firstCall && _isEnabled) {
        uint32_t nbrOfCalls = _nbrOfCalls[taskIndex];
        _taskPeriod[taskIndex] =
            ((_taskPeriod[taskIndex] * nbrOfCalls) + taskPeriod) / (nbrOfCalls + 1);
        std::chrono::microseconds taskEndTime = timer.elapsed_time();
        std::chrono::microseconds taskComputationTime =
            taskEndTime - _taskStartTime[taskIndex];
        _taskComputationTime[taskIndex] =
            ((_taskComputationTime[taskIndex] * nbrOfCalls) + taskComputationTime) /
            (nbrOfCalls + 1);
        tr_debug("%s task: period %" PRIu64 " usecs execution time %" PRIu64
                 " usecs start time %" PRIu64 " usecs",
                 kTaskDescriptors[taskIndex],
                 _taskPeriod[taskIndex].count(),
                 _taskComputationTime[taskIndex].count(),
                 _taskStartTime[taskIndex].count());
        _nbrOfCalls[taskIndex]++;
    }
}

}  // namespace advembsof
