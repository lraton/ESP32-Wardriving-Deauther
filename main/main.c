#include <stdio.h>
#include "lib.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_event.h"

static const char* TAG = "main";
void codeForTask1_core( void * parameter )
{
   for (;;)
   {
      printf("codeForTask1 is running on Core: ");
      printf("%d\n", xPortGetCoreID());
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }

}
void codeForTask2_core( void * parameter )
{
   for (;;)
   {
      printf("codeForTask2 is running on Core: ");
      printf("%d\n", xPortGetCoreID());
      vTaskDelay(2000 / portTICK_PERIOD_MS);
   }

}

void app_main(void) {
    ESP_LOGD(TAG, "app_main started");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    start_ap_sta(); //Start AP and STA
    start_http_server(); //Start HTTP 
	xTaskCreatePinnedToCore(codeForTask1_core, "core0", 1024*2, NULL, 2, NULL, 0);
	xTaskCreatePinnedToCore(codeForTask2_core, "core1", 1024*2, NULL, 2, NULL, 1);
}
