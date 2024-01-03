
#include "esp_wifi_types.h"
#define ATTACK_PASSIVE_METHOD 0
#define ATTACK_BROADCAST_METHOD 1
#define ATTACK_ROGUE_METHOD 2

void attack_method_broadcast(const wifi_ap_record_t *ap_record, unsigned period_sec);
void attack_method_rogueapp(const wifi_ap_record_t *ap_record);
