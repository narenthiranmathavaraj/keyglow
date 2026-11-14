#systick configuration registers
.equ SYST_CSR, 0xE000E010
.equ SYST_RVR, 0xE000E014
.equ SYST_CVR, 0xE000E018
.equ SYST_CALIB, 0xE000E01C
.equ TIMEOUT, 0x00ffffff

.section .vectors
vector_table:
    .word 0x20001000
    .word reset_handler
.org 0x3C

    .word systick_handler
    .zero 400

    .section .text //liking calling a function
    .align 1
    .type reset_handler, %function


reset_handler:
    MOV	    r5, #0
    LDR	    r0, =SYST_CSR
    LDR	    r1, =SYST_RVR
    LDR	    r2, =SYST_CVR
    LDR	    r3, =TIMEOUT

    STR	    r3, [r1]
    MOV	    r3, #0x0
    STR	    r3, [r2]
    MOV	    r3, #0x7
    STR	    r3, [r0]
    b .


    .section .text //liking calling a function
    .align 1
    .type systick_handler, %function

systick_handler:
    add r5, r5, #1
break_here:
    bx lr

