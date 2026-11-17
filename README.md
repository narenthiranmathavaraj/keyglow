# KeyGlow

Power-saving keyboard backlight controller for a simulated STM32F100 (Cortex-M3), running on QEMU.
Type on the keyboard: a UART interrupt wakes the backlight, keystrokes flow through a ring buffer and
FreeRTOS tasks to a "display" over a second UART, and the backlight turns off after 5 s of inactivity.

[![ci](https://github.com/<your-user>/<repo>/actions/workflows/ci.yml/badge.svg)](https://github.com/<your-user>/<repo>/actions)
[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/<your-user>/<repo>)

## Try it (no install)
1. Click **Open in GitHub Codespaces** above and wait for the container to build.
2. Run `make test`. It builds the firmware, runs the Unity unit tests, runs the QEMU simulation test, and ends with `RESULT: PASS`.

## Run it yourself
| Command | What it does |
|---|---|
| `make` | build the ELF and BIN |
| `make run` | run on QEMU. Terminal = keyboard on USART1. Display on USART2: `nc localhost 5555` in a second terminal |
| `make debug` then `make gdb` | QEMU paused, GDB attached on :1234 (or press F5 in VS Code) |
| `make unit` | host-side Unity tests with ASan and UBSan |
| `make sim` | automated QEMU test (Python) |
| `make test` | build + unit + sim |
| `make doctor` | print tool versions and check dependencies |

## Architecture
_TODO: add diagram and a short description of the tasks, queues, semaphore and mutex._

## Design notes
_TODO: startup and linker script, ISR design, ring buffer, backlight state machine, semaphore vs mutex, GDB experiments._


```
The code provided as part of this repository is ONLY for learning purposes!
```

# Resources

Refer to the following resource when progressing through the course:
1. [Cortex-M for Beginners - 2017_EN_v2.pdf](https://community.arm.com/cfs-file/__key/telligent-evolution-components-attachments/01-2057-00-00-00-01-28-35/Cortex_2D00_M-for-Beginners-_2D00_-2017_5F00_EN_5F00_v2.pdf)
2. [Arm_Cortex-M_Processor_Comparison_Table.pdf](https://documentation-service.arm.com/static/655e085f2c8b3557fee7048f?token=)
3. [ARM Architecture Reference Manual Thumb-2 Supplement](https://documentation-service.arm.com/static/661d03b55d66282bc2cf7868?token=)

## Documentation
1. [gnu as](https://ftp.gnu.org/old-gnu/Manuals/gas/html_chapter/as_toc.html)
1. [Cortex-M3 Instructions](https://developer.arm.com/documentation/ddi0337/h/programmers-model/instruction-set-summary/cortex-m3-instructions)

### STM32VL Discovery
1. [STM32F100x reference manual](https://www.st.com/resource/en/reference_manual/cd00246267-stm32f100xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
1. [STM32F100x data sheet](https://www.st.com/resource/en/datasheet/stm32f100cb.pdf)

### Cortex-M3 Documentation
1. [Cortex-M3 Devices Generic User Guide](https://developer.arm.com/documentation/dui0552/latest/)

# Copyright
Copyright © 2025 pyjamabrah.com. All Rights Reserved.
https://dev.to/ripan030/linker-scripts-explained-controlling-memory-layout-on-bare-metal-3ocb


Bare-Metal UART Communication with Interrupts and SysTick

This project implements a bare-metal UART communication system using C and ARM assembly (.S and .C) on a Cortex-M microcontroller, without relying on an RTOS or HAL.

It uses USART1 RX interrupts to receive incoming characters asynchronously and stores them in a circular ring buffer. The SysTick timer provides a 1 ms system time base, allowing the main timing logic to process and echo received characters at a controlled rate of 3 letters per second (3 LPS).

The project explores fundamental embedded systems concepts, including:

Bare-metal programming: Direct memory-mapped register access and hardware configuration.

ARM Assembly: Startup code, vector table, and interrupt handler setup.

UART Interrupts: Non-blocking reception using USART1 RXNE interrupts.

Ring Buffer: Decoupling asynchronous UART reception from character processing.

SysTick: Hardware timer-based timekeeping and periodic scheduling.

Interrupt Handling: Managing peripheral interrupts and the Cortex-M NVIC.

The objective is to understand how embedded firmware handles asynchronous communication, hardware interrupts, buffering, and time-dependent operations at the register level, without an RTOS.