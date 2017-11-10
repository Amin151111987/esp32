/*
 * Copyright (C) 2017 Benjamin Aigner, FH Technikum Wien
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
 
#include "btstack.h"
#include "l2cap.h"
#include "btstack_rfc7668.h"
#include "netif/rfc7668.h"
#include "netif/rfc7668_opts.h"
#include "lwip/ip6_addr.h"
#include "lwip/tcpip.h"

int btstack_main(int argc, const char * argv[]);

struct netif rfc7668_netif;

char local_ip6_str[40];
char remote_ip6_str[40];

#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 1000

//WTF is this?!?
#define TEST_COD 0x1234
#define NUM_ROWS 25
#define NUM_COLS 40
#define DATA_VOLUME (10 * 1000 * 1000)

/*
 * @section Advertisements 
 *
 * @text The Flags attribute in the Advertisement Data indicates if a device is in dual-mode or not.
 * Flag 0x06 indicates LE General Discoverable, BR/EDR not supported although we're actually using BR/EDR.
 * In the past, there have been problems with Anrdoid devices when the flag was not set.
 * Setting it should prevent the remote implementation to try to use GATT over LE/EDR, which is not 
 * implemented by BTstack. So, setting the flag seems like the safer choice (while it's technically incorrect).
 */
/* LISTING_START(advertisements): Advertisement data: Flag 0x06 indicates LE-only device */
const uint8_t adv_data[] = {
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x05, 
    // IPSP profile
    0x03, 0x03, 0x20, 0x18,
    // Name
    0x08, 0x09, 'T', 'o', 'R', 'a', 'D', 'e', 'S', 
};

static btstack_packet_callback_registration_t hci_event_callback_registration;
uint8_t adv_data_len = sizeof(adv_data);
static bd_addr_t peer_addr;

//IPSP - buffer
static uint8_t   ipsp_service_buffer[1300];
static uint8_t   ipsp_addr_type;
static uint16_t  ipsp_cid = 0;
static hci_con_handle_t le_connection_handle;
ip6_addr_t src;
bd_addr_t address_buffer;

/** 
 * Get the current remote BT address
 */
bd_addr_t* getBTRemoteAddress(void)
{
	return &peer_addr;
}

/** 
 * Get the current host BT address
 */
bd_addr_t * getBTLocalAddress(void)
{
	gap_local_bd_addr(address_buffer);
	return &address_buffer;
}

/**
 * Get the current local host address string (IPv6)
 * */
char* getLocalIPv6Address(void)
{
	return local_ip6_str;
}

/**
 * Get the current remote (BT host) address string (IPv6)
 * ONLY VALID AFTER CONNECTED!
 * */
char* getRemoteIPv6Address(void)
{
	return remote_ip6_str;
}


/* 
 * @section Packet Handler
 * 
 * @text The packet handler of the combined example is just the combination of the individual packet handlers.
 */
static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    UNUSED(channel);

    bd_addr_t event_addr;
    //int i;
    //int num_responses;

	switch (packet_type) {
		case HCI_EVENT_PACKET:
			switch (hci_event_packet_get_type(packet)) {

				case HCI_EVENT_PIN_CODE_REQUEST:
					// inform about pin code request
                    printf("Pin code request - using '0000'\n");
                    hci_event_pin_code_request_get_bd_addr(packet, event_addr);
					hci_send_cmd(&hci_pin_code_request_reply, &event_addr, 4, "0000");
					break;

                case HCI_EVENT_USER_CONFIRMATION_REQUEST:
                    // inform about user confirmation request
                    printf("SSP User Confirmation Request with numeric value '%06"PRIu32"'\n", little_endian_read_32(packet, 8));
                    printf("SSP User Confirmation Auto accept\n");
                    break;

                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    break;

                case HCI_EVENT_LE_META:
                    switch (hci_event_le_meta_get_subevent_code(packet)) {
                        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                            break;
                    }
                    break;  

                case GAP_EVENT_ADVERTISING_REPORT:
                    break;

                case BTSTACK_EVENT_STATE:
                    if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
                    break;

                case HCI_EVENT_EXTENDED_INQUIRY_RESPONSE:
                    break;
                    
                case HCI_EVENT_INQUIRY_COMPLETE:
                    printf("Inquiry complete\n");
                    break;
                    
                case HCI_EVENT_COMMAND_COMPLETE:
                case HCI_EVENT_NUMBER_OF_COMPLETED_PACKETS:
                //completely unknown...
                case 0x61:
                case 0x66:
                case 0x6e:
					//Command complete, do nothing...
					break;

                default:
					printf("Unknown HCI EVT: %d / 0x%x\n",hci_event_packet_get_type(packet),hci_event_packet_get_type(packet));
                    break;
			}
            break;

        default:
			printf("Unknown packet_type: %d / 0x%x\n",packet_type,packet_type);
            break;
	}
}

/** Callback for IPSP L2CAP events
 * 
 * This method is the main connection between BTStack and the RFC7668
 * implementation.
 * If BTStack should be replaced (for example Bluedroid), this callback is necessary
 * to be called on all L2CAP events.
 * It handles incoming connections and transfers data to lwIP.
 * */
void l2cap_ipsp_cb(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
	uint8_t ret = 0;
	struct pbuf *p;
	ip6_addr_t ip6addr;
	
	switch(packet_type)
	{
		case HCI_EVENT_PACKET:
			switch (hci_event_packet_get_type(packet)) {
				                    
                case L2CAP_EVENT_LE_INCOMING_CONNECTION:
					ipsp_cid = l2cap_event_le_incoming_connection_get_local_cid(packet);
					le_connection_handle = l2cap_event_le_incoming_connection_get_handle(packet);
					l2cap_event_le_incoming_connection_get_address(packet, peer_addr);
					
					//save remote address for further connecting
					ble_addr_to_eui64(&((uint8_t *)&ip6addr)[8],(uint8_t *)peer_addr,0);
					memset(&((uint8_t *)&ip6addr)[2],0,6);
					((uint8_t *)&ip6addr)[0] = 0xFE;
					((uint8_t *)&ip6addr)[1] = 0x80;
					strcpy(remote_ip6_str,ip6addr_ntoa(&ip6addr));
					
					
					ipsp_addr_type = l2cap_event_le_incoming_connection_get_address_type(packet);
					ret = l2cap_le_accept_connection(ipsp_cid,ipsp_service_buffer,sizeof(ipsp_service_buffer),L2CAP_LE_AUTOMATIC_CREDITS);
					if(ret != 0) 
					{
						ESP_LOGE(LOWPAN_BLE_TAG,"IPSP - error accepting conn: %d",ret);
					} else {
						ESP_LOGD(LOWPAN_BLE_TAG,"IPSP - incoming connection accepted, local CID: %d, handle: %d",ipsp_cid,le_connection_handle);
						netif_set_link_up(&rfc7668_netif);
						netif_set_up(&rfc7668_netif);
						xEventGroupSetBits(lowpan_ble_flags,LOWPAN_BLE_FLAG_CONNECTED);
					}
					break;
                case L2CAP_EVENT_LE_CAN_SEND_NOW:
					#if LOWPAN_BLE_DEBUG_L2CAP_CB
						ESP_LOGD(LOWPAN_BLE_TAG,"IPSP - LE can send now...");
					#endif
					break;
                case L2CAP_EVENT_LE_PACKET_SENT:
					#if LOWPAN_BLE_DEBUG_L2CAP_CB
						ESP_LOGD(LOWPAN_BLE_TAG,"IPSP - LE packet sent...");
					#endif
					break;
                case L2CAP_EVENT_LE_CHANNEL_CLOSED:
					ESP_LOGD(LOWPAN_BLE_TAG,"IPSP - LE channel closed...");
					l2cap_le_disconnect(ipsp_cid);
					xEventGroupClearBits(lowpan_ble_flags,LOWPAN_BLE_FLAG_CONNECTED);
					break;
                case L2CAP_EVENT_LE_CHANNEL_OPENED:
					#if LOWPAN_BLE_DEBUG_L2CAP_CB
						ESP_LOGD(LOWPAN_BLE_TAG,"IPSP - LE channel opened...");
					#endif
					break;
					
				case L2CAP_EVENT_CHANNEL_CLOSED:
					#if LOWPAN_BLE_DEBUG_L2CAP_CB
						ESP_LOGD(LOWPAN_BLE_TAG,"IPSP - channel closed...");
					#endif
					l2cap_disconnect(ipsp_cid, 0);
					break;
				                    
                case L2CAP_EVENT_CHANNEL_OPENED:
                case L2CAP_EVENT_INCOMING_CONNECTION:
				case L2CAP_EVENT_CAN_SEND_NOW:
				
				case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST:
				case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE:
				case L2CAP_EVENT_TIMEOUT_CHECK:
					ESP_LOGE(LOWPAN_BLE_TAG,"IPSP - event without LE? Problem... (EVT: %d / 0x%2X)", hci_event_packet_get_type(packet),hci_event_packet_get_type(packet));
					break;
                    
				default:
					ESP_LOGE(LOWPAN_BLE_TAG,"IPSP - HCI EVT: %d, 0x%2X",hci_event_packet_get_type(packet),hci_event_packet_get_type(packet));
					break;
			}
		break;
		
		//If L2CAP data arrives -> allocate pbuf memory from lwIP -> send to network IF
		case L2CAP_DATA_PACKET:	
			if(ipsp_addr_type == BD_ADDR_TYPE_LE_PUBLIC)
			{
				ble_addr_to_eui64((uint8_t *)src.addr, peer_addr, 1);
			} else {
				ble_addr_to_eui64((uint8_t *)src.addr, peer_addr, 0);
			}
			
			p = pbuf_alloc(PBUF_RAW, 512, PBUF_RAM);
			p->payload = packet;
			p->len = size;
			#if LOWPAN_BLE_DEBUG_L2CAP_CB
				ESP_LOGD(LOWPAN_BLE_TAG,"L2CAP: len: %d, tot_len: %d",p->len,p->tot_len);
			#endif
			rfc7668_input(p,&rfc7668_netif,&src);
		break;
				
		default:
			ESP_LOGE(LOWPAN_BLE_TAG,"IPSP, unknown packet type: %d / 0x%2X",packet_type,packet_type);
		break;
	}
}

/**
 * Output method to send data from the lwIP netif to L2CAP connection
 * */
err_t rfc7668_send_L2CAP(struct netif *netif, struct pbuf *p)
{
	return l2cap_le_send_data(ipsp_cid, p->payload, p->len);
}

/**
 * Main init function for BTStack
 * 
 * This method inits all necessary callbacks, adds pairing standards
 * and attaches the IPSP service.
 * In addition, the RFC7668 netif is added to lwIP
 * 
 * */
int btstack_main(int argc, const char * argv[])
{
    UNUSED(argc);
    (void)argv;
    ip6_addr_t ip6addr;

	//create flag group
	lowpan_ble_flags = xEventGroupCreate();

    // register for HCI events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

	//TODO: hier die security/pairing sachen richtig einstellen...
    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);

    // short-cut to find other ODIN module?!? -> welche COD (class of device)?
    gap_set_class_of_device(TEST_COD);

    gap_discoverable_control(1);

    // setup le device db
    le_device_db_init();

    // enabled EIR
    hci_set_inquiry_mode(INQUIRY_MODE_STANDARD);
    //hci_set_inquiry_mode(INQUIRY_MODE_RSSI);

    // setup SM: Display only
    sm_init();


    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
    gap_advertisements_enable(1);
    
    //setup l2cap for IPSP support
    //BT_PSM_IPSP => 0x0023 /* Internet Protocol Support Profile  */
    //WARNING: change security level 0!!!
    l2cap_le_register_service(l2cap_ipsp_cb,PSM_IPSP,LEVEL_0);
    
    //save local IPv6 address (if needed by other parts/layers)
    ble_addr_to_eui64(&((uint8_t *)&ip6addr)[8],(uint8_t *)getBTLocalAddress(),0);
	memset(&((uint8_t *)&ip6addr)[2],0,6);
	((uint8_t *)&ip6addr)[0] = 0xFE;
	((uint8_t *)&ip6addr)[1] = 0x80;
	strcpy(local_ip6_str,ip6addr_ntoa(&ip6addr));
    

    // turn on!
	hci_power_control(HCI_POWER_ON);
	
	//activate LWIP stack & add the RFC7668 network IF
	tcpip_init(NULL, NULL); 
	
	netif_add(&rfc7668_netif, 
	//fields are only available if compiled with IPv4
	#if LWIP_IPV4
		NULL,NULL,NULL,
	#endif /* LWIP_IPV4 */
	 NULL, rfc7668_if_init, rfc7668_input);
	 netif_add_ip6_address(&rfc7668_netif,&ip6addr,NULL);
	 rfc7668_netif.name[0] = 'B';
	 rfc7668_netif.name[1] = 'T';
	 
	rfc7668_netif.linkoutput = rfc7668_send_L2CAP;

    return 0;
}
