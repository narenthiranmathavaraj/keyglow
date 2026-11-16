# KeyGlow: firmware for a simulated STM32F100 (Cortex-M3) on QEMU
CPU     ?= cortex-m3
BOARD   ?= stm32vldiscovery
PROJECT ?= src/startup

build:
	arm-none-eabi-as -mcpu=$(CPU) -g -c src/startup.S -o src/startup.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding -c src/main.c -o src/main.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding -c src/ringbuf.c -o src/ringbuf.o
	arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -g -O0 -ffreestanding -c src/uart.c -o src/uart.o
		arm-none-eabi-gcc -mcpu=$(CPU) -mthumb -nostdlib \
		src/startup.o src/main.o src/ringbuf.o src/uart.o \
		-T src/linker.ld -o $(PROJECT).elf
	arm-none-eabi-objdump -D -S $(PROJECT).elf > $(PROJECT).elf.lst
	arm-none-eabi-readelf -a $(PROJECT).elf > $(PROJECT).elf.debug

qemu: build
	qemu-system-arm -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJECT).elf -gdb tcp::1234

gdb:
	gdb-multiarch -q $(PROJECT).elf \
	    -ex "target remote localhost:1234"

clean_outputs:
	rm -rf src/*.out src/*.elf src/*.gdb_history src/*.lst src/*.debug src/*.o src/*.bin

run: build
	qemu-system-arm -M $(BOARD) -cpu $(CPU) -nographic -kernel $(PROJECT).elf


run_terminal: build
	qemu-system-arm -M $(BOARD) -cpu $(CPU) -nographic\
	  -kernel $(PROJECT).elf \
	  -gdb tcp::1234 \
	  -monitor none \
	  -serial stdio\
	  -serial unix:/tmp/uart1.sock,server,nowait


socket:
	socat -,raw,echo=0 unix-connect:/tmp/uart1.sock





CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Isrc -pthread

SRC = src/ringbuf.c
TEST = tests/unit/test_ringbuf.c

.PHONY: unit clean

unit:
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o test_ringbuf
	./test_ringbuf

clean:
	rm -f test_ringbuf















# CPU      ?= cortex-m3
# BOARD    ?= stm32vldiscovery
# NAME     ?= keyglow
# BUILD    ?= build
# PREFIX   ?= arm-none-eabi-
# LDSCRIPT ?= linker.ld

# FREERTOS ?= third_party/FreeRTOS-Kernel
# UNITY    ?= third_party/Unity
# # Host-compiled sources used by `make unit` (pure logic only, no hardware registers), e.g.:
# #   make unit HOST_SRCS="src/ringbuf.c src/crc8.c"
# HOST_SRCS ?=

# CC      := $(PREFIX)gcc
# OBJCOPY := $(PREFIX)objcopy
# SIZE    := $(PREFIX)size
# QEMU    ?= qemu-system-arm
# GDB     ?= gdb-multiarch

# APP_SRCS := $(wildcard src/*.c)
# ASM_SRCS := $(wildcard src/*.s)

# ifneq ($(wildcard $(FREERTOS)/tasks.c),)
# RTOS_SRCS := $(addprefix $(FREERTOS)/,list.c queue.c tasks.c timers.c) \
#              $(FREERTOS)/portable/GCC/ARM_CM3/port.c \
#              $(FREERTOS)/portable/MemMang/heap_4.c
# RTOS_INC  := -I$(FREERTOS)/include -I$(FREERTOS)/portable/GCC/ARM_CM3 -Iconfig
# endif

# CFLAGS  := -mcpu=$(CPU) -mthumb -std=c11 -Og -g3 -Wall -Wextra -Wshadow \
#            -ffunction-sections -fdata-sections -fno-common -MMD -MP -Isrc $(RTOS_INC)
# ASFLAGS := -mcpu=$(CPU) -mthumb -g
# LDFLAGS := -mcpu=$(CPU) -mthumb -nostartfiles --specs=nano.specs --specs=nosys.specs \
#            -T$(LDSCRIPT) -Wl,--gc-sections -Wl,-Map=$(BUILD)/$(NAME).map

# OBJS := $(addprefix $(BUILD)/,$(APP_SRCS:.c=.o) $(ASM_SRCS:.s=.o) $(RTOS_SRCS:.c=.o))
# ELF  := $(BUILD)/$(NAME).elf
# BIN  := $(BUILD)/$(NAME).bin

# # QEMU serial ports are assigned in order: 1st = USART1 (keyboard in, log out),
# # 2nd = USART2 (display link, connect with: nc localhost 5555), 3rd = USART3 (unused)
# SERIAL    ?= -serial stdio -serial tcp::5555,server,nowait -serial null
# QEMU_BASE  = -M $(BOARD) -cpu $(CPU) -display none -kernel $(ELF)

# UNIT_TESTS := $(wildcard tests/unit/test_*.c)

# .PHONY: all size run debug gdb unit sim test doctor clean help

# all: $(ELF) $(BIN) size

# $(ELF): $(OBJS) $(LDSCRIPT)
# 	@test -n "$(OBJS)" || { echo "error: no sources in src/ yet"; exit 1; }
# 	$(CC) $(OBJS) $(LDFLAGS) -o $@

# $(BIN): $(ELF)
# 	$(OBJCOPY) -O binary $< $@

# $(BUILD)/%.o: %.c
# 	@mkdir -p $(dir $@)
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(BUILD)/%.o: %.s
# 	@mkdir -p $(dir $@)
# 	$(CC) $(ASFLAGS) -c $< -o $@

# -include $(OBJS:.o=.d)

# size: $(ELF)
# 	@$(SIZE) $(ELF)

# run: all
# 	$(QEMU) $(QEMU_BASE) $(SERIAL)

# debug: all
# 	@echo "QEMU paused, waiting for GDB on :1234. In another terminal: make gdb (or press F5 in VS Code)"
# 	$(QEMU) $(QEMU_BASE) $(SERIAL) -S -gdb tcp::1234

# gdb:
# 	$(GDB) -q $(ELF) -ex "target remote :1234"

# unit:
# 	@if [ -z "$(UNIT_TESTS)" ]; then echo "unit: no tests/unit/test_*.c yet"; exit 1; fi
# 	@test -f $(UNITY)/src/unity.c || { echo "unit: Unity missing, run scripts/bootstrap.sh"; exit 1; }
# 	@mkdir -p $(BUILD)/host
# 	@for t in $(UNIT_TESTS); do \
# 	  n=$$(basename $$t .c); \
# 	  gcc -std=c11 -Wall -Wextra -g -fsanitize=address,undefined -I$(UNITY)/src -Isrc \
# 	      $$t $(UNITY)/src/unity.c $(HOST_SRCS) -o $(BUILD)/host/$$n || exit 1; \
# 	  ./$(BUILD)/host/$$n || exit 1; \
# 	done

# sim: all
# 	@test -f tests/sim/run_sim.py || { echo "sim: tests/sim/run_sim.py missing"; exit 1; }
# 	python3 tests/sim/run_sim.py

# test: all unit sim
# 	@echo "RESULT: PASS"

# doctor:
# 	@echo "== toolchain =="; $(CC) --version | head -1; $(QEMU) --version | head -1; $(GDB) --version | head -1; python3 --version
# 	@$(QEMU) -M help | grep -q "^$(BOARD) " && echo "QEMU machine $(BOARD): ok" || { echo "QEMU machine $(BOARD): MISSING"; exit 1; }
# 	@test -f $(FREERTOS)/tasks.c && echo "FreeRTOS: ok" || echo "FreeRTOS: missing (run scripts/bootstrap.sh)"
# 	@test -f $(UNITY)/src/unity.c && echo "Unity: ok" || echo "Unity: missing (run scripts/bootstrap.sh)"

# clean:
# 	rm -rf $(BUILD)

# help:
# 	@echo "targets: all run debug gdb unit sim test doctor clean"
# 	@echo "vars:    CPU=$(CPU) BOARD=$(BOARD) NAME=$(NAME) HOST_SRCS=<host-testable .c files>"
