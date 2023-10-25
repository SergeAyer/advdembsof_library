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
 * @file cpu_logger.Cpp
 * @author Serge Ayer <serge.ayer@hefr.ch>
 *
 * @brief CPU logger (for cpu utilisation) implementation
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "cpu_logger.hpp"

#include <chrono>

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "CPULogger"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

namespace advembsof {

CPULogger::CPULogger(Timer& timer) : _timer(timer) {}

void CPULogger::printStats() {
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    std::chrono::microseconds time = _timer.elapsed_time();
    if (_lastTime != std::chrono::microseconds::zero()) {
        std::chrono::microseconds elapsedTime = time - _lastTime;

        // Calculate the percentage of CPU usage
        us_timestamp_t up_usec_in_time_range   = stats.uptime - _stats.uptime;
        us_timestamp_t idle_usec_in_time_range = stats.idle_time - _stats.idle_time;
        uint32_t usage = 100 - ((idle_usec_in_time_range * 100) / elapsedTime.count());
        tr_info("Up time since start: %" PRIu64 " msecs", stats.uptime / 1000);
        tr_info("Up time in period: %" PRIu64 " msecs", up_usec_in_time_range / 1000);
        tr_info("Idle time in period: %" PRIu64 " msecs", idle_usec_in_time_range / 1000);
        tr_info("Usage in period: %" PRIu32 " %%", usage);
    }

    _stats    = stats;
    _lastTime = time;
}

}  // namespace advembsof
