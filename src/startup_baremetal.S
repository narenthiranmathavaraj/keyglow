.syntax unified
.cpu cortex-m3
.thumb
// #systick configuration registers
.equ SYST_CSR, 0xE000E010
.equ SYST_RVR, 0xE000E014
.equ SYST_CVR, 0xE000E018
.equ SYST_CALIB, 0xE000E01C
.equ TIMEOUT, 23999


.global RESET_Handler
.extern USART1_IRQHandler
.extern SYSTICK_Handler

.section .vectors, "a"
.align 8
/*C can configure and enable interrupts and implement their handlers.
The startup vector table is still needed to tell the processor where those handlers are located.*/
vector_table:

    /* Initial stack pointer and reset handler */
    .word _estack
    .word RESET_Handler

    .org 0x3C

    .word SYSTICK_Handler

    .org 0xD4
    .word USART1_IRQHandler
    /* External IRQ 37 = USART1 */


.section .text
.thumb_func

RESET_Handler:
    /* 1. Initialize systick timer*/

    LDR	    r0, =SYST_CSR
    LDR	    r1, =SYST_RVR
    LDR	    r2, =SYST_CVR
    LDR	    r3, =TIMEOUT
    STR	    r3, [r1]
    MOV	    r3, #0x0
    STR	    r3, [r2]
    MOV	    r3, #0x7 /*to make last three bits one*/
    STR	    r3, [r0]

    /* 1. Initialize stack pointer */
    ldr sp, =_estack

    /* 2. Copy .data from Flash to RAM */

    ldr r0, =_sidata      /* Source: Flash */
    ldr r1, =_sdata       /* Destination: RAM */
    ldr r2, =_edata       /* End of .data */

    copy_data:
        cmp r1, r2
        bcc copy_word
        b zero_bss

copy_word:

    ldr r3, [r0], #4
    str r3, [r1], #4

    b copy_data
 /* 3. Zero initialize .bss */

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


    /* 4. Call main() */

call_main:

    bl main


    /* main should not return */

hang:

    b hang


