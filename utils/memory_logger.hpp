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
 * @brief Memory logger header file
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#pragma once

#include "mbed.h"

namespace advembsof {

#if defined(MBED_ALL_STATS_ENABLED)

class MemoryLogger {
   public:
    // methods used by owners
    void getAndPrintStatistics();
    void printDiffs();
    void printRuntimeMemoryMap();

    void getAndPrintHeapStatistics();
    void getAndPrintStackStatistics();

   private:
    // data members
    static constexpr uint8_t kMaxThreadInfo       = 10;
    mbed_stats_heap_t _heapInfo                   = {0};
    mbed_stats_stack_t _stackInfo[kMaxThreadInfo] = {0};
    mbed_stats_stack_t _globalStackInfo           = {0};
};

#endif  // MBED_ALL_STATS_ENABLED

}  // namespace advembsof
