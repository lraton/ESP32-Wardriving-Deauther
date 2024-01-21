/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
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
char* dynamic_content = NULL;
void generate_dynamic_html(char* dynamic_content);

//function to  remove a substring from a string
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

//returns a string for corresponding authmode
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


//http request handler main page
static esp_err_t main_page_handler(httpd_req_t *req)
{
	esp_err_t error;
    set_scan();
    ESP_LOGI(TAG,"%d", get_scan());

    //allocation of dynamic content
    if(dynamic_content == NULL){
        dynamic_content = (char *) malloc(sizeof(char) * 6144);
    }
    else{
        dynamic_content = (char *) realloc(dynamic_content,sizeof(char) * 6144);
    }


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

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = main_page_handler,
    .user_ctx  = NULL
};
void generate_dynamic_html(char* dynamic_content) {
    // Format the HTML dynamically based on variables
    
    wifi_ap_record_t* apRecords = getAPrecords();
    uint16_t num_ssid = get_AP_num();
    
    char* authmode;
    while(apRecords==NULL){
        ESP_LOGI(TAG, "waiting for aprecords");
    }

    //--------------HTML---------------------// 
    snprintf(dynamic_content, 2048,
              "<html> <head> <style> table, th, td { width: 100%%; border: 1px solid black; } table { min-height:70vh ; font-size: 2em; } </style> </head> <body> <h1>ESP32 WEBSERVER</h1> <h2>Press attack to attack the network, during the attack ESP32 AP will be down make sure to reconnect and press download</h2><table> <tr><th><a href=\"capture.pcap\">Download All</a></th> </tr> <br> <tr> <th>SSID</th> <th>AuthMode</th> <th>Coordinate</th> <th>Attack</th> <th>DoS Attack</th> </tr>");
    
    ESP_LOGI(TAG, "Num_ssid %d",num_ssid);
    for (int i=0; i<num_ssid;i++){
        authmode= get_auth_mode(apRecords[i].authmode);

        strcat(dynamic_content, "<tr> <td>");
        strcat(dynamic_content, (char*)apRecords[i].ssid);
        strcat(dynamic_content, "</td> <td>");
        strcat(dynamic_content, authmode);
        strcat(dynamic_content, "</td>\
            <td>x-y-z</td>\
            <td><a href=\"attack?");
        strcat(dynamic_content, (char*)apRecords[i].ssid);
        strcat(dynamic_content,"\">Attack</a></td>\
        ");
        strcat(dynamic_content, "<td><a href=\"dos?");
        strcat(dynamic_content, (char*)apRecords[i].ssid);
        strcat(dynamic_content,"\">DoS Attack</a></td>\
        </tr>");
    }
}

//HTTP request handler for /dos
static esp_err_t uri_dos_handler(httpd_req_t *req){

    ESP_LOGI(TAG, "req->uri PRESO DA Attack DoS: %s", (char*)req->uri);

    
    char* encodedUrl;
    //strcpy(ssid,(char*)req->uri);
    encodedUrl = strremove(req->uri,"/dos?");
    char* ssid = decodeUrl(encodedUrl);
    ESP_LOGI(TAG, "SSID->%s", ssid);
    for(uint8_t i = 0; i < 5; i++){
        attack_ssid(ssid);
    }
    free(ssid);
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
static httpd_uri_t uri_dos = {
    .uri = "/dos",
    .method = HTTP_GET,
    .handler = uri_dos_handler,
    .user_ctx = "<h1>GET dossed</h2>"
    
};

//HTTP request handler for /capure.pcap -> DOWNLOAD .pcap
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




static esp_err_t attack_handler(httpd_req_t *req)
{

    ESP_LOGI(TAG, "req->uri PRESO DA attack: %s", (char*)req->uri);
    uint8_t len_uri = strlen((char*)req->uri) + 1;
    ESP_LOGI(TAG,"%d",len_uri );
    char* encodedUrl;
    //strcpy(ssid,(char*)req->uri);
    encodedUrl = strremove(req->uri,"/attack?");
    char* ssid = decodeUrl(encodedUrl);
    ESP_LOGI(TAG, "SSID->%s", ssid);
    attack_ssid(ssid);
    free(ssid);

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
//HTTP request handler for attack
static const httpd_uri_t attack = {
    .uri       = "/attack",
    .method    = HTTP_GET,
    .handler   = attack_handler,
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




int isHexDigit(char c) {
    return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
}

char hexToChar(char c1, char c2) {
    char hex[3] = {c1, c2, '\0'};
    return (char) strtol(hex, NULL, 16);
}

char* decodeUrl(const char* encodedUrl) {
    int len = strlen(encodedUrl);
    char* decodedUrl = (char*)malloc((len + 1) * sizeof(char));

    int i = 0, j = 0;

    while (i < len) {
        if (encodedUrl[i] == '%' && isHexDigit(encodedUrl[i + 1]) && isHexDigit(encodedUrl[i + 2])) {
            decodedUrl[j] = hexToChar(encodedUrl[i + 1], encodedUrl[i + 2]);
            i += 3;
        } else {
            decodedUrl[j] = encodedUrl[i];
            i++;
        }
        j++;
    }

    decodedUrl[j] = '\0';

    return decodedUrl;
}

//HTTP 404 error 
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
        httpd_register_uri_handler(server, &attack);
        httpd_register_uri_handler(server, &uri_capture_pcap_get);
        httpd_register_uri_handler(server, &uri_dos);
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
//starts webserver on STA connection 
static void connect_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

//Starts http server by assigning a connection handler
void start_http_server(void)
{
	static httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "ESP_HTTP_SERVER");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
}