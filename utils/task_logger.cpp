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

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "TaskLogger"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace advembsof {

const char* TaskLogger::kTaskDescriptors[TaskLogger::kNbrOfTasks] = {
    const_cast<char*>("Gear"),
    const_cast<char*>("Speed"),
    const_cast<char*>("Sensor"),
    const_cast<char*>("Reset"),
    const_cast<char*>("Display")};

void TaskLogger::enable(bool enable) { _isEnabled = enable; }

void TaskLogger::logPeriodAndExecutionTime(
    Timer& timer, int taskIndex, const std::chrono::microseconds& taskStartTime) {
    bool firstCall = _taskStartTime[taskIndex] == std::chrono::microseconds::zero();
    std::chrono::microseconds periodTime = taskStartTime - _taskStartTime[taskIndex];
    _taskStartTime[taskIndex]            = taskStartTime;
    if (!firstCall && _isEnabled) {
        std::chrono::microseconds taskEndTime   = timer.elapsed_time();
        std::chrono::microseconds executionTime = taskEndTime - _taskStartTime[taskIndex];
        tr_debug("%s task: period %" PRIu64 " usecs execution time %" PRIu64
                 " usecs start time %" PRIu64 " usecs",
                 kTaskDescriptors[taskIndex],
                 periodTime.count(),
                 executionTime.count(),
                 _taskStartTime[taskIndex].count());
    }
}

}  // namespace advembsof
