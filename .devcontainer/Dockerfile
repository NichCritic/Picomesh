# syntax=docker/dockerfile:1
ARG VARIANT=debian-12
FROM mcr.microsoft.com/devcontainers/base:${VARIANT}

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       ca-certificates \
       git \
       build-essential \
       cmake \
       ninja-build \
       pkg-config \
       python3 \
       python3-pip \
       python3-venv \
       gcc-arm-none-eabi \
       g++-arm-none-eabi \
       binutils-arm-none-eabi \
       gdb-multiarch \
       openocd \
       libnewlib-arm-none-eabi \
       libstdc++-arm-none-eabi-newlib \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install --no-cache-dir --upgrade pip