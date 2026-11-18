# Tiny Chip, Two Brains: Bare Metal and RTOS

Two embedded projects in one repo. Both run on a simulated STM32F100 (Cortex-M3) in QEMU. No hardware needed.

1. **baremetal**: UART echo using interrupts and SysTick. No RTOS, no HAL.
2. **rtos**: Speed typing test and motivation chatbot on FreeRTOS.

## Try it in the browser (GitHub Codespaces)

1. Click the badge and wait for the container to build.

   [![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/<your-user>/<repo>)

2. Open a terminal in the Codespace: **Terminal > New Terminal** (or Ctrl+`).

## Commands

**Step 1. Check the setup**

```bash
make doctor
```

Prints tool versions and tells you if something is missing.

**Step 2. See all commands**

```bash
make help
```

Lists the commands to build, run and stop each project.

**Step 3. Run a project**

```bash
make -s qemu_baremetal
```
```bash
make -s qemu_rtos
```

**Step 4. Stop a project**

Press `Ctrl+A`, release, then press `X`.

## What you will see

### Baremetal

- Type characters in the terminal.
- They are echoed back slowly, at 3 letters per second.
- The speed stays fixed even if you type fast, because the extra characters wait in a ring buffer.

### RTOS

- Typing test: shows your words per minute (WPM) and accuracy.
- Chatbot: prints a random motivational quote.
- Both run as separate FreeRTOS tasks on one CPU.

---

## Project 1: Bare-metal UART with interrupts and SysTick

A UART echo program written in Embedded C and ARM Assembly. It does not use an RTOS or a HAL. All hardware is controlled by writing to memory-mapped registers.

How it works:

- USART1 RX interrupts receive characters without blocking.
- Received characters go into a circular ring buffer.
- The SysTick timer gives a 1 ms time base.
- The main loop takes one character from the buffer and echoes it at 3 letters per second.

### Features

- Bare-metal programming: direct register access and peripheral setup.
- ARM Assembly: startup code, interrupt vector table, interrupt handler setup.
- UART interrupts: non-blocking receive using the RXNE interrupt.
- Ring buffer: separates the interrupt (producer) from the main loop (consumer).
- SysTick: 1 ms timekeeping, no blocking delays.
- NVIC: enables and manages the peripheral interrupt.

### What it demonstrates

| Technique | Where it is used |
|---|---|
| Bare-metal programming | Hardware control without an OS or HAL |
| Memory-mapped registers | Peripheral configuration |
| ARM Assembly startup | Processor setup before C code runs |
| Interrupt vector table | Maps interrupts to handlers |
| UART RX interrupts | Asynchronous serial input |
| Circular ring buffer | Producer-consumer data handling |
| SysTick timer | Periodic timekeeping |
| NVIC | Interrupt enable and priority |
| Non-blocking reception | No continuous UART polling |

### Tech stack

Embedded C, ARM Thumb Assembly, ARM Cortex-M, USART1, SysTick, NVIC, ring buffer, GNU ARM Embedded Toolchain.

### What I learned

- How to configure peripherals through memory-mapped registers.
- How startup code prepares the processor before C code runs.
- How the vector table connects interrupts to handlers.
- How UART interrupts receive data without polling.
- How a ring buffer separates interrupt code from application code.
- How SysTick makes a 1 ms time base for non-blocking timing.

---

## Project 2: RTOS Speed Typing Chatbot

A UART application built on FreeRTOS, ARM Cortex-M3, Embedded C and Assembly. It shows how an RTOS runs several activities at once, handles interrupts, and shares the CPU between tasks.

It has two features:

1. **Speed typing test**: measures WPM and character accuracy.
2. **Motivation chatbot**: prints random quotes about FreeRTOS ideas.

### Features

Speed typing test:

- UART typing interface.
- Measures WPM.
- Calculates character accuracy.
- Interrupt-driven UART receive.
- Characters pass through a ring buffer.
- A binary semaphore tells the task when the UART interrupt has data.

Chatbot:

- Picks a random quote.
- Uses task notifications to wake the task when an event happens.
- The task stays blocked until the event arrives.
- Uses RTOS delays while sending over UART.

### What it demonstrates

| Technique | Where it is used |
|---|---|
| Priority-based scheduling | Important tasks run first |
| Interrupt-driven UART | Asynchronous serial communication |
| Circular ring buffer | Producer-consumer data handling |
| Binary semaphore | Interrupt-to-task signal |
| Task notifications | Light event signal between tasks |
| Task blocking | CPU is free while a task waits |
| Context switching | Independent tasks run in turns |
| Stack overflow hook | Detects stack memory problems |
| WFI instruction | Low-power wait |
| Assembly startup | Processor and runtime setup |
| Linker script | Memory layout set by hand |

### Tech stack

Embedded C, FreeRTOS, ARM Cortex-M3, ARM Thumb Assembly, USART1 (QEMU), binary semaphore, task notifications, ring buffer, QEMU, GDB, GNU ARM Embedded Toolchain, custom linker script.

### What I learned

- How the scheduler runs many tasks.
- How an interrupt hands data to a task.
- How semaphores and task notifications differ.
- How task priority changes execution order.
- How an interrupt can request a context switch.
- How Assembly and the linker script set up the memory and startup before the RTOS runs.

---

## Simulation and debugging

Both projects were built and run in QEMU (Cortex-M3). GDB was used to debug.

Debug commands: `make gdb_baremetal` and `make gdb_rtos`.

**Note:** Tested in QEMU only. Not tested on a real board.

---

## References

Course material:

1. [Cortex-M for Beginners - 2017_EN_v2.pdf](https://community.arm.com/cfs-file/__key/telligent-evolution-components-attachments/01-2057-00-00-00-01-28-35/Cortex_2D00_M-for-Beginners-_2D00_-2017_5F00_EN_5F00_v2.pdf)
2. [Arm_Cortex-M_Processor_Comparison_Table.pdf](https://documentation-service.arm.com/static/655e085f2c8b3557fee7048f?token=)
3. [ARM Architecture Reference Manual Thumb-2 Supplement](https://documentation-service.arm.com/static/661d03b55d66282bc2cf7868?token=)

Documentation:

1. [gnu as](https://ftp.gnu.org/old-gnu/Manuals/gas/html_chapter/as_toc.html)
2. [Cortex-M3 Instructions](https://developer.arm.com/documentation/ddi0337/h/programmers-model/instruction-set-summary/cortex-m3-instructions)

STM32VL Discovery:

1. [STM32F100x reference manual](https://www.st.com/resource/en/reference_manual/cd00246267-stm32f100xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
2. [STM32F100x data sheet](https://www.st.com/resource/en/datasheet/stm32f100cb.pdf)

Cortex-M3:

1. [Cortex-M3 Devices Generic User Guide](https://developer.arm.com/documentation/dui0552/latest/)