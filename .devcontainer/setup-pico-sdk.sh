#!/usr/bin/env bash
set -euo pipefail

# Run from workspace root (default for postCreateCommand)
if [ ! -d "pico-sdk" ]; then
  echo "Cloning pico-sdk..."
  git clone --depth 1 https://github.com/raspberrypi/pico-sdk.git pico-sdk
  git -C pico-sdk submodule update --init --recursive
else
  echo "pico-sdk already present; skipping clone"
fi

# Show tool versions to verify environment
set +e
arm-none-eabi-gcc --version | head -n1
cmake --version | head -n1
ninja --version
python3 --version
set -e