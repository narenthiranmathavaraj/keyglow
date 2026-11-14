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
