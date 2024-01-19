#include <stdio.h>
#include "lib.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_event.h"

static const char* TAG = "main";

void app_main(void) {
    ESP_LOGD(TAG, "app_main started");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    main_deauth(); //Start deauth
    start_ap_sta(); //Start AP and STA
    start_http_server(); //Start HTTP 
    while(1){
        vTaskDelay(1000/ portTICK_PERIOD_MS);
    }
}
