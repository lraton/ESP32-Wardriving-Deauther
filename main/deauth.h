#include "esp_wifi.h"

typedef uint8_t MacAddr[6];

esp_err_t change_channel(uint8_t channel);

esp_err_t deauth(const MacAddr target, const MacAddr source,const MacAddr bssid, uint8_t reason, uint8_t channel);

esp_err_t raw(const uint8_t* packet, int32_t len, bool en_sys_seq);

esp_err_t esp32_deauther_configure_wifi(uint8_t channel);
