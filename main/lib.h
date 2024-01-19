
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_err.h"
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include <esp_http_server.h>

void start_ap_sta();
void start_http_server();
void main_deauth();

esp_netif_t *wifi_init_softap(void);
esp_netif_t *wifi_init_sta(void);

void wifictl_restore_ap_mac(void);
void wifictl_mgmt_ap_start(void);
void wifictl_ap_start(wifi_config_t *);
