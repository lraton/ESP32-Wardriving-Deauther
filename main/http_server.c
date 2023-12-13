/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "lib.h"
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


static const char *TAG = "webserver";


void generate_dynamic_html(char* dynamic_content);

static esp_err_t main_page_handler(httpd_req_t *req)
{
	esp_err_t error;
    char dynamic_content[2048];
    
    // Generate dynamic HTML content
    generate_dynamic_html(dynamic_content);
	error = httpd_resp_send(req, dynamic_content, HTTPD_RESP_USE_STRLEN);
	if (error != ESP_OK)
	{
		ESP_LOGI(TAG, "Error %d while sending Response", error);
	}
	else ESP_LOGI(TAG, "Response sent Successfully");
	return error;
}

void generate_dynamic_html(char* dynamic_content) {
    // Format the HTML dynamically based on variables
    snprintf(dynamic_content, 2048,
              "<html> <head> <style> table, th, td { width: 100%%; border: 1px solid black; } table { min-height:70vh ; font-size: 2em; } </style> </head> <body> <h1>ESP32 WEBSERVER</h1> <table> <tr> <th><a href=\"downloadwpa\">Download WPA</a></th> <th><a href=\"downloadwpa2\">Downalod WPA2</a></th> <th><a href=\"downloadall\">Download All</a></th> </tr> <br> <tr> <th>SSID</th> <th>HANDSHAKE</th> <th>AuthMode</th> <th>Coordinate</th> <th>Download</th> </tr>");
    for (int i=0; i<5;i++){
        strcat(dynamic_content,"<tr>\
            <td>DIGOS furgone monitoraggio</td>\
            <td>è un piacere</td>\
            <td>VUPIA</td>\
            <td>x-y-z</td>\
            <td><a href=\"download\">Download</a></td>\
        </tr>");
    }
}

static esp_err_t download_page_handler(httpd_req_t *req)
{
	esp_err_t error;
	const char *response = (const char *) req->user_ctx;
	error = httpd_resp_send(req, response, strlen(response));
	if (error != ESP_OK)
	{
		ESP_LOGI(TAG, "Error %d while sending Response", error);
	}
	else ESP_LOGI(TAG, "Response sent Successfully");
	return error;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = main_page_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t download = {
    .uri       = "/download",
    .method    = HTTP_GET,
    .handler   = download_page_handler,
    .user_ctx  = "<!DOCTYPE html>\
<html>\
<head>\
</head>\
<body>\
\
<h1>ESP32 WEBSERVER</h1>\
 <a href=\"/\">Ciao</a>\
</body>\
</html>"
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &download);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void start_http_server(void)
{
	static httpd_handle_t server = NULL;

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_HTTP_SERVER");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
//    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
}