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
  
#include "mqtt_sn_client.h"

#define MQTT_SN_CLIENT_TAG "[MQTT-SN-C] "


mqtt_sn_broker_cfg_t _config_broker;
mqtt_sn_client_cfg_t _config_client;

struct udp_pcb* mqtt_sn_client_upcb;
struct udp_pcb* mqtt_sn_client_upcbsend;

uint16_t topicIDTemp;
uint16_t msgID = 1;

struct subCallbacks
{
    uint16_t topicID; /* topic ID */
	mqtt_sn_topic_subscribe_cb cb; /* callback method */
    struct subCallbacks *next; /* next element */
};

struct subCallbacks *callbackList = NULL;


/** MQTT-SN client, current client state **/
EventGroupHandle_t mqtt_sn_client_state;

/** MQTT-SN client, current client state (internal), wait for something from the gateway **/
EventGroupHandle_t mqtt_sn_client_state_internal;

/** internal client state flags, wait for something */
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_CONNACK_WITHOUT_WILL (1<<0)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_WILLTOPICREQ (1<<1)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_WILLMSGREQ (1<<2)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_CONNACK (1<<3)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_REGACK (1<<4)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK (1<<5)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_SUBACK (1<<6)
#define MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_UNSUBACK (1<<7)

/** internal client state flags, something we waited for happened */
EventGroupHandle_t mqtt_sn_client_state_internal_acks;


/** Send a packet to the gateway (or any other UDP host)
 * 
 * This method sends a given data packet (buf + buflen) to a given
 * host at a given port.
 * 
 * If an interface is set while init, this interface is used.
 * Otherwise the first available interface will be used to send data.
 * 
 * @param host Character representation of the host (IP address!!!)
 * @param port Gateway port number
 * @param buf Data to be sent
 * @param buflen Size of the trasmitting data
 * 
 * @return ERR_OK if everything is fine, otherwise the UDP error messages are used
 * */
int mqtt_sn_client_send_packet(char* host, int port, unsigned char* buf, int buflen)
{
	int rc = 0;
	struct pbuf *pb;
	
	pb = pbuf_alloc(PBUF_IP,buflen+UDP_HLEN,PBUF_RAM);
	pbuf_header(pb,UDP_HLEN);
	if(pbuf_take(pb,buf,buflen) != ERR_OK) 
	{
		ESP_LOGE(MQTT_SN_CLIENT_TAG,"pbuf_take error, not sending message");
		return ERR_MEM;
	}
	
	if(mqtt_sn_client_upcbsend == NULL) 
	{
		ESP_LOGE(MQTT_SN_CLIENT_TAG,"UDP-PCB == null, not sending message");
		return ERR_VAL;
	}
	
	ip_addr_t dst_ip;
	dst_ip.type = IPADDR_TYPE_V6;
	ip6addr_aton(host,&dst_ip.u_addr.ip6);
	//ip6addr_aton(local,ip_2_ip6(&mqtt_sn_client_upcbsend->local_ip)); //TODO: works without local or not?!?
	ip6addr_aton(host,ip_2_ip6(&mqtt_sn_client_upcbsend->remote_ip));
	
	mqtt_sn_client_upcbsend->remote_ip.type = IPADDR_TYPE_V6;
	mqtt_sn_client_upcbsend->local_ip.type = IPADDR_TYPE_V6;
	
	//either send to a dedicated interface or just to the first one
	if(_config_broker.interface != NULL)
	{
		//if(netif_is_up() != ) ESP_LOGW(MQTT_SN_CLIENT_TAG, "Interface is not up, trying anyway...");
		rc = udp_sendto_if(mqtt_sn_client_upcbsend,pb,&dst_ip,port,_config_broker.interface);
	} else {
		rc = udp_sendto(mqtt_sn_client_upcbsend,pb,&dst_ip,port);
	}
	
	pbuf_free(pb);
	return rc;
}

void mqtt_sn_client_receive_connack(unsigned char *buf, int len)
{
	int rc;
	
	//deserialize, analyze returncode
	if(MQTTSNDeserialize_connack(&rc,buf,len))
	{
		if(rc != 0)
		{
			ESP_LOGW(MQTT_SN_CLIENT_TAG, "CONNACK return code: %d",rc);
			return;
		}	
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "CONNACK deserialize problem");
		return;
	}

	//are we waiting for CONNACK?
	if(xEventGroupGetBits(mqtt_sn_client_state) == MQTT_SN_CLIENT_CONNECTING)
	{
		ESP_LOGD(MQTT_SN_CLIENT_TAG, "Received CONNACK, connected now");
		xEventGroupClearBits(mqtt_sn_client_state, MQTT_SN_CLIENT_CONNECTING);
		xEventGroupSetBits(mqtt_sn_client_state, MQTT_SN_CLIENT_ACTIVE);
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Received CONNACK, not waiting for it? Flags: 0x%2X",mqtt_sn_client_get_status());
	}
	return; 
}

void mqtt_sn_client_receive_regack(unsigned char *buf, int len)
{
	unsigned char rc;
	unsigned short packetID;
	
	//deserialize, analyze returncode
			
	//currently unused, only one packet at once
	packetID = 0;
	if(MQTTSNDeserialize_regack(&topicIDTemp,&packetID, &rc, buf, len))
	{
		if(rc != 0)
		{
			ESP_LOGW(MQTT_SN_CLIENT_TAG, "REGACK return code: %d",rc);
			return;
		}	
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "REGACK deserialize problem");
		return;
	}

	//are we waiting for REGACK?
	if(xEventGroupGetBits(mqtt_sn_client_state_internal) & MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_REGACK)
	{
		ESP_LOGD(MQTT_SN_CLIENT_TAG, "Received REGACK, registered topic: [%d]",topicIDTemp);
		xEventGroupClearBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_REGACK);
		xEventGroupSetBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_REGACK_RECV);
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Received REGACK, not waiting for it? Flags: 0x%2X",mqtt_sn_client_get_status());
	}
	return; 
}


void mqtt_sn_client_receive_puback(unsigned char *buf, int len)
{
	unsigned char rc;
	unsigned short packetID;
	unsigned short topicID;
	
	//deserialize, analyze returncode
			
	//currently unused, only one packet at once
	packetID = 0;
	topicID = 0;
	if(MQTTSNDeserialize_puback(&topicID,&packetID, &rc, buf, len))
	{
		if(rc != 0)
		{
			ESP_LOGW(MQTT_SN_CLIENT_TAG, "PUBACK return code: %d",rc);
			return;
		}	
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "PUBACK deserialize problem");
		return;
	}

	//are we waiting for PUBACK?
	if(xEventGroupGetBits(mqtt_sn_client_state_internal) & MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK)
	{
		ESP_LOGD(MQTT_SN_CLIENT_TAG, "Received PUBACK, registered topic: [%d]",topicID);
		xEventGroupClearBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK);
		xEventGroupSetBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_PUBACK_RECV);
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Received PUBACK, not waiting for it? Flags: 0x%2X",mqtt_sn_client_get_status());
	}
	return;  
}


void mqtt_sn_client_receive_publish(unsigned char *buf, int len)
{
	struct subCallbacks *tempList = callbackList;
	mqtt_sn_pkt_t pkt;
	MQTTSN_topicid topic;
	unsigned short packetID;
	
	
	if(!MQTTSNDeserialize_publish(&pkt.dup,&pkt.qos, &pkt.retained, &packetID, &topic, &pkt.payload, &pkt.len, buf, len))
	{
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Receive publish deserialize problem");
		return;
	}
	
	//search linked list for topicID
	for( ; tempList != NULL ; tempList = tempList->next )
    {
		//is this the right id?
		if(tempList->topicID == topic.data.id)
		{
			//save id to new packet
			pkt.topicID = topic.data.id;
			
			//set '\0' character at the end
			pkt.payload[pkt.len] = '\0';
			
			ESP_LOGD(MQTT_SN_CLIENT_TAG, "Receive data, found callback");
			//call cb method
			tempList->cb(&pkt);
			return;
		}
    }
    
    ESP_LOGW(MQTT_SN_CLIENT_TAG, "No topicID found for subscribed topic (data receive)");
	return; 
}


void mqtt_sn_client_receive_suback(unsigned char *buf, int len)
{
	unsigned char rc;
	unsigned short packetID;
	unsigned short topicID;
	int qos;
	
	//deserialize, analyze returncode
			
	//currently unused, only one packet at once
	packetID = 0;
	topicID = 0;
	if(MQTTSNDeserialize_suback(&qos,&topicID, &packetID, &rc, buf, len))
	{
		if(rc != 0)
		{
			ESP_LOGW(MQTT_SN_CLIENT_TAG, "SUBACK return code: %d",rc);
			return;
		}	
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "SUBACK deserialize problem");
		return;
	}

	//are we waiting for SUBACK?
	if(xEventGroupGetBits(mqtt_sn_client_state_internal) & MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_SUBACK)
	{
		ESP_LOGD(MQTT_SN_CLIENT_TAG, "Received SUBACK, registered topic: [%d]",topicID);
		topicIDTemp = topicID;
		xEventGroupClearBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_SUBACK);
		xEventGroupSetBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_SUBACK_RECV);
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Received SUBACK, not waiting for it? Flags: 0x%2X",mqtt_sn_client_get_status());
	}
	return;
}

void mqtt_sn_client_receive_unsuback(unsigned char *buf, int len)
{
	unsigned short packetID;
	
	//deserialize, analyze returncode
			
	//currently unused, only one packet at once
	packetID = 0;

	if(!MQTTSNDeserialize_unsuback(&packetID, buf, len))
	{
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "UNSUBACK deserialize problem");
		return;
	}

	//are we waiting for UNSUBACK?
	if(xEventGroupGetBits(mqtt_sn_client_state_internal) & MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_UNSUBACK)
	{
		ESP_LOGD(MQTT_SN_CLIENT_TAG, "Received UNSUBACK");
		xEventGroupClearBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_UNSUBACK);
		xEventGroupSetBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_UNSUBACK_RECV);
	} else {
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Received UNSUBACK, not waiting for it? Flags: 0x%2X",mqtt_sn_client_get_status());
	}
	return; 
}



void mqtt_sn_client_receive_packet(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	ESP_LOGD(MQTT_SN_CLIENT_TAG, "received on port %d, len: %d, tot_len: %d",port,p->len,p->tot_len);
	int datalen = 0;
	int msgTypeOffset = 0;
	unsigned char *buf = p->payload;
	
	msgTypeOffset = MQTTSNPacket_decode(buf, p->len, &datalen);

	if (datalen != p->len)
	{
		ESP_LOGW(MQTT_SN_CLIENT_TAG, "unparsable message!");
		goto exit; /* there was an error */
	}
		
	ESP_LOGD(MQTT_SN_CLIENT_TAG, "incoming msg type: %d",buf[msgTypeOffset]);

	//determine MQTT-SN packet type
	switch(buf[msgTypeOffset])
	{
		case MQTTSN_CONNACK:
			mqtt_sn_client_receive_connack(buf,p->len);
		break;
		
		case MQTTSN_REGACK:
			mqtt_sn_client_receive_regack(buf,p->len);
		break;
		
		case MQTTSN_PUBACK:
			mqtt_sn_client_receive_puback(buf,p->len);
		break;
		
		case MQTTSN_SUBACK:
			mqtt_sn_client_receive_suback(buf,p->len);
		break;
		
		case MQTTSN_UNSUBACK:
			mqtt_sn_client_receive_suback(buf,p->len);
		break;
		
		case MQTTSN_PUBLISH:
			mqtt_sn_client_receive_publish(buf,p->len);
		break;
		
		default:
			ESP_LOGW(MQTT_SN_CLIENT_TAG, "Unknown/unimplemented MQTT-SN message received: %d, %s", buf[msgTypeOffset], MQTTSNPacket_name(buf[msgTypeOffset]));
			break;
	}
	
	exit:
	//necessary!!
	pbuf_free(p);
}

void append(struct subCallbacks **lst, uint16_t topicid, mqtt_sn_topic_subscribe_cb cb)
{
    struct subCallbacks *newElement;
    /* go to end of list */
    while( *lst != NULL ) 
    {
       lst = &(*lst)->next;
    }

    newElement = malloc(sizeof(*newElement)); /* create new element */
    newElement->topicID = topicid;
    newElement->cb = cb;
    newElement->next = NULL; /* mark end of list */
    *lst = newElement;
    
    ESP_LOGD(MQTT_SN_CLIENT_TAG,"Added new subscribe callback to list (id=%d)",topicid);
}

/** Add a subscription to a specific topic, with callbacks
 * This method subscribes to a topic at the broker. Incoming data
 * is passed to the callback method, provided with the configuration.
 * All available options are described at @see mqtt_sn_topic_susbcribe_cfg_t
 * 
 * @param config Subscription configuration
 */
err_t mqtt_sn_client_add_subscribe_topic(mqtt_sn_topic_subscribe_cfg_t *config)
{
	unsigned short topicID = 0;
	err_t rc;
	MQTTSN_topicid topic;
	unsigned char *buf;
	EventBits_t evtAck;
	int buflen, len;

	//send subscribe
	//alloc length of payload, add 10Bytes overhead (header)
	buf = malloc(16+strlen(config->topicName));
	if(buf == NULL) return ERR_MEM;
	buflen = 16 + strlen(config->topicName);
	
	topic.data.long_.name = config->topicName;
	topic.data.long_.len = strlen(config->topicName);
	
	len = MQTTSNSerialize_subscribe(buf, buflen, 0,0, msgID++, &topic);
	rc = mqtt_sn_client_send_packet(_config_broker.gateway_addr, _config_broker.gateway_port, buf, len);
	
	//wait for the suback receive
	xEventGroupSetBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_SUBACK);
	
	free(buf);
	
	if(rc != ERR_OK) 
	{
		ESP_LOGE(MQTT_SN_CLIENT_TAG,"Subscribe for topic failed: %d",rc);
		return rc;
	}
	
	//wait for suback
	evtAck = xEventGroupWaitBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_SUBACK_RECV, \
		pdTRUE, pdFALSE, MQTT_SN_TICKS_TO_WAIT);
	if(!(evtAck & MQTT_SN_CLIENT_STATE_SUBACK_RECV)) return ERR_TIMEOUT;
	
	//add to linkedlist
	append(&callbackList, topicIDTemp, config->cb);
	config->topicID = topicIDTemp;
	
	return ERR_OK;
}


/** Publish data, use short topic name (2 characters!)
 * This method publishes a given MQTT-SN packet ( @see mqtt_sn_publish_pkt_t )
 * to a topic, addressed via the short topic name. This is usually faster than
 * a topic name to topic id mapped publishing, but it is limited to a 2 character
 * topic name.
 * 
 * @param packet Transmitted packet
 */
err_t mqtt_sn_client_publish_short(mqtt_sn_pkt_t *packet)
{
	if(packet == NULL) return ERR_VAL;
	
	EventBits_t currentstatewait = xEventGroupGetBits(mqtt_sn_client_state_internal);
	MQTTSN_topicid topic;
	unsigned char *buf;
	err_t rc;
	int buflen, len = 0;
	EventBits_t evtAck;
	
	if(currentstatewait & MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK)
	{
		return ERR_INPROGRESS;
	}
	
	//alloc length of payload, add 10Bytes overhead (header)
	buf = malloc(packet->len + 10);
	if(buf == NULL) return ERR_MEM;
	buflen = packet->len + 10;
	
	topic.type = MQTTSN_TOPIC_TYPE_SHORT;
	topic.data.short_name[0] = packet->topicShort[0];
	topic.data.short_name[1] = packet->topicShort[1];
	
		
	if(packet->qos > 0)
	{
		xEventGroupSetBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK);
	}
	
	len = MQTTSNSerialize_publish(buf, buflen, packet->dup, packet->qos, packet->retained, 0,
						topic, packet->payload, packet->len);
	rc = mqtt_sn_client_send_packet(_config_broker.gateway_addr, _config_broker.gateway_port, buf, len);
	free(buf);
	
	
	//Wait for PUBACK if QoS > 0
	if(packet->qos > 0)
	{
		//wait for the PUBACK flag
		evtAck = xEventGroupWaitBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_PUBACK_RECV, \
			pdTRUE, pdFALSE, MQTT_SN_TICKS_TO_WAIT);
		if(!(evtAck & MQTT_SN_CLIENT_STATE_PUBACK_RECV)) return ERR_TIMEOUT;
	}
	
	return rc;
}


/** Publish data, use topic id (already registered)
 * This method publishes a given MQTT-SN packet ( @see mqtt_sn_publish_pkt_t )
 * to a topic, addressed via a previously registered topic id 
 * ( @see mqtt_sn_client_add_publish_topic ).
 * Also very fast (similar to publish to a short topic), but it is necessary
 * to register the topic in advance.
 * Recommended for regular publishing.
 * 
 * @param packet Transmitted packet
 * 
 * @return ERR_INPROGRESS if there is a pending transmission or waiting for PUBACK
 * ERR_OK if everything is fine
 */
err_t mqtt_sn_client_publish_registered(mqtt_sn_pkt_t *packet)
{	
	
	if(packet == NULL) return ERR_VAL;
	
	EventBits_t currentstatewait = xEventGroupGetBits(mqtt_sn_client_state_internal);
	
	MQTTSN_topicid topic;
	err_t rc;
	unsigned char *buf;
	int buflen, len = 0, packetid = 0;
	EventBits_t evtAck;
	
	if(currentstatewait & MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK)
	{
		return ERR_INPROGRESS;
	}
	
	//alloc length of payload, add 10Bytes overhead (header)
	buf = malloc(packet->len + 10);
	if(buf == NULL) return ERR_MEM;
	buflen = packet->len + 10;
	
	//fill topic:
	topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
	topic.data.id = packet->topicID;
	
	
	len = MQTTSNSerialize_publish(buf, buflen, packet->dup, packet->qos, 
		packet->retained, packetid, topic, packet->payload, packet->len);
	
	if(packet->qos > 0)
	{
		xEventGroupSetBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK);
	}
	
	//#define ERR_OK          0    /* No error, everything OK. */
	//#define ERR_MEM        -1    /* Out of memory error.     */
	//#define ERR_BUF        -2    /* Buffer error.            */
	//#define ERR_TIMEOUT    -3    /* Timeout.                 */
	//#define ERR_RTE        -4    /* Routing problem.         */
	//#define ERR_INPROGRESS -5    /* Operation in progress    */
	//#define ERR_VAL        -6    /* Illegal value.           */
	//#define ERR_WOULDBLOCK -7    /* Operation would block.   */
	//#define ERR_USE        -8    /* Address in use.          */
	rc = mqtt_sn_client_send_packet(_config_broker.gateway_addr, _config_broker.gateway_port, buf, len);
	free(buf);
	
	//Wait for PUBACK if QoS > 0
	if(packet->qos > 0)
	{
		//wait for the PUBACK flag
		evtAck = xEventGroupWaitBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_PUBACK_RECV, \
			pdTRUE, pdFALSE, MQTT_SN_TICKS_TO_WAIT);
		if(!(evtAck & MQTT_SN_CLIENT_STATE_PUBACK_RECV)) return ERR_TIMEOUT;
	}
	
	return rc;
}

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
err_t mqtt_sn_client_publish_unregistered(mqtt_sn_pkt_t *packet, char *topicName)
{
	err_t rc;
	mqtt_sn_topic_register_cfg_t pubCfg;
	
	pubCfg.topicName = topicName;
	
	//first request a topicId for this name
	rc =  mqtt_sn_client_register_topic(&pubCfg);
	if(rc != ERR_OK) return rc;
	
	//map topic id to packet
	packet->topicID = pubCfg.topicID;
	
	//send as registered
	return mqtt_sn_client_publish_registered(packet);
}

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
err_t mqtt_sn_client_register_topic(mqtt_sn_topic_register_cfg_t *config)
{
		
	MQTTSNString topicName;
	err_t rc;
	unsigned char *buf;
	int buflen, len = 0;
	EventBits_t evtAck;
	
	//alloc length of payload, add 10Bytes overhead (header)
	buf = malloc(strlen(config->topicName) + 10);
	if(buf == NULL) return ERR_MEM;
	buflen = strlen(config->topicName) + 10;
	
	topicName.cstring = config->topicName;
	
	len	= MQTTSNSerialize_register(buf, buflen, 0, 0, &topicName);
	
	xEventGroupSetBits(mqtt_sn_client_state_internal, MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_REGACK);
	
	rc = mqtt_sn_client_send_packet(_config_broker.gateway_addr, _config_broker.gateway_port, buf, len);
	free(buf);
	if(rc != ERR_OK) return rc;
	
	//wait for the REGACK flag
	evtAck = xEventGroupWaitBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_REGACK_RECV, \
		pdTRUE, pdFALSE, MQTT_SN_TICKS_TO_WAIT);
	if(!(evtAck & MQTT_SN_CLIENT_STATE_REGACK_RECV)) return ERR_TIMEOUT;
	
	
	//and save topic ID
	config->topicID = topicIDTemp;
	return rc;
}


/** Wait for a client flags
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
uint8_t mqtt_sn_client_wait_status(const EventBits_t state, TickType_t ticksToWait)
{
	return xEventGroupWaitBits(mqtt_sn_client_state, state, pdFALSE,pdFALSE, ticksToWait);
}


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
uint8_t mqtt_sn_client_get_status()
{
	return xEventGroupGetBits(mqtt_sn_client_state);
}


/** Wait for a gateway PUBACK
 *
 * This method blocks until the gateway returns PUBACK for publishing
 * with a QoS > 0.
 * Similar usage as @see xEventGroupWaitBits, but only with ticks to wait.
 * 
 * @see MQTT_SN_CLIENT_STATE_PUBACK_RECV
 * @return MQTT-SN client ACK flags at the time of the unblock
 */
uint8_t mqtt_sn_client_wait_puback(TickType_t ticksToWait)
{
	return xEventGroupWaitBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_PUBACK_RECV, pdTRUE, pdFALSE, ticksToWait);
}

/** Initialise the MQTT-SN client
 * This method initialises the MQTT-SN client and connects to the gateway
 * For all full list of configuration options see @see mqtt_sn_broker_cfg_t
 * Must be called prior to all publish/subscribe actions.

 * @param config Provides a broker & network interface configuration
 */
err_t mqtt_sn_client_init(mqtt_sn_broker_cfg_t *config, mqtt_sn_client_cfg_t *client_cfg)
{
	err_t retVal;
	const ip_addr_t ipv6addrany = IPADDR_ANY_TYPE_INIT;
	
	//init current state event group (flags), external visible via mqtt_sn_client_get_status()
	mqtt_sn_client_state = xEventGroupCreate();
	xEventGroupClearBits(mqtt_sn_client_state,MQTT_SN_CLIENT_ACTIVE | MQTT_SN_CLIENT_ASLEEP | \
		MQTT_SN_CLIENT_AWAKE | MQTT_SN_CLIENT_CONNECTING);
	xEventGroupSetBits(mqtt_sn_client_state, MQTT_SN_CLIENT_IDLE);
	
	//init internal state event group
	mqtt_sn_client_state_internal = xEventGroupCreate();
	xEventGroupClearBits(mqtt_sn_client_state,MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_CONNACK | MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_CONNACK_WITHOUT_WILL | \
		MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_WILLMSGREQ |  MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_WILLTOPICREQ | \
		MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_REGACK | MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_PUBACK | \
		MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_SUBACK | MQTT_SN_CLIENT_CONNSTATE_WAIT_FOR_UNSUBACK );
		
	//init internal state ack event group
	mqtt_sn_client_state_internal_acks = xEventGroupCreate();
	xEventGroupClearBits(mqtt_sn_client_state_internal_acks, MQTT_SN_CLIENT_STATE_REGACK_RECV | MQTT_SN_CLIENT_STATE_PUBACK_RECV | \
		MQTT_SN_CLIENT_STATE_SUBACK_RECV | MQTT_SN_CLIENT_STATE_UNSUBACK_RECV);
	
	//create UDP socket, depending on config, see mqtt_sn_broker_cfg_t
	mqtt_sn_client_upcb = udp_new();
	mqtt_sn_client_upcb->local_ip = ipv6addrany;
	//IP_ADDR6(mqtt_sn_client_upcb->local_ip, 0,0,0,0);
	mqtt_sn_client_upcbsend = udp_new();

	if(config->ip_type != IPADDR_TYPE_V6)
	{
		ESP_LOGE(MQTT_SN_CLIENT_TAG, "Currently only IPv6 support");
		return ERR_VAL;
	}
	
	if(config->local_port != 0)
	{
		//retVal = udp_bind(mqtt_sn_client_upcb, &ipv6addrany, config->local_port);
		retVal = udp_bind(mqtt_sn_client_upcb, &ipv6addrany, config->local_port);
		if(retVal != ERR_OK) {
			ESP_LOGE(MQTT_SN_CLIENT_TAG, "Error binding to UDP port: %d, error: %d", config->local_port,retVal);
			return retVal;
		}
    } else {
		retVal = udp_bind(mqtt_sn_client_upcb, &ipv6addrany, 1883);
		if(retVal != ERR_OK) {
			ESP_LOGE(MQTT_SN_CLIENT_TAG, "Error binding to UDP port: 1883 (default), error: %d",retVal);
			return retVal;
		}
	}
	//set receive callback
    udp_recv(mqtt_sn_client_upcb,mqtt_sn_client_receive_packet, NULL);
    
    //save config to local (all necessary parameters are checked at this point)
	memcpy(&_config_broker,config,sizeof(_config_broker));
	memcpy(&_config_client,client_cfg,sizeof(_config_client));
	
	ESP_LOGI(MQTT_SN_CLIENT_TAG,"UDP socket created, connecting to gateway");
    
    //and connect
    
    //set CONNECT options
    MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
    //memcpy(options.clientID.cstring, client_cfg.clientID, sizeof(client_cfg.clientID));
    options.clientID.cstring = _config_client.clientID;
    options.willFlag = _config_client.willFlag;
    options.cleansession = _config_client.cleansession;
    options.duration = _config_client.duration;
	//send packet (fixed size MQTT buffer, max for CONNECT is 30Bytes)
	uint8_t buf[30];
	uint8_t len = MQTTSNSerialize_connect(buf, 30, &options);
	
	xEventGroupSetBits(mqtt_sn_client_state, MQTT_SN_CLIENT_CONNECTING);
	xEventGroupClearBits(mqtt_sn_client_state, MQTT_SN_CLIENT_IDLE);
	
	return mqtt_sn_client_send_packet(_config_broker.gateway_addr, _config_broker.gateway_port, buf, len);
}
