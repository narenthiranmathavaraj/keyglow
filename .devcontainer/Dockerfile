FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04
ENV DEBIAN_FRONTEND=noninteractive
# Ubuntu 24.04: arm-none-eabi-gcc 13.2.1, QEMU 8.2 (has the stm32vldiscovery machine), gdb-multiarch 15
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential \
      gcc-arm-none-eabi libnewlib-arm-none-eabi \
      qemu-system-arm gdb-multiarch \
      make git ca-certificates \
      python3 python3-pip python3-pexpect \
      netcat-openbsd \
      cppcheck clang-format \
    && rm -rf /var/lib/apt/lists/*
