cmake_minimum_required(VERSION 3.13)

# Set board to Pico W (CYW43)
set(PICO_BOARD pico_w)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pico_mesh_demo C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_mesh_demo
    src/main.c
)

target_include_directories(pico_mesh_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Generate GATT header for Mesh Provisioning + Proxy services
pico_btstack_make_gatt_header(pico_mesh_demo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/mesh_proxy.gatt ${PICO_SDK_PATH}/lib/btstack/src/mesh/gatt-service)

# Link against BTstack BLE, Mesh, and CYW43 driver
# For BLE/mesh only, use pico_cyw43_arch_none
# For Wi‑Fi coexistence, consider pico_cyw43_arch_threadsafe_background
 target_link_libraries(pico_mesh_demo
    pico_stdlib
    pico_btstack_ble
    pico_btstack_mesh
    pico_btstack_cyw43
    pico_btstack_hci_transport_cyw43
    pico_cyw43_arch_none
)

# Enable useful logs
# Define WANT_HCI_DUMP=1 to create hci_dump file output on stdio (can be parsed in Wireshark)
target_compile_definitions(pico_mesh_demo PRIVATE
    PICO_RUNTIME=1
)

# UART/USB stdio enable as needed
pico_enable_stdio_usb(pico_mesh_demo 1)
pico_enable_stdio_uart(pico_mesh_demo 0)

pico_add_extra_outputs(pico_mesh_demo)