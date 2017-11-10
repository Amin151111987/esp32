/**
  * @file
 *
 * MQTT-SN API, working with FreeRTOS & lwIP
 */

/*
 * Copyright (c) 2017 Benjamin Aigner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Benjamin Aigner <aignerb@technikum-wien.at>
 */

#ifndef _MQTT_SN_CLIENT_H_
#define _MQTT_SN_CLIENT_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "esp_log.h"

#include <string.h>

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "MQTTSNPacket.h"
#include "MQTTSNConnect.h"
#include "MQTTSNPublish.h"

#define MQTT_SN_TICKS_TO_WAIT 5000/portTICK_PERIOD_MS


typedef struct mqtt_sn_broker_cfg {
	//string representation of the gateway's ip address
	char gateway_addr[IP6ADDR_STRLEN_MAX];
	//gateway port number
	uint16_t gateway_port;
	//local port number, if not set 1883 is used.
	uint16_t local_port;
	//bind to a defined network interface
	struct netif *interface;
	//IP version (either IPADDR_TYPE_V6 or IPADDR_TYPE_V4; V4 is currently NOT supported)
	uint8_t ip_type;
} mqtt_sn_broker_cfg_t;

typedef struct mqtt_sn_client_cfg {
	//client id, unique string identifier, max 23 characters (MQTT-SN specs v1.2)
	char clientID[24];
	//value of keep alive timer ([s], same as for MQTT)
	uint16_t duration;

	//clean session flag, documentation in MQTT-SN specs v1.2 or paho mqtt-sn
	uint8_t cleansession;
	//will flag, documentation in MQTT-SN specs v1.2 or paho mqtt-sn
	uint8_t willFlag;
	
	//will topic name
	char willTopic[24];
} mqtt_sn_client_cfg_t;


typedef struct mqtt_sn_topic_register_cfg_t {
	char * topicName;
	uint16_t topicID;
} mqtt_sn_topic_register_cfg_t;



typedef struct mqtt_sn_pkt {
	char topicShort[2];
	uint16_t topicID;
	unsigned char *payload;
	int len;
	uint8_t dup;
	int qos;
	uint8_t retained;
} mqtt_sn_pkt_t;



/** Function prototype for a topic subscribe callback.
 * This cb is called if data on a configured topic arrives.
 *
 * @param data The received data
 * @param topic_cfg Topic subscription configuration (name, QoS, ...)
 */
typedef err_t (*mqtt_sn_topic_subscribe_cb)(mqtt_sn_pkt_t* topicData);


typedef struct mqtt_sn_topic_subscribe_cfg {
	char * topicName;
	uint16_t topicID;
	mqtt_sn_topic_subscribe_cb cb;
} mqtt_sn_topic_subscribe_cfg_t;


/** Initialise the MQTT-SN client
 * This method initialises the MQTT-SN client.
 * For all full list of configuration options see @see mqtt_sn_broker_cfg_t
 * Must be called prior to all publish/subscribe actions.
 * 
 * @param config Provides a broker & network interface configuration
 */
err_t mqtt_sn_client_init(mqtt_sn_broker_cfg_t *config, mqtt_sn_client_cfg_t *client_cfg);

/** Add a subscription to a specific topic, with callbacks
 * This method subscribes to a topic at the broker. Incoming data
 * is passed to the callback method, provided with the configuration.
 * All available options are described at @see mqtt_sn_topic_susbcribe_cfg_t
 * 
 * @param config Subscription configuration
 */
err_t mqtt_sn_client_add_subscribe_topic(mqtt_sn_topic_subscribe_cfg_t *config);

/** Add a mapping of topic name to topic id.
 * 
 * Due to the restriction of packet sizes within sensor networks, MQTT-SN provides
 * the possibility to acquire a topic id for a given topic name at the MQTT-SN gateway
 * Further publishing/subscribing is handled via the 16bit topic id (saving a lot of data transmission)
 * 
 * The resulting topicID is found in the mqtt_sn_topic_register_cfg_t struct, if
 * this method returns with ERR_OK.
 * 
 * All available options are described at @see mqtt_sn_topic_register_cfg_t
 * 
 * @param config Register topic configuration
 */
err_t mqtt_sn_client_register_topic(mqtt_sn_topic_register_cfg_t *config);

/** Publish data, use short topic name (2 characters!)
 * This method publishes a given MQTT-SN packet ( @see mqtt_sn_publish_pkt_t )
 * to a topic, addressed via the short topic name. This is usually faster than
 * a topic name to topic id mapped publishing, but it is limited to a 2 character
 * topic name.
 * 
 * @param packet Transmitted packet
 */
err_t mqtt_sn_client_publish_short(mqtt_sn_pkt_t *packet);


/** Publish data, use topic id (already registered)
 * This method publishes a given MQTT-SN packet ( @see mqtt_sn_publish_pkt_t )
 * to a topic, addressed via a previously registered topic id 
 * ( @see mqtt_sn_client_add_publish_topic ).
 * Also very fast (similar to publish to a short topic), but it is necessary
 * to register the topic in advance.
 * Recommended for regular publishing.
 * 
 * @param packet Transmitted packet
 */
err_t mqtt_sn_client_publish_registered(mqtt_sn_pkt_t *packet);

/** Publish data, use a full topic name
 * This method publishes a given MQTT-SN packet ( @see mqtt_sn_publish_pkt_t )
 * to a topic, addressed via the full topic name.
 * Slow & much overhead compared to other publishing, but saves memory if
 * data is sent only once or twice ( no mapping name<->topicId saved )
 * Recommended for e.g. single publishing.
 * 
 * @param packet Transmitted packet
 * @param topicName Temporary topic name to publish this packet
 */
err_t mqtt_sn_client_publish_unregistered(mqtt_sn_pkt_t *packet, char *topicName);

/** Get the current client flags
 * This method returns the current status flags.
 * 
 * @see MQTT_SN_CLIENT_IDLE
 * @see MQTT_SN_CLIENT_CONNECTING
 * @see MQTT_SN_CLIENT_ACTIVE
 * @see MQTT_SN_CLIENT_ASLEEP
 * @see MQTT_SN_CLIENT_AWAKE
 * @return MQTT-SN client status flags
 */
uint8_t mqtt_sn_client_get_status();

/** Wait for a client status flag(s)
 *
 * This method blocks until one or more states of the client apply.
 * Similar usage as @see xEventGroupWaitBits, but limited to flags to 
 * wait for and ticks to wait.
 * 
 * @see MQTT_SN_CLIENT_IDLE
 * @see MQTT_SN_CLIENT_CONNECTING
 * @see MQTT_SN_CLIENT_ACTIVE
 * @see MQTT_SN_CLIENT_ASLEEP
 * @see MQTT_SN_CLIENT_AWAKE
 * @return MQTT-SN client status flags at the time of the unblock
 */
uint8_t mqtt_sn_client_wait_status(const EventBits_t state, TickType_t ticksToWait);

/** Wait for a gateway PUBACK
 *
 * This method blocks until the gateway returns PUBACK for publishing
 * with a QoS > 0.
 * Similar usage as @see xEventGroupWaitBits, but only with ticks to wait.
 * 
 * @see MQTT_SN_CLIENT_STATE_PUBACK_RECV
 * @return MQTT-SN client ACK flags at the time of the unblock
 */
uint8_t mqtt_sn_client_wait_puback(TickType_t ticksToWait);


/** signals, that the client is currently idle (disconnected or lost) **/
#define MQTT_SN_CLIENT_IDLE (1<<0)

/** signals, that the client is currently connecting **/
#define MQTT_SN_CLIENT_CONNECTING (1<<1)

/** signals, that the client is currently connected **/
#define MQTT_SN_CLIENT_ACTIVE (1<<2)

/** signals, that the client is in sleep state **/
#define MQTT_SN_CLIENT_ASLEEP (1<<3)

/** signals, that the client is awake (during pings in asleep state only) **/
#define MQTT_SN_CLIENT_AWAKE (1<<4)



/** internal client state flags, something we waited for happened */

/** gateway acknowledged topic register DO NOT USE (internally used) */
#define MQTT_SN_CLIENT_STATE_REGACK_RECV (1<<0)
/** gateway acknowledged a PUBLISH action (if qos > 0), can be
 * used with @see mqtt_sn_client_wait_ack */
#define MQTT_SN_CLIENT_STATE_PUBACK_RECV (1<<1)
/** gateway acknowledged topic subscribe DO NOT USE (internally used) */
#define MQTT_SN_CLIENT_STATE_SUBACK_RECV (1<<2)
/** gateway acknowledged topic unsubscribe DO NOT USE (internally used) */
#define MQTT_SN_CLIENT_STATE_UNSUBACK_RECV (1<<3)


//normally unchanged settings (used to port to other plattforms)


#endif
