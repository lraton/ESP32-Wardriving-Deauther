#include <stdint.h>
#define LED_GPIO_PIN                     5
#define WIFI_CHANNEL_SWITCH_INTERVAL  (500)
#define WIFI_CHANNEL_MAX               (13)



#define ETHER_TYPE_EAPOL   0x888e


typedef enum {
    EAPOL_EAP_PACKET = 0,
	EAPOL_START,
	EAPOL_LOGOFF,
	EAPOL_KEY,
	EAPOL_ENCAPSULATED_ASF_ALERT,
    EAPOL_MKA,
    EAPOL_ANNOUNCEMENT_GENERIC,
    EAPOL_ANNOUNCEMENT_SPECIFIC,
    EAPOL_ANNOUNCEMENT_REQ
} eapol_packet_types_t;

typedef struct {
    uint8_t protocol_version:2;
    uint8_t type:2;
    uint8_t subtype:4;
    uint8_t to_ds:1;
    uint8_t from_ds:1;
    uint8_t more_fragments:1;
    uint8_t retry:1;
    uint8_t power_management:1;
    uint8_t more_data:1;
    uint8_t protected_frame:1;
    uint8_t htc_order:1;
} frame_control_t;

typedef struct {
  unsigned frame_ctrl:16;
  unsigned duration_id:16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl:16;
  //uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
    frame_control_t frame_control;
    uint16_t duration;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t sequence_control;
} data_frame_mac_header_t;
typedef struct {
	uint8_t version;
	uint8_t packet_type;
	uint16_t packet_body_length;
} eapol_packet_header_t;

typedef struct {
	eapol_packet_header_t header;
	uint8_t packet_body[];
} eapol_packet_t;


typedef struct {
    uint8_t snap_dsap;
    uint8_t snap_ssap;
    uint8_t control;
    uint8_t encapsulation[3];
} llc_snap_header_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

typedef struct {
    data_frame_mac_header_t mac_header;
    uint8_t body[];
} data_frame_t;

typedef struct {
    uint8_t key_descriptor_version:3;
    uint8_t key_type:1;
    uint8_t :2;
    uint8_t install:1;
    uint8_t key_ack:1;
    uint8_t key_mic:1;
    uint8_t secure:1;
    uint8_t error:1;
    uint8_t request:1;
    uint8_t encrypted_key_data:1;
    uint8_t smk_message:1;
    uint8_t :2;
} key_information_t;

typedef struct __attribute__((__packed__)) {
    uint8_t descriptor_type;
    key_information_t key_information;
    uint16_t key_length;
    uint8_t key_replay_counter[8];
    uint8_t key_nonce[32];
    uint8_t key_iv[16];
    uint8_t key_rsc[8];
    uint8_t reserved[8];
    uint8_t key_mic[16];
    uint16_t key_data_length;
    uint8_t key_data[];
} eapol_key_packet_t;

void wifi_sniffer_init(void);
void wifi_sniffer_set_channel(uint8_t channel);
static const char *wifi_sniffer_packet_type2str(wifi_promiscuous_pkt_type_t type);
static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);
eapol_packet_t *parse_eapol_packet(data_frame_t *frame);
