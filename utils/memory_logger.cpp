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
 * @brief Memory logger implementation
 *
 * @date 2023-08-20
 * @version 1.0.0
 ***************************************************************************/

#include "memory_logger.hpp"

#include "mbed_trace.h"
#if MBED_CONF_MBED_TRACE_ENABLE
#define TRACE_GROUP "MemoryLogger"
#endif  // MBED_CONF_MBED_TRACE_ENABLE

#if defined(MBED_ALL_STATS_ENABLED)
extern unsigned char* mbed_stack_isr_start;
extern uint32_t mbed_stack_isr_size;
extern unsigned char* mbed_heap_start;
extern uint32_t mbed_heap_size;
#endif  // MBED_ALL_STATS_ENABLED

namespace advembsof {

#if defined(MBED_ALL_STATS_ENABLED)

void MemoryLogger::printDiffs() {
    {
        tr_debug("MemoryStats (Heap):");
        mbed_stats_heap_t heapInfo = {0};
        mbed_stats_heap_get(&heapInfo);
        uint32_t currentSizeDiff = heapInfo.current_size - _heapInfo.current_size;
        if (currentSizeDiff > 0) {
            tr_debug("\tBytes allocated increased by %" PRIu32 " to %" PRIu32 " bytes",
                     currentSizeDiff,
                     heapInfo.current_size);
        }
        uint32_t maxSizeDiff = heapInfo.max_size - _heapInfo.max_size;
        if (maxSizeDiff > 0) {
            tr_debug("\tMax bytes allocated at a given time increased by %" PRIu32
                     " to %" PRIu32 " bytes (max heap size is %" PRIu32 " bytes)",
                     maxSizeDiff,
                     heapInfo.max_size,
                     heapInfo.reserved_size);
        }
        _heapInfo = heapInfo;
    }
    {
        mbed_stats_stack_t globalStackInfo = {0};
        mbed_stats_stack_get(&globalStackInfo);
        tr_debug("Cumulative Stack Info:");
        uint32_t maxSizeDiff = globalStackInfo.max_size - _globalStackInfo.max_size;
        if (maxSizeDiff > 0) {
            tr_debug("\tMaximum number of bytes used on the stack increased by %" PRIu32
                     " to %" PRIu32 " bytes (stack size is %" PRIu32 " bytes)",
                     maxSizeDiff,
                     globalStackInfo.max_size,
                     globalStackInfo.reserved_size);
        }
        uint32_t stackCntDiff = globalStackInfo.stack_cnt - _globalStackInfo.stack_cnt;
        if (stackCntDiff > 0) {
            tr_debug("\tNumber of stacks stats accumulated increased by %" PRIu32
                     " to %" PRIu32 "",
                     stackCntDiff,
                     globalStackInfo.stack_cnt);
        }
        _globalStackInfo = globalStackInfo;

        mbed_stats_stack_t stackInfo[kMaxThreadInfo] = {0};
        mbed_stats_stack_get_each(stackInfo, kMaxThreadInfo);
        tr_debug("Thread Stack Info:");
        for (uint32_t i = 0; i < kMaxThreadInfo; i++) {
            if (stackInfo[i].thread_id != 0) {
                for (uint32_t j = 0; j < kMaxThreadInfo; j++) {
                    if (stackInfo[i].thread_id == _stackInfo[j].thread_id) {
                        maxSizeDiff = stackInfo[i].max_size - _stackInfo[j].max_size;
                        if (maxSizeDiff > 0) {
                            tr_debug("\tThread: %" PRIu32 "", j);
                            tr_debug(
                                "\t\tThread Id: 0x%08" PRIx32 " with name %s",
                                _stackInfo[j].thread_id,
                                osThreadGetName((osThreadId_t)_stackInfo[j].thread_id));
                            tr_debug(
                                "\t\tMaximum number of bytes used on the stack increased "
                                "by %" PRIu32 " to %" PRIu32
                                " bytes (stack size is %" PRIu32 " bytes)",
                                maxSizeDiff,
                                stackInfo[i].max_size,
                                stackInfo[i].reserved_size);
                        }
                        _stackInfo[j] = stackInfo[i];
                    }
                }
            }
        }
    }
}

void MemoryLogger::getAndPrintHeapStatistics() {
    tr_debug("MemoryStats (Heap):");
    mbed_stats_heap_get(&_heapInfo);
    tr_debug("\tBytes allocated currently: %" PRIu32 "", _heapInfo.current_size);
    tr_debug("\tMax bytes allocated at a given time: %" PRIu32 "", _heapInfo.max_size);
    tr_debug("\tCumulative sum of bytes ever allocated: %" PRIu32 "",
             _heapInfo.total_size);
    tr_debug("\tCurrent number of bytes allocated for the heap: %" PRIu32 "",
             _heapInfo.reserved_size);
    tr_debug("\tCurrent number of allocations: %" PRIu32 "", _heapInfo.alloc_cnt);
    tr_debug("\tNumber of failed allocations: %" PRIu32 "", _heapInfo.alloc_fail_cnt);
}

void MemoryLogger::getAndPrintStackStatistics() {
    mbed_stats_stack_get(&_globalStackInfo);
    tr_debug("Cumulative Stack Info:");
    tr_debug("\tMaximum number of bytes used on the stack: %" PRIu32 "",
             _globalStackInfo.max_size);
    tr_debug("\tCurrent number of bytes allocated for the stack: %" PRIu32 "",
             _globalStackInfo.reserved_size);
    tr_debug("\tNumber of stacks stats accumulated in the structure: %" PRIu32 "",
             _globalStackInfo.stack_cnt);

    mbed_stats_stack_get_each(_stackInfo, kMaxThreadInfo);
    tr_debug("Thread Stack Info:");
    for (uint32_t i = 0; i < kMaxThreadInfo; i++) {
        if (_stackInfo[i].thread_id != 0) {
            tr_debug("\tThread: %" PRIu32 "", i);
            tr_debug("\t\tThread Id: 0x%08" PRIx32 " with name %s",
                     _stackInfo[i].thread_id,
                     osThreadGetName((osThreadId_t)_stackInfo[i].thread_id));
            tr_debug("\t\tMaximum number of bytes used on the stack: %" PRIu32 "",
                     _stackInfo[i].max_size);
            tr_debug("\t\tCurrent number of bytes allocated for the stack: %" PRIu32 "",
                     _stackInfo[i].reserved_size);
            tr_debug("\t\tNumber of stacks stats accumulated in the structure: %" PRIu32
                     "",
                     _stackInfo[i].stack_cnt);
        }
    }
}

void MemoryLogger::getAndPrintStatistics() {
    getAndPrintHeapStatistics();
    getAndPrintStackStatistics();
}

void MemoryLogger::printRuntimeMemoryMap() {
    // defined in rtx_thread.c
    // uint32_t osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items)
    tr_debug("Runtime Memory Map:");
    osThreadId_t threadIdArray[kMaxThreadInfo] = {0};
    uint32_t nbrOfThreads = osThreadEnumerate(threadIdArray, kMaxThreadInfo);
    for (uint32_t threadIndex = 0; threadIndex < nbrOfThreads; threadIndex++) {
        osRtxThread_t* pThreadCB =
            // cppcheck-suppress cstyleCast
            (osRtxThread_t*)threadIdArray[threadIndex];  // NOLINT(readability/casting)
        uint8_t state             = pThreadCB->state & osRtxThreadStateMask;
        const char* szThreadState = (state & osThreadInactive)     ? "Inactive"
                                    : (state & osThreadReady)      ? "Ready"
                                    : (state & osThreadRunning)    ? "Running"
                                    : (state & osThreadBlocked)    ? "Blocked"
                                    : (state & osThreadTerminated) ? "Terminated"
                                                                   : "Unknown";
        tr_debug("\t thread with name %s, stack_start: %p, stack_end: %p, size: %" PRIu32
                 ", priority: %" PRIu8 ", state: %s",
                 pThreadCB->name,
                 pThreadCB->stack_mem,
                 // cppcheck-suppress cstyleCast
                 (char*)pThreadCB->stack_mem +  // NOLINT(readability/casting)
                     pThreadCB->stack_size,
                 pThreadCB->stack_size,
                 pThreadCB->priority,
                 szThreadState);
    }
    tr_debug("\t mbed_heap_start: %p, mbed_heap_end: %p, size: %" PRIu32 "",
             mbed_heap_start,
             (mbed_heap_start + mbed_heap_size),
             mbed_heap_size);
    tr_debug("\t mbed_stack_isr_start: %p, mbed_stack_isr_end: %p, size: %" PRIu32 "",
             mbed_stack_isr_start,
             (mbed_stack_isr_start + mbed_stack_isr_size),
             mbed_stack_isr_size);
}

#endif  // MBED_ALL_STATS_ENABLED

}  // namespace advembsof
