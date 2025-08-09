#pragma once

// Enable BTstack Mesh (also defined by pico_btstack_mesh)
#define ENABLE_MESH

// Use GATT bearer + PB-GATT + Proxy Server for provisioning via smartphone app
#define ENABLE_MESH_GATT_BEARER
#define ENABLE_MESH_PB_GATT
#define ENABLE_MESH_PROXY_SERVER

// BLE roles needed (Security Manager requires at least one role)
#define ENABLE_LE_PERIPHERAL

// HCI dump optional
#define ENABLE_PRINTF_HEXDUMP
// #define ENABLE_LOG_INFO
// #define ENABLE_LOG_ERROR

// Memory pool sizes (conservative defaults for small demo)
#define HCI_ACL_PAYLOAD_SIZE (251 + 4)
#define MAX_NR_HCI_CONNECTIONS 2
#define MAX_NR_L2CAP_SERVICES 6
#define MAX_NR_L2CAP_CHANNELS 6
#define MAX_NR_GATT_CLIENTS 2
#define MAX_ATT_DB_SIZE 1024
#define MAX_NR_WHITELIST_ENTRIES 16
#define MAX_NR_LE_DEVICE_DB_ENTRIES 16
#define NVM_NUM_DEVICE_DB_ENTRIES 16

// Mesh specific buffers
#define MAX_NR_MESH_NETWORK_KEYS        2
#define MAX_NR_MESH_APPLICATION_KEYS    2
#define MAX_NR_MESH_DEV_KEYS            1
#define MAX_NR_MESH_VIRTUAL_ADDRESSES   2
#define MAX_NR_MESH_SUBNETS             2
#define MAX_NR_MESH_TRANSPORT_KEYS      4
#define MAX_NR_MESH_SEGMENTED_INCOMING  4
#define MAX_NR_MESH_SEGMENTED_OUTGOING  4
#define MAX_NR_MESH_TRANS_SUBS          4
#define MAX_NR_MESH_FOUNDATION_MODELS   4
#define MAX_NR_MESH_GENERIC_ONOFF_SERVERS 1