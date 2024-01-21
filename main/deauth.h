#include "esp_wifi.h"

typedef uint8_t MacAddr[6];

void print_cipher_type(int pairwise_cipher, int group_cipher);

void print_auth_mode(int authmode);

esp_err_t change_channel(uint8_t channel);

esp_err_t deauth(const MacAddr target, const MacAddr source,const MacAddr bssid, uint8_t reason, uint8_t channel);

esp_err_t raw(const uint8_t* packet, int32_t len, bool en_sys_seq);