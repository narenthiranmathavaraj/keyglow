#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1

#define configCPU_CLOCK_HZ                      24000000
// #define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configTICK_RATE_HZ                      1000
#define configTICK_TYPE_WIDTH_IN_BITS    TICK_TYPE_WIDTH_32_BITS

#define configMAX_PRIORITIES                    4
#define configMINIMAL_STACK_SIZE                128 //words

#define configTOTAL_HEAP_SIZE                   (4096)

#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0

#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     0

#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               2
#define configTIMER_QUEUE_LENGTH                5
#define configTIMER_TASK_STACK_DEPTH            128

#define configCHECK_FOR_STACK_OVERFLOW          2

#define configPRIO_BITS                         4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
// Interrupts with a higher priority (numerically lower than 5, such as 0 to 4)
//  cannot call FreeRTOS API functions. These high-priority interrupts are never masked or delayed
// by FreeRTOS critical sections, ensuring ultra-low latency for time-critical hardware events.


#define configKERNEL_INTERRUPT_PRIORITY \
    (15 << (8 - configPRIO_BITS))

#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (5 << (8 - configPRIO_BITS))

#define vPortSVCHandler       SVC_Handler
#define xPortPendSVHandler    PendSV_Handler
#define xPortSysTickHandler   SysTick_Handler

#define configUSE_MUTEXES                 1
#define configUSE_RECURSIVE_MUTEXES       0
#define configUSE_COUNTING_SEMAPHORES     0

#define configUSE_QUEUE_SETS               0
#define configUSE_TASK_NOTIFICATIONS       1

#define configUSE_APPLICATION_TASK_TAG     0
#define configUSE_CO_ROUTINES              0

#define configUSE_TRACE_FACILITY           0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

#define configUSE_MALLOC_FAILED_HOOK       0

#define INCLUDE_vTaskDelay                 1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelete                1
#define INCLUDE_vTaskSuspend               1
#define INCLUDE_xTaskGetSchedulerState     1