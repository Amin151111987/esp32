#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "driver/gpio.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"

#include "sixxlowpan_test.h"
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

//Just a test main file, created by the BTStack task

char mqqt_host[] = "fe80::21a:7dff:feda:7113";
char local[] = "fe80::240a:c4ff:fe04:b132";
struct udp_pcb* upcb;
int port = 1883;
extern struct netif rfc7668_netif;
struct pbuf *pb;

int sendPacketBuffer(char* host, int port, unsigned char* buf, int buflen)
{
	struct sockaddr_in6 cliaddr;
	int rc = 0;
	
	pb = pbuf_alloc(PBUF_IP,buflen+UDP_HLEN,PBUF_RAM);
	pbuf_header(pb,UDP_HLEN);
	if(pbuf_take(pb,buf,buflen) != ERR_OK) printf("pbuf_take error\n");
	
	ip_addr_t dst_ip;
	dst_ip.type = IPADDR_TYPE_V6;
	ip6addr_aton(mqqt_host,&dst_ip.u_addr.ip6);
	ip6addr_aton(local,ip_2_ip6(&upcb->local_ip));
	ip6addr_aton(mqqt_host,ip_2_ip6(&upcb->remote_ip));
	
	upcb->remote_ip.type = IPADDR_TYPE_V6;
	upcb->local_ip.type = IPADDR_TYPE_V6;
	if(upcb == NULL) printf("UDP-PCB == null!\n");
	//netif_is_up
	rc = udp_sendto_if(upcb,pb,&dst_ip,port, &rfc7668_netif);
	
	printf("UDP return: %d\n",rc);
	pbuf_free(pb);
	return rc;
}

void receivePacket(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	//TODO: process MQTT-SN packets...
	printf("received on port %d, len: %d, tot_len: %d\n",port,p->len,p->tot_len);
	
	//necessary!!
	pbuf_free(p);
}

//TODO: send/receive for admin channel...

void user_task(void)
{
	struct hostent *hp;
	int level = 0;
	uint8_t test = 0;
    //esp_err_t ret;
    
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    
    int rc = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int mysock = 0;
	MQTTSN_topicid topic;
	unsigned char* payloadOn = (unsigned char*)"ON";
	unsigned char* payloadOff = (unsigned char*)"OFF";
	int payloadlenOn = strlen((char*)payloadOn);
	int payloadlenOff = strlen((char*)payloadOff);
	int len = 0, dup = 0, qos = 0, retained = 0, packetid = 0;
	char *host = "127.0.0.1";
	char *topicname = "/test";
	const ip_addr_t ipv6addrany = IPADDR_ANY_TYPE_INIT;
	
    upcb = udp_new();
    udp_bind(upcb, &ipv6addrany, 1883);
    udp_recv(upcb,receivePacket, NULL);
    
	while(1)
	{
		for(int i = 0; i<300; i++)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			//esp_task_wdt_feed();
		}
		
        gpio_set_level(GPIO_NUM_5, level);
        level = !level;	
        
        if((xEventGroupGetBits(lowpan_ble_flags) & LOWPAN_BLE_FLAG_CONNECTED))
        {   
			if(test == 0) { 
				vTaskDelay(3000 / portTICK_PERIOD_MS); 
				test++;
				
				MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
				printf("Sending to hostname %s port %d\n", mqqt_host, port);
				options.clientID.cstring = "myclientid";
				options.willFlag = 0;
				
				len = MQTTSNSerialize_connect(buf, buflen, &options);
				
				printf("MQTT-SN packet: ");
				for(int i = 0; i<len; i++) { printf("%2X ",buf[i]); }
				printf("\n");
				
				rc = sendPacketBuffer(mqqt_host, port, buf, len); //TODO, sendPacket	
			}
			
			//if normal is used: topic name must be mapped to topic ID!!!
			//otherwise: "Received reserved packet, which is forbidden: closing connection"
			//topic.type = MQTTSN_TOPIC_TYPE_NORMAL;
			
			//topic short: only 2 chars name allowed
			topic.type = MQTTSN_TOPIC_TYPE_SHORT;
			
			topic.data.long_.name = topicname;
			topic.data.long_.len = strlen(topicname);
			
			topic.data.short_name[0] = '3';
			topic.data.short_name[1] = '2';
			
			if(level)
			{
				printf("Stub: MQQT-SN ON\n");
				len = MQTTSNSerialize_publish(buf, buflen - len, dup, qos, retained, packetid,
						topic, payloadOn, payloadlenOn);
			} else {
				printf("Stub: MQQT-SN OFF\n");
				len = MQTTSNSerialize_publish(buf, buflen - len, dup, qos, retained, packetid,
						topic, payloadOff, payloadlenOff);
			}
			rc = sendPacketBuffer(mqqt_host, port, buf, len);
			
			
		}
	}
	
	
	rc = shutdown(mysock, SHUT_WR);
	rc = close(mysock);
}
