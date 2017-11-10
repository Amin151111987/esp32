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

#include "mqtt_sn_client.h"

/*#include "MQTTSNPacket.h"
#include "MQTTSNConnect.h"
#include "MQTTSNPublish.h"*/

//Just a test main file, created by the BTStack task


char local[] = "fe80::240a:c4ff:fe04:b132";
char mqtt_host[] = "fe80::21a:7dff:feda:7113";
char client[] = "ToRaDes_123";
extern struct netif rfc7668_netif;

err_t cb_command(mqtt_sn_pkt_t* topicData)
{
	printf("CB command: %s\n",topicData->payload);
	return ERR_OK;
}
err_t cb_rgb(mqtt_sn_pkt_t* topicData)
{
	printf("CB RGB: %s\n",topicData->payload);
	return ERR_OK;
}
err_t cb_brightness(mqtt_sn_pkt_t* topicData)
{
	printf("CB brightness: %s\n",topicData->payload);
	return ERR_OK;
}


//TODO: send/receive for admin channel...

void user_task(void)
{
	int level = 0;
	uint8_t test = 0;
    err_t ret;
    
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
	
	mqtt_sn_broker_cfg_t mqtt_config;
	mqtt_sn_client_cfg_t mqtt_config_client;
	mqtt_sn_topic_subscribe_cfg_t topicSubBrightness;
	mqtt_sn_topic_subscribe_cfg_t topicSubRGB;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommand;

	topicSubBrightness.topicID = 0;
	topicSubRGB.topicID = 0;
	topicSubCommand.topicID = 0;
	char topicRegisterBrightness[] = "/light/brightness";
	char topicRegisterRGB[] = "/light/rgb";
	char topicRegisterCommand[] = "/light/command";
	
	topicSubBrightness.topicName = topicRegisterBrightness;
	topicSubRGB.topicName = topicRegisterRGB;
	topicSubCommand.topicName = topicRegisterCommand;
	
	topicSubBrightness.cb = cb_brightness;
	topicSubRGB.cb = cb_rgb;
	topicSubCommand.cb = cb_command;

	while(1)
	{
		for(int i = 0; i<30; i++)
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
				
				strcpy(mqtt_config.gateway_addr,mqtt_host);
				mqtt_config.gateway_port = 1883;
				mqtt_config.local_port = 448;
				mqtt_config.interface = &rfc7668_netif;
				mqtt_config.ip_type = IPADDR_TYPE_V6;
				
				strcpy(mqtt_config_client.clientID,client);
				mqtt_config_client.willFlag = 0;
				
				
				mqtt_sn_client_init(&mqtt_config,&mqtt_config_client);
				
			}
			
			//wait until connected , max 2s; 
			if(mqtt_sn_client_wait_status(MQTT_SN_CLIENT_ACTIVE, 2000 / portTICK_PERIOD_MS) == MQTT_SN_CLIENT_ACTIVE)
			{
				mqtt_sn_pkt_t sendData;
				
				
				sendData.topicShort[0] = '4';
				sendData.topicShort[1] = '2';
				sendData.dup = 0;
				sendData.retained = 0;
				sendData.qos = 0;
				unsigned char data1[] = "ON";
				unsigned char data2[] = "OFF";
				uint8_t len1=2;
				uint8_t len2=3;
				
				
				//register for topic
				if(topicSubBrightness.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubBrightness);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering topic: %d",ret);
				} 
				
				//register for topic
				if(topicSubRGB.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubRGB);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering topic2: %d",ret);
				} 
				
				//register for topic
				if(topicSubCommand.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommand);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering topic3: %d",ret);
				} 
				
				
				if(level)
				{
					sendData.payload = data1;
					sendData.len = len1;
					mqtt_sn_client_publish_short(&sendData);
				} else {
					sendData.payload = data2;
					sendData.len = len2;
					mqtt_sn_client_publish_short(&sendData);
				}
			} else {
				printf("No CONNACK in 2s...");
			}
			

			
			
		}
	}
}
