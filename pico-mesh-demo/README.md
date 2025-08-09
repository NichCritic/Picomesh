# Pico W Bluetooth Mesh Demo (BTstack)

This is a minimal Bluetooth Mesh Generic OnOff Server for Raspberry Pi Pico W using the Pico SDK's BTstack integration. When provisioned into a mesh network, it toggles the onboard LED with Generic OnOff messages.

## Prerequisites
- Pico SDK installed and `PICO_SDK_PATH` exported
- Toolchain (cmake, gcc-arm-none-eabi)
- Pico W hardware

## Clone
```
cd /workspace
# Ensure pico-sdk is available and PICO_SDK_PATH is set in your environment
```

## Build
```
cd /workspace/pico-mesh-demo
mkdir -p build && cd build
cmake ..
make -j
```

The build produces `pico_mesh_demo.uf2` in `build/`.

## Flash
- Hold BOOTSEL on the Pico W, plug into USB
- Copy `pico_mesh_demo.uf2` onto the RPI-RP2 drive

## Provisioning
- The node advertises unprovisioned over PB-GATT (Proxy enabled)
- Use a Bluetooth Mesh provisioner app (e.g. Nordic nRF Mesh) to provision
  - After provisioning, bind Generic OnOff Server to an AppKey and assign a subscription/publish address
  - Send Generic OnOff Set to toggle the LED

## Notes
- `include/btstack_config.h` enables Mesh and sets minimal buffer sizes
- The code uses `pico_cyw43_arch_none` (BLE only). If you also use Wi‑Fi, switch to `pico_cyw43_arch_threadsafe_background` in CMake and initialize Wi‑Fi as needed
- Enable verbose logging by adding `WANT_HCI_DUMP=1` to `target_compile_definitions`