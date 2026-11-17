#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1

#define configCPU_CLOCK_HZ                      (SystemCoreClock)
#define configTICK_RATE_HZ                      1000

#define configMAX_PRIORITIES                    4
#define configMINIMAL_STACK_SIZE                128

#define configTOTAL_HEAP_SIZE                   (2048)

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

#define configKERNEL_INTERRUPT_PRIORITY \
    (15 << (8 - configPRIO_BITS))

#define configMAX_SYSCALL_INTERRUPT_PRIORITY \
    (5 << (8 - configPRIO_BITS))

#define vPortSVCHandler       SVC_Handler
#define xPortPendSVHandler    PendSV_Handler
#define xPortSysTickHandler   SysTick_Handler