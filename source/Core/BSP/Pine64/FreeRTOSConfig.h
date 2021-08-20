#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#include "nuclei_sdk_soc.h"
#include <stdint.h>
#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ((uint32_t)SystemCoreClock)
#define configRTC_CLOCK_HZ                      ((uint32_t)32768)
#define configTICK_RATE_HZ                      ((TickType_t)1000)
#define configMAX_PRIORITIES                    (4)
#define configMINIMAL_STACK_SIZE                ((unsigned short)128)
#define configMAX_TASK_NAME_LEN                 24
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configQUEUE_REGISTRY_SIZE               10
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0

#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_vTaskDelay                  1
/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION  1
#define configSUPPORT_DYNAMIC_ALLOCATION 0
#define configTOTAL_HEAP_SIZE            1024
#define configAPPLICATION_ALLOCATED_HEAP 0

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                1
#define configUSE_TICK_HOOK                0
#define configCHECK_FOR_STACK_OVERFLOW     1
#define configUSE_MALLOC_FAILED_HOOK       0
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS        0
#define configUSE_TRACE_FACILITY             0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES 1

/* Software timer related definitions. */
#define configUSE_TIMERS             0
#define configTIMER_TASK_PRIORITY    3
#define configTIMER_QUEUE_LENGTH     5
#define configTIMER_TASK_STACK_DEPTH configMINIMAL_STACK_SIZE

/* Interrupt nesting behaviour configuration. */
#define configPRIO_BITS                              (4UL)
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      0x1
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 0xe
#define configKERNEL_INTERRUPT_PRIORITY              (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY         (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* Define to trap errors during development. */
#define configASSERT(x)       \
  if ((x) == 0) {             \
    taskDISABLE_INTERRUPTS(); \
    for (;;)                  \
      ;                       \
  }

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_xResumeFromISR              1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_xTaskGetCurrentTaskHandle   1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_xTaskGetIdleTaskHandle      1
#define INCLUDE_eTaskGetState               0
#define INCLUDE_xEventGroupSetBitFromISR    1
#define INCLUDE_xTimerPendFunctionCall      0
#define INCLUDE_xTaskAbortDelay             0
#define INCLUDE_xTaskGetHandle              1
#define INCLUDE_xTaskResumeFromISR          1
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* FREERTOS_CONFIG_H */
