.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.extern USART1_IRQHandler

/* =====================================================
 * 1. VECTOR TABLE
 * ===================================================== */
/*C can configure and enable interrupts and implement their handlers.
The startup vector table is still needed to tell the processor where those handlers are located.*/
.section .vectors, "a"
.align 8

vector_table:

    /* Initial stack pointer and reset handler */
    .word _estack
    .word Reset_Handler

    /* Cortex-M3 exceptions */
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler

    /* Reserved vectors */
    .word 0
    .word 0
    .word 0
    .word 0

    /* FreeRTOS exception handlers */
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

    .org 0xD4
    .word USART1_IRQHandler
    /* External IRQ 37 = USART1 */
/* =====================================================
 * 2. RESET HANDLER
 * ===================================================== */

.section .text
.thumb_func

Reset_Handler:

    /* Initialize stack pointer */
    ldr sp, =_estack

    /* -----------------------------------------------
     * Copy .data from Flash to RAM
     * ----------------------------------------------- */

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data:
    cmp r1, r2
    bcc copy_word
    b zero_bss

copy_word:
    ldr r3, [r0], #4
    str r3, [r1], #4
    b copy_data

    /* -----------------------------------------------
     * Zero initialize .bss
     * ----------------------------------------------- */

zero_bss:
    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0

zero_loop:
    cmp r1, r2
    bcc zero_word
    b call_main

zero_word:
    str r3, [r1], #4
    b zero_loop

    /* -----------------------------------------------
     * Call main()
     * ----------------------------------------------- */

call_main:
    bl main

    /* main should never return */

hang:
    b hang


/* =====================================================
 * 3. DEFAULT EXCEPTION HANDLER
 * ===================================================== */

.section .text.Default_Handler, "ax"
.thumb_func

Default_Handler:
    b Default_Handler


/* =====================================================
 * 4. WEAK DEFAULT HANDLER ALIASES (overeverything by default in loop so no crashing)

    qemu: fatal: Lockup: can't escalate 3 to HardFault (current priority -1)
    R00=20002000 R01=00000001 R02=00000000 R03=20000000
    R04=00000000 R05=00000000 R06=00000000 R07=20001fc8
    R08=00000000 R09=00000000 R10=00000000 R11=00000000
    R12=00000000 R13=20001fc0 R14=fffffff1 R15=490d480c
    XPSR=60000003 -ZC- A handler
    FPSCR:

make: *** [Makefile:66: qemu_rtos] Aborted (core dumped)
 * ===================================================== */

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler
