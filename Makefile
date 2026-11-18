
# make -s <cmd> s->suppress make -s build_rtos

# KeyGlow: firmware for a simulated STM32F100 (Cortex-M3) on QEMU
CPU     ?= cortex-m3
BOARD   ?= stm32vldiscovery
PROJECT_BAREMETAL ?= src/baremetal/startup
INCLUDES_BAREMETAL = -I src/common -I src/drivers
build_baremetal:
	arm-none-eabi-as -mcpu=$(CPU) -g -c src/startup_baremetal.S -o src/startup.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_BAREMETAL) -c src/baremetal/main.c -o src/baremetal/main.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_BAREMETAL) -c src/common/ringbuf.c -o src/baremetal/ringbuf.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_BAREMETAL) -c src/drivers/uart.c -o src/baremetal/uart.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -nostdlib src/startup.o src/baremetal/main.o src/baremetal/ringbuf.o src/baremetal/uart.o \
	-T src/linker_baremetal.ld -o $(PROJECT_BAREMETAL).elf
	arm-none-eabi-objdump -D -S $(PROJECT_BAREMETAL).elf > $(PROJECT_BAREMETAL).elf.lst
	arm-none-eabi-readelf -a $(PROJECT_BAREMETAL).elf > $(PROJECT_BAREMETAL).elf.debug

qemu_baremetal: build_baremetal
# 	qemu-system-arm -M $(BOARD) -cpu $(CPU) -no/graphic -kernel $(PROJECT_BAREMETAL).elf -gdb tcp::1234
	qemu-system-arm -S -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJECT_BAREMETAL).elf -gdb tcp::1234

gdb_baremetal:
	gdb-multiarch -q $(PROJECT_BAREMETAL).elf \
	    -ex "target remote localhost:1234"

clean_baremetal:
	rm -rf src/*.out src/*.elf src/*.gdb_history src/*.lst src/*.debug src/*.o src/*.bin \
	src/baremetal/*.out src/baremetal/*.elf src/baremetal/*.gdb_history src/baremetal/*.lst \
	src/baremetal/*.elf.debug src/baremetal/*.o src/baremetal/*.bin

socket_baremetal:
	socat -,raw,echo=0 unix-connect:/tmp/uart1.sock


PROJECT_RTOS ?= src/rtos/startup
FREERTOS_DIR = third_party/FreeRTOS-Kernel
FREERTOS_SRC = \
    $(FREERTOS_DIR)/tasks.c \
    $(FREERTOS_DIR)/queue.c \
    $(FREERTOS_DIR)/list.c \
    $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c \
    $(FREERTOS_DIR)/portable/MemMang/heap_4.c
INCLUDES_RTOS = -Isrc/common \
                -Isrc/drivers \
                -Iconfig \
                -I$(FREERTOS_DIR)/include \
                -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3
build_rtos:
	arm-none-eabi-as -mcpu=$(CPU) -g -c src/startup_rtos.S -o src/startup.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c src/rtos/main.c -o src/rtos/main.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c src/common/ringbuf.c -o src/rtos/ringbuf.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c src/drivers/uart.c -o src/rtos/uart.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c $(FREERTOS_DIR)/tasks.c -o src/rtos/tasks.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c $(FREERTOS_DIR)/queue.c -o src/rtos/queue.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c $(FREERTOS_DIR)/list.c -o src/rtos/list.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c $(FREERTOS_DIR)/timers.c -o src/rtos/timers.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c $(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c -o src/rtos/port.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding $(INCLUDES_RTOS) -c $(FREERTOS_DIR)/portable/MemMang/heap_4.c -o src/rtos/heap_4.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -nostartfiles --specs=nosys.specs \
    src/startup.o src/rtos/main.o src/rtos/ringbuf.o src/rtos/uart.o \
    src/rtos/tasks.o src/rtos/queue.o src/rtos/list.o src/rtos/timers.o src/rtos/port.o src/rtos/heap_4.o \
    -T src/linker_rtos.ld -o $(PROJECT_RTOS).elf
	arm-none-eabi-objdump -D -S $(PROJECT_RTOS).elf > $(PROJECT_RTOS).elf.lst
	arm-none-eabi-readelf -a $(PROJECT_RTOS).elf > $(PROJECT_RTOS).elf.debug

qemu_rtos: build_rtos
	qemu-system-arm -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJECT_RTOS).elf -gdb tcp::1234
# 	qemu-system-arm -S -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJECT_RTOS).elf -gdb tcp::1234

gdb_rtos:
	gdb-multiarch -q $(PROJECT_RTOS).elf \
	    -ex "target remote localhost:1234"

clean_rtos:
	rm -rf src/*.out src/*.elf src/*.gdb_history src/*.lst src/*.debug src/*.o src/*.bin \
	src/rtos/*.out src/rtos/*.elf src/rtos/*.gdb_history src/rtos/*.lst \
	src/rtos/*.elf.debug src/rtos/*.o src/rtos/*.bin

socket_rtos:
	socat -,raw,echo=0 unix-connect:/tmp/uart1.sock

size_rtos:
	arm-none-eabi-size -B -d $(PROJECT_RTOS).elf





CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Isrc -pthread

SRC = src/ringbuf.c
TEST = tests/unit/test_ringbuf.c

.PHONY: unit clean

unit_test:
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o test_ringbuf
	./test_ringbuf

clean_test:
	rm -f test_ringbuf




# help:
# 	@echo "targets: all run debug gdb unit sim test doctor clean"
# 	@echo "vars:    CPU=$(CPU) BOARD=$(BOARD) NAME=$(NAME) HOST_SRCS=<host-testable .c files>"


# .PHONY: all size run debug gdb unit sim test doctor clean help

# size: $(ELF)
# 	@$(SIZE) $(ELF)


# sim: all
# 	@test -f tests/sim/run_sim.py || { echo "sim: tests/sim/run_sim.py missing"; exit 1; }
# 	python3 tests/sim/run_sim.py

doctor:
	@echo "== toolchain =="; $(CC) --version | head -1; $(QEMU) --version | head -1; $(GDB) --version | head -1; python3 --version
	@$(QEMU) -M help | grep -q "^$(BOARD) " && echo "QEMU machine $(BOARD): ok" || { echo "QEMU machine $(BOARD): MISSING"; exit 1; }
	@test -f $(FREERTOS)/tasks.c && echo "FreeRTOS: ok" || echo "FreeRTOS: missing (run scripts/bootstrap.sh)"
	@test -f $(UNITY)/src/unity.c && echo "Unity: ok" || echo "Unity: missing (run scripts/bootstrap.sh)"
