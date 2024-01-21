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
#include "serializer.h"
#define TAG "DEBUG"
#define IMP "IMPORTANT"

static wifi_country_t wifi_country = {.cc="CN", .schan = 1, .nchan = 13}; //Most recent esp32 library struct
uint8_t level = 0, channel = 2;

//set the if in promiscuous mode and assign the packet handler
void wifi_sniffer_init(void)
{
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

void wifi_sniffer_set_channel(uint8_t channel)
{
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}
//1° filter eapol packets ->ETHER_TYPE_EAPOL 
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
//2° filter eapol key packets 
eapol_key_packet_t *parse_eapol_key_packet(eapol_packet_t *eapol_packet){
  if(eapol_packet->header.packet_type != EAPOL_KEY){
    ESP_LOGD(TAG, "Not an EAPoL-Key packet.");
    return NULL;
  }
  return (eapol_key_packet_t *) eapol_packet->packet_body;
}

void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
  
  if(type != WIFI_PKT_DATA)
  {
    return;
  }

  wifi_promiscuous_pkt_t *frame = (wifi_promiscuous_pkt_t *) buff;
  data_frame_t *data = (data_frame_t *) frame->payload;
  eapol_packet_t *eapol_packet = parse_eapol_packet((data_frame_t *) frame->payload);
  if(eapol_packet == NULL){
    return;
  }
  eapol_key_packet_t *eapol_key_packet = parse_eapol_key_packet(eapol_packet);
  if(eapol_key_packet == NULL){
    return;
  }
  // for (int i = 0; i < 6; i++) {
  //   ESP_LOGI("SORG", "%02X ,%02X",data->mac_header.addr1[i],data->mac_header.addr2[i]);
  // }
  ESP_LOGI("IMP", "got an Eapol-key-packet");
  pcap_serializer_append_frame(frame->payload, frame->rx_ctrl.sig_len, frame->rx_ctrl.timestamp);
  
}