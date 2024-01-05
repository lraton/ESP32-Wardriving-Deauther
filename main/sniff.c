#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <arpa/inet.h>
#include "sniff.h"

#define TAG "DEBUG"
#define IMP "IMPORTANT"

static wifi_country_t wifi_country = {.cc="CN", .schan = 1, .nchan = 13}; //Most recent esp32 library struct
uint8_t level = 0, channel = 2;

void wifi_sniffer_init(void)
{
 esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void wifi_sniffer_set_channel(uint8_t channel)
{
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

// void printByte(eapol_key_packet_t pkt)
// {
//   uint16_t numBytes = sizeof(pkt);
//   ESP_LOGI("Block of %d bytes:\n", numBytes);
//   for (int i = 0; i < numBytes; ++i) {
     
//     printf("%02X ", bytes[i]);
//   }
//   printf("\n");

//   return 0;
// }


eapol_packet_t *parse_eapol_packet(data_frame_t *frame) {
  uint8_t *frame_buffer = frame->body;

  if(frame->mac_header.frame_control.protected_frame == 1) {
    //ESP_LOGI(TAG, "Protected frame, skipping...");
    return NULL;
  }

  if(frame->mac_header.frame_control.subtype > 7) {
    //ESP_LOGI(TAG, "QoS data frame");
    // Skipping QoS field (2 bytes)

    frame_buffer += 2;
  }

  // Skipping LLC SNAP header (6 bytes)
  frame_buffer += sizeof(llc_snap_header_t);

  // Check if frame is type of EAPoL
  if(ntohs(*(uint16_t *) frame_buffer) == ETHER_TYPE_EAPOL) {
    //ESP_LOGI(IMP, "EAPOL packet");
    frame_buffer += 2;
    return (eapol_packet_t *) frame_buffer; 
  }
  return NULL;
}

eapol_key_packet_t *parse_eapol_key_packet(eapol_packet_t *eapol_packet){
  if(eapol_packet->header.packet_type != EAPOL_KEY){
    ESP_LOGD(TAG, "Not an EAPoL-Key packet.");
    return NULL;
  }
  return (eapol_key_packet_t *) eapol_packet->packet_body;
}

const char * wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type)
{
  switch(type) {
  case WIFI_PKT_MGMT: return "MGMT";
  case WIFI_PKT_DATA: return "DATA";
  default:  
  case WIFI_PKT_MISC: return "MISC";
  }
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
  
  if(type != WIFI_PKT_DATA)
  {
    return;
  }

  // ESP_LOGI(TAG, "packet type : %s, channel : %d", wifi_sniffer_packet_type2str(type), channel);


  wifi_promiscuous_pkt_t *frame = (wifi_promiscuous_pkt_t *) buff;
  data_frame_t *data = (data_frame_t *) frame->payload;
  eapol_packet_t *eapol_packet = parse_eapol_packet((data_frame_t *) frame->payload);
  if(eapol_packet == NULL){
    // ESP_LOGI(TAG, "Not an EAPOL packet.");
    return;
  }
  eapol_key_packet_t *eapol_key_packet = parse_eapol_key_packet(eapol_packet);
  if(eapol_key_packet == NULL){
    // ESP_LOGV(TAG, "Not an EAPOL-Key packet");
    return;
  }
  // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_post(FRAME_ANALYZER_EVENTS, DATA_FRAME_EVENT_EAPOLKEY_FRAME, frame, sizeof(wifi_promiscuous_pkt_t) + frame->rx_ctrl.sig_len, portMAX_DELAY));
  // printByte(eapol_key_packet);
  for (int i = 0; i < 6; i++) {
    ESP_LOGI("SORG", "%02X ,%02X",data->mac_header.addr1[i],data->mac_header.addr2[i]);
  }
  ESP_LOGI(IMP, "got an Eapol-key-packet : %0x",*(((unsigned int*)eapol_key_packet)));
}



