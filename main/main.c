#include <stdio.h>
#include "lib.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_event.h"

static const char* TAG = "main";



void app_main(void) {
    ESP_LOGD(TAG, "app_main started");

    nvs_flash_init();
    esp_netif_init(); 
    ESP_ERROR_CHECK(esp_event_loop_create_default());    
    //start_ap_sta(); //Start AP and STA

    wifictl_mgmt_ap_start();
    //wifictl_restore_ap_mac(); 
    
    wardriver_init(); //Start deauth
    
    start_http_server(); //Start HTTP 

    


}
