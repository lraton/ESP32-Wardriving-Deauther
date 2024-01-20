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
#include "serializer.h"
#include "esp_err.h"
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include <esp_http_server.h>


static const char *TAG = "webserver";
char* get_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        return "OPEN";
        break;
    case WIFI_AUTH_OWE:
        return "WIFI_AUTH_OWE";
        break;
    case WIFI_AUTH_WEP:
        return "WIFI_AUTH_WEP";
        break;
    case WIFI_AUTH_WPA_PSK:
        return "WIFI_AUTH_WPA_PSK";
        break;
    case WIFI_AUTH_WPA2_PSK:
        return "WIFI_AUTH_WPA2_PSK";
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        return "WIFI_AUTH_WPA_WPA2_PSK";
        break;
    case WIFI_AUTH_ENTERPRISE:
        return "WIFI_AUTH_ENTERPRISE";
        break;
    case WIFI_AUTH_WPA3_PSK:
        return "WIFI_AUTH_WPA3_PSK";
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        return "WIFI_AUTH_WPA2_WPA3_PSK";
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        return "WIFI_AUTH_WPA3_ENT_192";
        break;
    default:
        return "ERROR DETECTING AUTHMODE";
        break;
    }
}

void generate_dynamic_html(char* dynamic_content);

static esp_err_t main_page_handler(httpd_req_t *req)
{
	esp_err_t error;
    set_scan();
    ESP_LOGI(TAG,"%d", get_scan());
    char* dynamic_content = (char *) malloc(sizeof(char)* 6144);
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
    
    wifi_ap_record_t* apRecords = getAPrecords();
    
    uint16_t num_ssid = get_AP_num();
    
    char* authmode;
    //roba da stampare
    //apRecords[i].ssid
    //apRecords[i].authmode
    snprintf(dynamic_content, 2048,
              "<html> <head> <style> table, th, td { width: 100%%; border: 1px solid black; } table { min-height:70vh ; font-size: 2em; } </style> </head> <body> <h1>ESP32 WEBSERVER</h1> <table> <tr><th><a href=\"captureall.pcap\">Download All</a></th> </tr> <br> <tr> <th>SSID</th> <th>AuthMode</th> <th>Coordinate</th> <th>Download</th> </tr>");
    ESP_LOGI(TAG,"//////%s//////", apRecords[0].ssid);
    ESP_LOGI(TAG, "Num_ssid %d",num_ssid);
    for (int i=0; i<num_ssid;i++){
        authmode= get_auth_mode(apRecords[i].authmode);

        strcat(dynamic_content, "<tr> <td>");
        strcat(dynamic_content, (char*)apRecords[i].ssid);
        strcat(dynamic_content, "</td> <td>");
        strcat(dynamic_content, authmode);
        strcat(dynamic_content, "</td>\
            <td>x-y-z</td>\
            <td><a href=\"download.pcap?");
        strcat(dynamic_content, (char*)apRecords[i].ssid);
        strcat(dynamic_content,"\">Download</a></td>\
        </tr>");
    }
    strcat(dynamic_content, "<button onclick=\"location.href=\'new-scan\'\" type=\"button\">\
         Scan now</button>");
}

static esp_err_t uri_new_scan_handler(httpd_req_t *req){
    ESP_LOGD(TAG, "setting scan");
    ESP_ERROR_CHECK(httpd_resp_set_type(req, HTTPD_TYPE_OCTET));
    set_scan();
    const char *response = (const char *) req->user_ctx;
    return httpd_resp_send(req,response, strlen(response));
    
}

static httpd_uri_t uri_new_scan_get = {
    .uri = "/scan",
    .method = HTTP_GET,
    .handler = uri_new_scan_handler,
    .user_ctx = NULL
    
};


static esp_err_t uri_capture_pcap_get_handler(httpd_req_t *req){
    ESP_LOGD(TAG, "Providing PCAP file...");
    ESP_ERROR_CHECK(httpd_resp_set_type(req, HTTPD_TYPE_OCTET));
    return httpd_resp_send(req, (char *) pcap_serializer_get_buffer(), pcap_serializer_get_size());
}

static httpd_uri_t uri_capture_pcap_get = {
    .uri = "/capture.pcap",
    .method = HTTP_GET,
    .handler = uri_capture_pcap_get_handler,
    .user_ctx = NULL
};

char *strremove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}


static esp_err_t download_page_handler(httpd_req_t *req)
{

    ESP_LOGI(TAG, "req->uri PRESO DA DOWNLOAD.PCAP: %s", (char*)req->uri);

    uint8_t len_uri = strlen((char*)req->uri) + 1;
    ESP_LOGI(TAG,"%d",len_uri );
    //uint8_t len_uri_tagliare = strlen("/download.pcap?");
    //uint8_t len_ssid = len_uri - len_uri_tagliare;
    
    char* ssid;
    //strcpy(ssid,(char*)req->uri);
    ssid = strremove(req->uri,"/download.pcap?");

    attack_ssid(ssid);

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
    .uri       = "/download.pcap",
    .method    = HTTP_GET,
    .handler   = download_page_handler,
    .user_ctx  = "<!DOCTYPE html>\
<html>\
<head>\
</head>\
<body>\
\
<h1>ESP32 WEBSERVER</h1>\
 <a href=\"/capture.pcap\">Ciao</a>\
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
        httpd_register_uri_handler(server, &uri_capture_pcap_get);
        httpd_register_uri_handler(server, &uri_new_scan_get);
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

    // //Initialize NVS
    // esp_err_t ret = nvs_flash_init();
    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //   ESP_ERROR_CHECK(nvs_flash_erase());
    //   ret = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_HTTP_SERVER");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
//    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
}