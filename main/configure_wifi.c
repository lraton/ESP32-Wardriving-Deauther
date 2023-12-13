#include "configure_wifi.h"
#include <string.h>

//esp_err_t esp32_deauther_configure_wifi(uint8_t channel) {
/*    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ciaociao",
            .ssid_len = 22,
            .password = "hello",
            .channel = channel,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = 0,
            .max_connection = 4,
            .beacon_interval = 60000
        }
    };
*/
esp_err_t esp32_deauther_configure_wifi(uint8_t channel){
    wifi_config_t ap_config = {};
    strcpy((char*)ap_config.ap.ssid,"Prosciutto wifi");
    ap_config.ap.ssid_len = 14;
    strcpy((char*)ap_config.ap.password,"C@dedddddd");
    ap_config.ap.channel = channel;
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK,
    ap_config.ap.ssid_hidden = 0;
    ap_config.ap.max_connection = 4;
    ap_config.ap.beacon_interval = 60000;

    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

