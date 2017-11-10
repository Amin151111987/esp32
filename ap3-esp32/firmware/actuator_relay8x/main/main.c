#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"


#include "btstack_rfc7668.h"
#include "mqtt_sn_client.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

//TODOs:
//implement config authentication & tracking
//implement further config via MQTT (pairing, topics,...)

/* used to set either networking mode == 0 or debug the hardware == 1
 * which set/resets all relays with a 2s delay between each switching */
#define DEBUG_HW	0

/** define the size of each relay's task command (event or configuration)
 * queue */
#define RELAY_TASK_COMMANDS 5

/** this flag is set, if an event for a relay's task is received */
#define FLAG_RELAY_EVENT 		(1<<0)
/** this flag is set, if config data for a relay's task is received */
#define FLAG_RELAY_CONFIG 		(1<<1)
/** this flag is set, if a timer for a relay's task is expired */
#define FLAG_RELAY_TIMER 		(1<<2)
/** this flag is set, if the main task tells a relay task to free
 * its resources to be deleted by the main task */
#define FLAG_RELAY_DELETE_TASK 	(1<<3)
/** this flag is set, if a task which got @see FLAG_RELAY_DELETE_TASK
 * has freed its resources */
#define FLAG_RELAY_TASK_FREED	(1<<4)

// relay (direct ones) GPIO pin selection
#define GPIO_RELAY_G	14
#define GPIO_RELAY_H	12

// shift-register controlled relay outputs
/** serial data input */
#define GPIO_SR_SER		32
/** latch clock (rising edge active) */
#define GPIO_SR_RCLK	33
/** output enable (active low) */
#define GPIO_SR_G		25
/** serial clock (rising edge active) */
#define GPIO_SR_SRCLK	26
/** serial clear (active low, clears ONLY SR) */
#define GPIO_SR_SRCLR	27

char topicRegisterCommandA[] = "/light/switch/A";
char topicRegisterCommandB[] = "/light/switch/B";
char topicRegisterCommandC[] = "/light/switch/C";
char topicRegisterCommandD[] = "/light/switch/D";
char topicRegisterCommandE[] = "/light/switch/E";
char topicRegisterCommandF[] = "/light/switch/F";
char topicRegisterCommandG[] = "/light/switch/G";
char topicRegisterCommandH[] = "/light/switch/H";
char topicRegisterConfig[] = "/tsh/config/actuators";

/** event group for relay A, task management */
EventBits_t* flagsA;
/** event group for relay B, task management */
EventBits_t* flagsB;
/** event group for relay C, task management */
EventBits_t* flagsC;
/** event group for relay D, task management */
EventBits_t* flagsD;
/** event group for relay E, task management */
EventBits_t* flagsE;
/** event group for relay F, task management */
EventBits_t* flagsF;
/** event group for relay G, task management */
EventBits_t* flagsG;
/** event group for relay H, task management */
EventBits_t* flagsH;

/** queue for relay A, containing either commands/events or config data
 * @see flagsA
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayA;
/** queue for relay B, containing either commands/events or config data
 * @see flagsB
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayB;
/** queue for relay C, containing either commands/events or config data
 * @see flagsC
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayC;
/** queue for relay D, containing either commands/events or config data
 * @see flagsD
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayD;
/** queue for relay E, containing either commands/events or config data
 * @see flagsE
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayE;
/** queue for relay F, containing either commands/events or config data
 * @see flagsF
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayF;
/** queue for relay G, containing either commands/events or config data
 * @see flagsG
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayG;
/** queue for relay H, containing either commands/events or config data
 * @see flagsH
 * @see FLAG_RELAY_CONFIG
 * @see FLAG_RELAY_EVENT */
QueueHandle_t queueRelayH;

mqtt_sn_topic_register_cfg_t topicStateA = 
{
	.topicName = "/light/status/A",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateB = 
{
	.topicName = "/light/status/B",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateC = 
{
	.topicName = "/light/status/C",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateD = 
{
	.topicName = "/light/status/D",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateE = 
{
	.topicName = "/light/status/E",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateF = 
{
	.topicName = "/light/status/F",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateG = 
{
	.topicName = "/light/status/G",
	.topicID = 0
};

mqtt_sn_topic_register_cfg_t topicStateH = 
{
	.topicName = "/light/status/H",
	.topicID = 0
};

//buffers for local & remote IPv6 adresses (string notation)
char local[40];
char mqtt_host[40];
//client identification
char client[] = "ToRaDes_Relay8X";

//reference to the RFC7668 netif
extern struct netif rfc7668_netif;

/** Send one relay output pattern
 * 
 * This method is used to send the output pattern for one relay:
 * IDLE: 0b1111
 * RESET: 0b0011
 * SET: 0b1100
 * LSB is sent first!
 * 
 * @param val output pattern
 * */
void sendRS(uint8_t val)
{
	uint8_t i = 0;
	
	for(i = 0; i<=3 ; i++)
	{
		//set bit at serial data
		gpio_set_level(GPIO_SR_SER, (val > i) & 0x01);
		ets_delay_us(1);
		//toggle shift register clock
		gpio_set_level(GPIO_SR_SRCLK,1);
		ets_delay_us(1);
		gpio_set_level(GPIO_SR_SRCLK,0);
	}
}

/** Latch current shift register output to relays
 * 
 * If this method is called, the current value of the shift
 * register is latched to the output.
 * */
void sendLatch(void)
{
	//toggle latch clock
	gpio_set_level(GPIO_SR_RCLK,1);
	ets_delay_us(1);
	gpio_set_level(GPIO_SR_RCLK,0);
}
/** update one relay
 * 
 * enables/disables one relay of the 8 available.
 * Relays 0-5 are set/reset relays, which are operated by H-bridges
 * (one coil). Many pins are used, so these relays are controlled
 * via shift-registers (74HC595, 3 times).
 * Relays 6 and 7 are "normal" relays, operated by MOSFETS (active high)
 * 
 * Example to set/reset one relay:
 * updateRelay(relayNumber,0);
 * //wait for setup time for relays (max 15ms initially)
 * ets_delay_us(16500);
 * //set outputs for R/S relays to idle
 * updateRelay(-1,0);
 * 
 * @param output output number (0-7 for relays; -1 for R/S idle)
 * @param value 0 switch of the relay, switch on otherwise. Ignored for output==-1
 * */
err_t updateRelay(int8_t output, uint8_t value)
{
	//pattern output counter before the selected relay
	uint8_t cntBefore = 0;
	//pattern output counter after the selected relay
	uint8_t cntAfter = 0;
	//general counter
	uint8_t i = 0;
	
	if(output > 7) return ERR_VAL;
	if(output < -1) return ERR_VAL;
	switch(output)
	{
		//set idle for all set/reset relays
		case -1:
			//send 6x idle
			for(i = 0;i<=5;i++) sendRS(0b1111);
			
			//latch the command to the relays
			sendLatch();
		break;
		
		//relay output "G" (directly controlled)
		case 6:
			gpio_set_level(GPIO_RELAY_G,value);
		break;
		
		//relay output "H" (directly controlled)
		case 7:
			gpio_set_level(GPIO_RELAY_H,value);
		break;
		
		//relay output "A" to "F" (controlled via shift-registers):
		//3 units -> 24 outputs
		//GPIO_SR_SER -> serial data input
		//GPIO_SR_SRCLK -> serial clock (rising edge active)
		//GPIO_SR_SRCLR -> serial clear (active low, clears ONLY SR)
		//GPIO_SR_RCLK -> latch clock (rising edge active)
		//GPIO_SR_G -> output enable (active low)
		//assignment:
		//order for each relay: xG1H xG1L xG2H xG2L
		//|	SR 1    |	SR 2  | SR 3	|
		//| R2 - R1 | R4 - R3 | R6 - R5 |
		//LSB						  MSB
		//patterns:
		//reset: 0b0011
		//set: 0b1100
		//idle: 0b1111
		
		//one of the set/reset relays
		default:
			//odd number (1-5; R2-R6)
			//IDLE: <6 - relay number> before
			//R/S byte
			//IDLE: <relay number - 1> after
			
			//even number (0-4; R1-R5)
			//IDLE: <6 - relaynumber - 2> before
			//R/S byte
			//IDLE: <relay number +1> after
			
			//is the output number even?
			if((output % 2) == 0)
			{
				cntBefore = 6 - output - 2;
				cntAfter = output + 1;
			} else {
				cntBefore = 6 - output;
				cntAfter = output - 1;
			}
			
			//reduce EMV
			gpio_set_level(GPIO_SR_SRCLR,0);
			ets_delay_us(1);
			gpio_set_level(GPIO_SR_SRCLR,1);
			
			//send idle pattern before
			for(i = 0;i<=cntBefore;i++) sendRS(0b1111);
			//send set/reset pattern for selected relay
			if(value)
			{
				sendRS(0b1100);
			} else {
				sendRS(0b0011);
			}
			//send idle pattern after
			for(i = 0;i<=cntAfter;i++) sendRS(0b1111);
			
			//latch the command to the relays
			sendLatch();
			
		break;
	}
	return ERR_OK;
}

err_t cb_command(unsigned char * topicData, uint8_t relay, uint8_t type)
{
	mqtt_sn_pkt_t ack;
	QueueHandle_t queueToPost;
	EventGroupHandle_t flagGroupToSet;
	err_t rc;
	
	ack.dup = 0;
	ack.retained = 0;
	ack.qos = 0;
	
	ESP_LOGD("MAIN","Received: ");
	ESP_LOGD("MAIN","%s",topicData);
	
	if(relay > 7) return ERR_VAL;

	//select the corresponding queue for data, topic for ACK reply and flag group
	switch(relay)
	{
		case 0: queueToPost = queueRelayA; ack.topicID = topicStateA.topicID; flagGroupToSet = flagsA; break;
		case 1: queueToPost = queueRelayB; ack.topicID = topicStateB.topicID; flagGroupToSet = flagsB; break;
		case 2: queueToPost = queueRelayC; ack.topicID = topicStateC.topicID; flagGroupToSet = flagsC; break;
		case 3: queueToPost = queueRelayD; ack.topicID = topicStateD.topicID; flagGroupToSet = flagsD; break;
		case 4: queueToPost = queueRelayE; ack.topicID = topicStateE.topicID; flagGroupToSet = flagsE; break;
		case 5: queueToPost = queueRelayF; ack.topicID = topicStateF.topicID; flagGroupToSet = flagsF; break;
		case 6: queueToPost = queueRelayG; ack.topicID = topicStateG.topicID; flagGroupToSet = flagsG; break;
		case 7: queueToPost = queueRelayH; ack.topicID = topicStateH.topicID; flagGroupToSet = flagsH; break;
		default:
			return ERR_VAL;
	}
	
	if(xQueueSend(queueToPost,(void *)topicData,0) != pdPASS)
	{
		ESP_LOGE("MAIN","Sending command to task %d failed, full queue",relay);
    }
     
    //signalize type of data which is sent to the task
	switch(type)
	{
		//type: data update (event)
		case 0: xEventGroupSetBits(flagGroupToSet,FLAG_RELAY_EVENT); break;
		case 1: xEventGroupSetBits(flagGroupToSet,FLAG_RELAY_CONFIG); break;
		default:
			ESP_LOGE("MAIN","Unknown type of event to send to relay queue");
		break;
	}
			
	//send ACK to host	
	ack.payload = topicData;
	ack.len = strlen((char*)topicData);
	rc = mqtt_sn_client_publish_registered(&ack);
		
	if(rc!=ERR_OK) ESP_LOGE("MAIN","ACK state failed: %d",rc);
	return ERR_OK;
}

err_t cb_commandA(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 0, 0); };
err_t cb_commandB(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 1, 0); };
err_t cb_commandC(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 2, 0); };
err_t cb_commandD(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 3, 0); };
err_t cb_commandE(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 4, 0); };
err_t cb_commandF(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 5, 0); };
err_t cb_commandG(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 6, 0); };
err_t cb_commandH(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 7, 0); };

err_t cb_configA(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 0, 1); };
err_t cb_configB(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 1, 1); };
err_t cb_configC(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 2, 1); };
err_t cb_configD(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 3, 1); };
err_t cb_configE(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 4, 1); };
err_t cb_configF(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 5, 1); };
err_t cb_configG(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 6, 1); };
err_t cb_configH(mqtt_sn_pkt_t* topicData) { return cb_command(topicData->payload, 7, 1); };

err_t cb_config(mqtt_sn_pkt_t* topicData)
{
	//TBD: use config only if this device is addressed (via BT MAC)
	//TBD: pairing & authentication
	//TBD: authenticate config update...

	//TBD: general settings for relays?
	//first: select mode
	// then send additional config data to the task
	
	char *configPart;
	char sep[] = ";";
	uint8_t i = 0;
	
	configPart = strtok((char*)topicData->payload, sep);
	
	//1.) MAC address
	if(configPart != NULL)
	{
		//TBD: test if this is my MAC...
		
		configPart = strtok(NULL,sep);
	} else {
		ESP_LOGE("MAIN","invalid config, MAC address missing");
		return ERR_VAL;
	}
	
	//2.) Output 1-4
	for(i=0; i<=3; i++)
	{
		if(configPart != NULL)
		{
			configPart = strtok(NULL,sep);
		} else {
			ESP_LOGE("MAIN","invalid config, output %d missing",i+1);
			return ERR_VAL;
		}
	}
	
	//3.) output mode
	if(configPart != NULL)
	{
		configPart = strtok(NULL,sep);
	} else {
		ESP_LOGE("MAIN","invalid config, mode missing");
		return ERR_VAL;
	}
	
	//4.) fade time
	if(configPart != NULL)
	{
		configPart = strtok(NULL,sep);
	}

	return ERR_OK;
}

void hw_configuration()
{
	uint8_t i = 0;
	
	//relay 6 and relay 7 (G and H)
	gpio_pad_select_gpio(GPIO_RELAY_G);
    gpio_set_direction(GPIO_RELAY_G, GPIO_MODE_OUTPUT);
    
	gpio_pad_select_gpio(GPIO_RELAY_H);
    gpio_set_direction(GPIO_RELAY_H, GPIO_MODE_OUTPUT);
    
    //relay 0 to 5 (A to F); shift register connected!
	gpio_pad_select_gpio(GPIO_SR_SER);
    gpio_set_direction(GPIO_SR_SER, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(GPIO_SR_RCLK);
    gpio_set_direction(GPIO_SR_RCLK, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(GPIO_SR_G);
    gpio_set_direction(GPIO_SR_G, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(GPIO_SR_SRCLK);
    gpio_set_direction(GPIO_SR_SRCLK, GPIO_MODE_OUTPUT);
	gpio_pad_select_gpio(GPIO_SR_SRCLR);
    gpio_set_direction(GPIO_SR_SRCLR, GPIO_MODE_OUTPUT);
    
    gpio_set_level(GPIO_SR_SER,0);
    gpio_set_level(GPIO_SR_RCLK,0);
    gpio_set_level(GPIO_SR_G,1);
    gpio_set_level(GPIO_SR_SRCLK,0);
    gpio_set_level(GPIO_SR_SRCLR,1);
    
    //clear all relays
    for(i=0; i<=7; i++) updateRelay(i,0);
    //wait for setup time for relays (max 15ms initially)
    ets_delay_us(16500);
    //set outputs for R/S relays to idle
    updateRelay(-1,0);
}

void user_task(void)
{
	int level = 0;
	uint8_t test = 0;
    err_t ret;
    //uint8_t buf[BD_ADDR_LEN];
    
    hw_configuration();
	
	mqtt_sn_broker_cfg_t mqtt_config;
	mqtt_sn_client_cfg_t mqtt_config_client;

	mqtt_sn_topic_subscribe_cfg_t topicSubCommandA;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandB;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandC;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandD;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandE;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandF;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandG;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommandH;
	mqtt_sn_topic_subscribe_cfg_t topicSubConfig;

	topicSubCommandA.topicID = 0;
	topicSubCommandB.topicID = 0;
	topicSubCommandC.topicID = 0;
	topicSubCommandD.topicID = 0;
	topicSubCommandE.topicID = 0;
	topicSubCommandF.topicID = 0;
	topicSubCommandG.topicID = 0;
	topicSubCommandH.topicID = 0;
	topicSubConfig.topicID = 0;
	
	topicSubCommandA.topicName = topicRegisterCommandA;
	topicSubCommandB.topicName = topicRegisterCommandB;
	topicSubCommandC.topicName = topicRegisterCommandC;
	topicSubCommandD.topicName = topicRegisterCommandD;
	topicSubCommandE.topicName = topicRegisterCommandE;
	topicSubCommandF.topicName = topicRegisterCommandF;
	topicSubCommandG.topicName = topicRegisterCommandG;
	topicSubCommandH.topicName = topicRegisterCommandH;
	topicSubConfig.topicName = topicRegisterConfig;
	
	topicSubCommandA.cb = cb_commandA;
	topicSubCommandB.cb = cb_commandB;
	topicSubCommandC.cb = cb_commandC;
	topicSubCommandD.cb = cb_commandD;
	topicSubCommandE.cb = cb_commandE;
	topicSubCommandF.cb = cb_commandF;
	topicSubCommandG.cb = cb_commandG;
	topicSubCommandH.cb = cb_commandH;
	topicSubConfig.cb = cb_config;
	
	flagsA = xEventGroupCreate();
	flagsB = xEventGroupCreate();
	flagsC = xEventGroupCreate();
	flagsD = xEventGroupCreate();
	flagsE = xEventGroupCreate();
	flagsF = xEventGroupCreate();
	flagsG = xEventGroupCreate();
	flagsH = xEventGroupCreate();
	
	queueRelayA = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayB = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayC = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayD = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayE = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayF = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayG = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	queueRelayH = xQueueCreate(RELAY_TASK_COMMANDS, sizeof(unsigned char*));
	
	//load local BT address (used to create an IPv6 link-local address)
	//memcpy(buf,getBTLocalAddress(), sizeof(bd_addr_t));
	strcpy(local,getLocalIPv6Address());
	

	while(1)
	{
		#if DEBUG_HW == 1
			uint8_t i = 0;
			//set all relays
		    for(i=0; i<=5; i++)
		    {
				ESP_LOGE("DEBUG_HW","Set R/S relay %d",i);
				updateRelay(i,1);
			    //wait for setup time for relays (max 15ms initially)
			    ets_delay_us(16500);
			    //set outputs for R/S relays to idle
			    updateRelay(-1,0);
			    vTaskDelay(2000 / portTICK_PERIOD_MS);
			}
			
			ESP_LOGE("DEBUG_HW","Set normal relay %d",6);
			updateRelay(6,1);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			ESP_LOGE("DEBUG_HW","Set normal relay %d",7);
			updateRelay(7,1);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			
			//reset all relays
		    for(i=0; i<=5; i++)
		    {
				ESP_LOGE("DEBUG_HW","Reset R/S relay %d",i);
				updateRelay(i,0);
			    //wait for setup time for relays (max 15ms initially)
			    ets_delay_us(16500);
			    //set outputs for R/S relays to idle
			    updateRelay(-1,0);
			    vTaskDelay(2000 / portTICK_PERIOD_MS);
			}
			
			ESP_LOGE("DEBUG_HW","Reset normal relay %d",6);
			updateRelay(6,0);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			ESP_LOGE("DEBUG_HW","Reset normal relay %d",7);
			updateRelay(7,0);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
		#else 		
			for(int i = 0; i<30; i++)
			{
				vTaskDelay(10 / portTICK_PERIOD_MS);
				//esp_task_wdt_feed();
			}
			
	        level = !level;	
	        
	        if((xEventGroupGetBits(lowpan_ble_flags) & LOWPAN_BLE_FLAG_CONNECTED))
	        {   
				if(test == 0) { 
					vTaskDelay(3000 / portTICK_PERIOD_MS); 
					test++;
					//get BLE host address from driver
					strcpy(mqtt_host,getRemoteIPv6Address());
					
					ESP_LOGD("MAIN","local  IPv6 address: %s",local);
					ESP_LOGD("MAIN","remote IPv6 address: %s",mqtt_host);
					
					//create configuration
					strcpy(mqtt_config.gateway_addr,mqtt_host);
					mqtt_config.gateway_port = 1883;
					mqtt_config.local_port = 448;
					mqtt_config.interface = &rfc7668_netif;
					mqtt_config.ip_type = IPADDR_TYPE_V6;
					
					strcpy(mqtt_config_client.clientID,client);
					mqtt_config_client.willFlag = 0;
					
					//init & connect to MQTT-SN host(gateway)
					mqtt_sn_client_init(&mqtt_config,&mqtt_config_client);
					
				}
				
				//wait until connected , max 2s; 
				if(mqtt_sn_client_wait_status(MQTT_SN_CLIENT_ACTIVE, 2000 / portTICK_PERIOD_MS) == MQTT_SN_CLIENT_ACTIVE)
				{				
					//register+subscribe for topic
					if(topicSubCommandA.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandA);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandA: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandB.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandB);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandB: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandC.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandC);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandC: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandD.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandD);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandD: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandE.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandE);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandE: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandF.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandF);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandF: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandG.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandG);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandG: %d",ret);
					} 
					//register+subscribe for topic
					if(topicSubCommandH.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommandH);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing commandH: %d",ret);
					} 
					
					//register+subscribe for topic
					if(topicSubConfig.topicID == 0)
					{
						ret = mqtt_sn_client_add_subscribe_topic(&topicSubConfig);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing config: %d",ret);
					} 
					
					
					//register for publishing topic id for status
					if(topicStateA.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateA);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status A: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateB.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateB);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status B: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateC.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateC);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status C: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateD.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateD);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status D: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateE.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateE);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status E: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateF.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateF);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status F: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateG.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateG);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status G: %d",ret);
					}
					//register for publishing topic id for status
					if(topicStateH.topicID == 0)
					{
						ret = mqtt_sn_client_register_topic(&topicStateH);
						if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering status H: %d",ret);
					}
				} else {
					printf("No CONNACK in 2s...");
				}
			}
		#endif
	}
}
