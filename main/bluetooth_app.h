#ifndef _BLUETOOTH_APP_H_E
#define _BLUETOOTH_APP_H_

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"

void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                      esp_ble_mesh_prov_cb_param_t *param);

void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
                                       esp_ble_mesh_cfg_server_cb_param_t *param);

void example_ble_mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event,
                                      esp_ble_mesh_model_cb_param_t *param);

esp_err_t ble_mesh_init(void);
esp_err_t ble_mesh_reset(void); 
void publish_trigger(void);

#endif