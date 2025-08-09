#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "btstack.h"
#include "mesh_proxy.h"  // generated from mesh_proxy.gatt
#include "btstack_run_loop_embedded.h"
#include "pico/btstack_hci_transport_cyw43.h"

// GAP device name buffer used by GATT
static char gap_name_buffer[] = "Mesh PicoW 00:00:00:00:00:00";

// Generic OnOff Server model and state
static mesh_model_t                 mesh_generic_on_off_server_model;
static mesh_generic_on_off_state_t  mesh_generic_on_off_state;

static btstack_packet_callback_registration_t hci_event_cb;

#ifdef ENABLE_MESH_GATT_BEARER
static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
    (void)connection_handle;
    if (att_handle == ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE){
        return att_read_callback_handle_blob((const uint8_t *)gap_name_buffer, (uint16_t) strlen(gap_name_buffer), offset, buffer, buffer_size);
    }
    return 0;
}
#endif

// Handle provisioning and state updates
static void mesh_provisioning_message_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    (void)channel; (void)size;
    if (packet_type != HCI_EVENT_PACKET) return;
    switch(packet[0]){
        case HCI_EVENT_MESH_META:
            switch(packet[2]){
                case MESH_SUBEVENT_PB_TRANSPORT_LINK_OPEN:
                    printf("Provisioner link opened\n");
                    break;
                case MESH_SUBEVENT_ATTENTION_TIMER:
                    printf("Attention Timer: %u\n", mesh_subevent_attention_timer_get_attention_time(packet));
                    break;
                case MESH_SUBEVENT_PB_TRANSPORT_LINK_CLOSED:
                    printf("Provisioner link closed\n");
                    break;
                case MESH_SUBEVENT_PB_PROV_COMPLETE:
                    printf("Provisioning complete\n");
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

static void mesh_state_update_message_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    (void)channel; (void)size;
    if (packet_type != HCI_EVENT_PACKET) return;
    if (packet[0] != HCI_EVENT_MESH_META) return;
    switch(packet[2]){
        case MESH_SUBEVENT_STATE_UPDATE_BOOL: {
            uint8_t value = mesh_subevent_state_update_bool_get_value(packet);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, value ? 1 : 0);
            printf("Generic OnOff -> LED: %u\n", value);
            break;
        }
        default:
            break;
    }
}

static void setup_mesh(void){
#ifdef ENABLE_MESH_GATT_BEARER
    // L2CAP + ATT + SM for GATT bearer
    l2cap_init();
    att_server_init(profile_data, &att_read_callback, NULL);
    sm_init();
#endif

    // Mesh core
    mesh_init();

#ifdef ENABLE_MESH_GATT_BEARER
    // Make connectable advertising (for PB-GATT)
    bd_addr_t null_addr; memset(null_addr, 0, 6);
    uint8_t adv_type = 0;   // ADV_IND
    uint16_t adv_int_min = 0x0030; // 30 ms
    uint16_t adv_int_max = 0x0030;
    adv_bearer_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
#endif

    // Track provisioning
    mesh_register_provisioning_device_packet_handler(&mesh_provisioning_message_handler);

    // Element location (arbitrary reasonable location code)
    mesh_node_set_element_location(mesh_node_get_primary_element(), 0x103);

    // Setup Generic On/Off model
    mesh_generic_on_off_server_model.model_identifier = mesh_model_get_model_identifier_bluetooth_sig(MESH_SIG_MODEL_ID_GENERIC_ON_OFF_SERVER);
    mesh_generic_on_off_server_model.operations = mesh_generic_on_off_server_get_operations();
    mesh_generic_on_off_server_model.model_data = (void *) &mesh_generic_on_off_state;
    mesh_generic_on_off_server_register_packet_handler(&mesh_generic_on_off_server_model, &mesh_state_update_message_handler);
    mesh_element_add_model(mesh_node_get_primary_element(), &mesh_generic_on_off_server_model);

    // Enable Proxy for PB-GATT
    mesh_foundation_gatt_proxy_set(1);

#ifdef ENABLE_MESH_ADV_BEARER
    // If ADV bearer is enabled, start scanning
    gap_set_scan_parameters(0, 0x300, 0x300);
    gap_start_scan();
#endif

    // Device UUID (example value; you may want to derive from unique ID)
    static const uint8_t device_uuid[16] = {
        0x00, 0x1B, 0xDC, 0x08, 0x10, 0x21, 0x0B, 0x0E,
        0x0A, 0x0C, 0x00, 0x0B, 0x0E, 0x0A, 0x0C, 0x00
    };
    mesh_node_set_device_uuid(device_uuid);
}

static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    (void)channel; (void)size;
    if (packet_type != HCI_EVENT_PACKET) return;
    switch (hci_event_packet_get_type(packet)){
        case BTSTACK_EVENT_STATE: {
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) break;
            // setup gap name from local address
            bd_addr_t addr;
            gap_local_bd_addr(addr);
            btstack_replace_bd_addr_placeholder((uint8_t*)gap_name_buffer, sizeof(gap_name_buffer), addr);
            break;
        }
        default:
            break;
    }
}

int main(void){
    stdio_init_all();

    // CYW43 init
    if (cyw43_arch_init()){
        printf("CYW43 init failed\n");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    // BTstack core
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_embedded_get_instance());

    // HCI over CYW43
    hci_init(hci_transport_cyw43_instance(), NULL);

    // Register HCI events
    hci_event_cb.callback = &hci_event_handler;
    hci_add_event_handler(&hci_event_cb);

    // Setup mesh and power on
    setup_mesh();
    hci_power_control(HCI_POWER_ON);

    // Run loop
    btstack_run_loop_execute();
    return 0;
}