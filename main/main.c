#include <stdio.h>
#include "lib.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_event.h"

static const char* TAG = "main";



void app_main(void) {
    ESP_LOGD(TAG, "app_main started");

    nvs_flash_init();   //inizializza memoria flash
    esp_netif_init();   //inizializza interfaccia di rete
    ESP_ERROR_CHECK(esp_event_loop_create_default());  //runna all' infinito (come arduino Loop)   

    wifictl_mgmt_ap_start(); //inizializza il controller wifi in modalita' APSTA
    
    wardriver_init(); //inizializza la task di Deauth, di packet sniffing, e di serializzazione pcap
    
    start_http_server(); //Starta il server http

    


}
