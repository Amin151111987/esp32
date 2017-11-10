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
//implement ADC measurement & publishing of current(power) & voltage



uint8_t driverMode = 0;
uint8_t driverOption = 0;
char outputMode[4] = {'+','R','G','B'};
uint16_t ledcFadeTime = 2000;



// PWM Channel definition
#define OUTPUT1_PWMchannel_LS	LEDC_CHANNEL_0
#define OUTPUT1_PWMchannel_HS	LEDC_CHANNEL_1
#define OUTPUT2_PWMchannel_LS	LEDC_CHANNEL_2
#define OUTPUT2_PWMchannel_HS	LEDC_CHANNEL_3
#define OUTPUT3_PWMchannel_LS	LEDC_CHANNEL_4
#define OUTPUT3_PWMchannel_HS	LEDC_CHANNEL_5
#define OUTPUT4_PWMchannel_LS	LEDC_CHANNEL_6
#define OUTPUT4_PWMchannel_HS	LEDC_CHANNEL_7

// PWM GPIO definition
#define OUTPUT1_PWMgpio_LS		GPIO_NUM_27
#define OUTPUT1_PWMgpio_HS		GPIO_NUM_12 
#define OUTPUT2_PWMgpio_LS		GPIO_NUM_32
#define OUTPUT2_PWMgpio_HS		GPIO_NUM_33
#define OUTPUT3_PWMgpio_LS		GPIO_NUM_25
#define OUTPUT3_PWMgpio_HS		GPIO_NUM_26
#define OUTPUT4_PWMgpio_LS		GPIO_NUM_16
#define OUTPUT4_PWMgpio_HS		GPIO_NUM_17


#define OUTPUT_MODE				LEDC_HIGH_SPEED_MODE
#define OUTPUT_TIMER			LEDC_TIMER_0

// PWM settings
#define PWMfrequency			5000
#define PWMresolution			LEDC_TIMER_13_BIT

// ADC lines
#define current_feedback_1		ADC1_CHANNEL_0
#define current_feedback_2		ADC1_CHANNEL_3 
#define current_feedback_3		ADC1_CHANNEL_6
//#define current_feedback_4		ADC2_CHANNEL_4 //ADC2 currently not usable
#define voltage_monitor			ADC1_CHANNEL_7

char topicRegisterBrightness[] = "/light/brightness/set";
char topicRegisterRGB[] = "/light/rgb/set";
char topicRegisterCommand[] = "/light/switch";
char topicRegisterConfig[] = "/tsh/config/actuators";

mqtt_sn_topic_register_cfg_t topicState = 
{
	.topicName = "/light/status",
	.topicID = 0
};
mqtt_sn_topic_register_cfg_t topicBrightnessState = 
{
	.topicName = "/light/brightness/status",
	.topicID = 0
};
mqtt_sn_topic_register_cfg_t topicRGBState = 
{
	.topicName = "/light/RGB/status",
	.topicID = 0
};


typedef struct {
    int channel;
    int io;
    int mode;
    int timer_idx;
} ledc_info_t;

uint16_t currentR = 0;
uint16_t currentG = 0;
uint16_t currentB = 0;
uint16_t currentBrightness = 0;
//brightness for driverMode 2 (2 single color LED stripes on 1+2 and 3+4)
uint16_t currentBrightness2 = 0;


//char local[] = "fe80::240a:c4ff:fe04:b132";
char local[40];
char mqtt_host[40];
//char mqtt_host[] = "fe80::21a:7dff:feda:7113";
char client[] = "ToRaDes_123";

extern struct netif rfc7668_netif;

    ledc_info_t ledc_ch[8] = {
        {
            .channel   = OUTPUT1_PWMchannel_LS,
            .io        = OUTPUT1_PWMgpio_LS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT1_PWMchannel_HS,
            .io        = OUTPUT1_PWMgpio_HS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT2_PWMchannel_LS,
            .io        = OUTPUT2_PWMgpio_LS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT2_PWMchannel_HS,
            .io        = OUTPUT2_PWMgpio_HS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT3_PWMchannel_LS,
            .io        = OUTPUT3_PWMgpio_LS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT3_PWMchannel_HS,
            .io        = OUTPUT3_PWMgpio_HS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT4_PWMchannel_LS,
            .io        = OUTPUT4_PWMgpio_LS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        },
        {
            .channel   = OUTPUT4_PWMchannel_HS,
            .io        = OUTPUT4_PWMgpio_HS,
            .mode      = OUTPUT_MODE,
            .timer_idx = OUTPUT_TIMER
        }
    };


void updateFadingMode2(uint16_t value1, uint16_t value2)
{
	uint16_t valW = 0;
	uint8_t i = 0;
	
	//do the same for output 1+2 and 3+4
	for(i = 0; i<=1; i++)
	{
		if(i == 0) valW = value1;
		if(i == 1) valW = value2;
		
		if(outputMode[i] == 'W')
		{
			if(outputMode[i+1] == '+') {
				//1 -> W (fade); 2 -> +
				//output 1: high side off
			    ledc_set_duty(ledc_ch[i+1].mode, ledc_ch[i+1].channel, 0);
			    ledc_update_duty(ledc_ch[i+1].mode, ledc_ch[i+1].channel);
				//output 1: low side fade
				ledc_set_fade_with_time(ledc_ch[i].mode, ledc_ch[i].channel, valW, ledcFadeTime);
			    ledc_fade_start(ledc_ch[i].mode, ledc_ch[i].channel, LEDC_FADE_NO_WAIT);
			    //output 2: low side off
			    ledc_set_duty(ledc_ch[i+2].mode, ledc_ch[i+2].channel, 0);
			    ledc_update_duty(ledc_ch[i+2].mode, ledc_ch[i+2].channel);
			    //output 2: high side on
			    ledc_set_duty(ledc_ch[i+3].mode, ledc_ch[i+3].channel, ((1<<PWMresolution) - 1));
			    ledc_update_duty(ledc_ch[i+3].mode, ledc_ch[i+3].channel);
			} else if(outputMode[1] == '-') {
				//1 -> W (fade); 2 -> -
				//output 1: low side off
			    ledc_set_duty(ledc_ch[i].mode, ledc_ch[i].channel, 0);
			    ledc_update_duty(ledc_ch[i].mode, ledc_ch[i].channel);
				//output 1: high side fade
				ledc_set_fade_with_time(ledc_ch[i+1].mode, ledc_ch[i+1].channel, valW, ledcFadeTime);
			    ledc_fade_start(ledc_ch[i+1].mode, ledc_ch[i+1].channel, LEDC_FADE_NO_WAIT);
			    //output 2: low side on
			    ledc_set_duty(ledc_ch[i+2].mode, ledc_ch[i+2].channel, ((1<<PWMresolution) - 1));
			    ledc_update_duty(ledc_ch[i+2].mode, ledc_ch[i+2].channel);
			    //output 2: high side off
			    ledc_set_duty(ledc_ch[i+3].mode, ledc_ch[i+3].channel, 0);
			    ledc_update_duty(ledc_ch[i+3].mode, ledc_ch[i+3].channel);
			} else {
				ESP_LOGE("MAIN","Invalid driver mode, no '+'/'-' output on %d",i+2);
			}
		} else if(outputMode[1] == 'W') {
			if(outputMode[i] == '+') {
				//1 -> + ; 2 -> W (fade)
				//output 1: low side off
			    ledc_set_duty(ledc_ch[i].mode, ledc_ch[i].channel, 0);
			    ledc_update_duty(ledc_ch[i].mode, ledc_ch[i].channel);
				//output 1: high side on
				ledc_set_duty(ledc_ch[i+1].mode, ledc_ch[i+1].channel, ((1<<PWMresolution) - 1));
			    ledc_update_duty(ledc_ch[i+1].mode, ledc_ch[i+1].channel);
			    //output 2: low side fade
			    ledc_set_fade_with_time(ledc_ch[i+2].mode, ledc_ch[i+2].channel, valW, ledcFadeTime);
			    ledc_fade_start(ledc_ch[i+2].mode, ledc_ch[i+2].channel, LEDC_FADE_NO_WAIT);
			    //output 2: high side off
			    ledc_set_duty(ledc_ch[i+3].mode, ledc_ch[i+3].channel, 0);
			    ledc_update_duty(ledc_ch[i+3].mode, ledc_ch[i+3].channel);
			} else if(outputMode[i] == '-') {
				//1 -> - ; 2 -> W (fade)
				//output 1: low side on
			    ledc_set_duty(ledc_ch[i].mode, ledc_ch[i].channel, ((1<<PWMresolution) - 1));
			    ledc_update_duty(ledc_ch[i].mode, ledc_ch[i].channel);
				//output 1: high side off
				ledc_set_duty(ledc_ch[i+1].mode, ledc_ch[i+1].channel, 0);
			    ledc_update_duty(ledc_ch[i+1].mode, ledc_ch[i+1].channel);
			    //output 2: low side off
			    ledc_set_duty(ledc_ch[i+2].mode, ledc_ch[i+2].channel, 0);
			    ledc_update_duty(ledc_ch[i+2].mode, ledc_ch[i+2].channel);
			    //output 2: high side fade
			    ledc_set_fade_with_time(ledc_ch[i+3].mode, ledc_ch[i+3].channel, valW, ledcFadeTime);
			    ledc_fade_start(ledc_ch[i+3].mode, ledc_ch[i+3].channel, LEDC_FADE_NO_WAIT);
			} else {
				ESP_LOGE("MAIN","Invalid driver mode, no '+'/'-' output on %d",i+1);
			}
		} else {
			ESP_LOGE("MAIN","Invalid driver mode, no 'W' output on %d or %d",i+1,i+2);
		}
	}
}


void updateFadingMode1(uint16_t r,uint16_t g,uint16_t b)
{
	uint8_t i = 0;
	uint16_t j = 0;
	//uint16_t maxOutput = (1<<PWMresolution)-1;
	
	uint8_t ca_cc_output;
	uint16_t ca_cc_config[2];
	
	uint8_t r_output = 0;
	uint16_t r_config[2];
	
	uint8_t g_output = 0;
	uint16_t g_config[2];
	
	uint8_t b_output = 0;
	uint16_t b_config[2];
	
	
	//test configuration - check for one single +/- output
	j = 0;
	for(i=0;i<=3;i++)
	{
		if(outputMode[i] == 'R') r_output = i;
		if(outputMode[i] == 'G') g_output = i;
		if(outputMode[i] == 'B') b_output = i;
		
		if(outputMode[i] == '+')
		{
			//increase count of CA/CC outputs. Will be tested later
			j++;
			//save output of CA/CC
			ca_cc_output = i;
			//low side: 0, high side: always on
			ca_cc_config[0] = 0;
			ca_cc_config[1] = ((1<<PWMresolution) - 1);
			//low side (RGB): fade (inverted!), high side: 0
			/*r_config[0] = maxOutput-r;
			g_config[0] = maxOutput-g;
			b_config[0] = maxOutput-b;*/
			r_config[0] = r;
			g_config[0] = g;
			b_config[0] = b;
			r_config[1] = g_config[1] = b_config[1] = 0;
		}
		if(outputMode[i] == '-')
		{
			//increase count of CA/CC outputs. Will be tested later
			j++;
			//save output of CA/CC
			ca_cc_output = i;
			//low side: always on, high side: 0
			ca_cc_config[0] = ((1<<PWMresolution) - 1);
			ca_cc_config[1] = 0;
			//low side (RGB): 0, high side: fade
			r_config[0] = g_config[0] = b_config[0] = 0;
			r_config[1] = r;
			g_config[1] = g;
			b_config[1] = b;
			
		}
	}
	
	if(j != 1)
	{
		ESP_LOGE("MAIN","Invalid output mode, CA/CC output count (must be 1): %d",j);
		return;
	}
	
	//test configuration -  check if all colors exist
	j = 0;
	for(i=0;i<=3;i++) j += outputMode[i]; //sum up all characters, will be tested

	if(!(j == ('R'+'G'+'B'+'+') || j == ('R'+'G'+'B'+'-')))
	{
		ESP_LOGE("MAIN","Invalid output mode, RGB not complete: %c,%c,%c,%c",outputMode[0],outputMode[1],outputMode[2],outputMode[3]);
		return;
	}
	
	//set CA/CC output either to high or low
	//output CA/CC: low side
    ledc_set_duty(ledc_ch[2*ca_cc_output].mode, ledc_ch[2*ca_cc_output].channel, ca_cc_config[0]);
    ledc_update_duty(ledc_ch[2*ca_cc_output].mode, ledc_ch[2*ca_cc_output].channel);
	//output CA/CC: high side
	ledc_set_duty(ledc_ch[2*ca_cc_output+1].mode, ledc_ch[2*ca_cc_output+1].channel, ca_cc_config[1]);
    ledc_update_duty(ledc_ch[2*ca_cc_output+1].mode, ledc_ch[2*ca_cc_output+1].channel);
    
    //set RGB colors (according to previous set output numbers AND high/low side configuration
    //RED: low side
    ledc_set_fade_with_time(ledc_ch[2*r_output].mode, ledc_ch[2*r_output].channel, r_config[0], ledcFadeTime);
    ledc_fade_start(ledc_ch[2*r_output].mode, ledc_ch[2*r_output].channel, LEDC_FADE_NO_WAIT);
	//RED: high side
	ledc_set_fade_with_time(ledc_ch[2*r_output+1].mode, ledc_ch[2*r_output+1].channel, r_config[1], ledcFadeTime);
    ledc_fade_start(ledc_ch[2*r_output+1].mode, ledc_ch[2*r_output+1].channel, LEDC_FADE_NO_WAIT);
    
    //GREEN: low side
    ledc_set_fade_with_time(ledc_ch[2*g_output].mode, ledc_ch[2*g_output].channel, g_config[0], ledcFadeTime);
    ledc_fade_start(ledc_ch[2*g_output].mode, ledc_ch[2*g_output].channel, LEDC_FADE_NO_WAIT);
	//GREEN: high side
	ledc_set_fade_with_time(ledc_ch[2*g_output+1].mode, ledc_ch[2*g_output+1].channel, g_config[1], ledcFadeTime);
    ledc_fade_start(ledc_ch[2*g_output+1].mode, ledc_ch[2*g_output+1].channel, LEDC_FADE_NO_WAIT);
    
    
    //BLUE: low side
    ledc_set_fade_with_time(ledc_ch[2*b_output].mode, ledc_ch[2*b_output].channel, b_config[0], ledcFadeTime);
    ledc_fade_start(ledc_ch[2*b_output].mode, ledc_ch[2*b_output].channel, LEDC_FADE_NO_WAIT);
	//BLUE: high side
	ledc_set_fade_with_time(ledc_ch[2*b_output+1].mode, ledc_ch[2*b_output+1].channel, b_config[1], ledcFadeTime);
    ledc_fade_start(ledc_ch[2*b_output+1].mode, ledc_ch[2*b_output+1].channel, LEDC_FADE_NO_WAIT);
	
}

void updateFading(uint8_t on)
{
	
	//channel 0: output 1 low side
	//channel 1: output 1 high side
	//channel 2: output 2 low side
	//channel 3: output 2 high side
	//channel 4: output 3 low side
	//channel 5: output 3 high side
	//channel 6: output 4 low side
	//channel 7: output 4 high side
	
	uint16_t valR = currentR;
	uint16_t valG = currentG;
	uint16_t valB = currentB;
	uint16_t valW = currentBrightness;
	uint16_t valW2 = currentBrightness2;
	
	//map 8 bit values from MQTT to LED fading width
	uint8_t shift = PWMresolution - 8; /* calculate bit width difference */
	uint8_t i = 0;
	
	if(on)
	{
		
		for(i = 0; i<shift; i++)
		{
			valR = valR*2 +1;
			valG = valG*2 +1;
			valB = valB*2 +1;
			valW = valW*2 +1;
			valW2 = valW2*2 +1;
		}
		
		
		//map RGB values to brightness settings (only for mode==1)
		float multi = (float)valW / (float)((1<<PWMresolution) - 1);
		valR = (uint16_t)((float)valR * multi);
		valG = (uint16_t)((float)valG * multi);
		valB = (uint16_t)((float)valB * multi);
	} else {
		valR = valG = valB = valW = valW2 = 0;
	}
	
	ESP_LOGD("MAIN","Adjusted values (to LEDC bit width): %d,%d,%d,%d,%d",valR,valG,valB,valW,valW2);
	
	switch(driverMode)
	{
		//mode 1: typical 1 RGB stripe, either Common Anode or Cathode
		//1 output MUST be either set to '+' or '-'
		//remaining outputs are set to R,G and B
		case 1:
			updateFadingMode1(valR,valG,valB);
			break;
			
			
		//mode 2: 2 independent single color LED (stripes) on 1+2 and 3+4
		//possible configurations (channel 3+4: equal to 1+2)
		//1		2	driverOption value
		//W		+	1
		//W		-	2
		//+		W	3
		//-		W	4
		case 2:
			updateFadingMode2(valW,valW2);
			break;
		default:
			ESP_LOGE("MAIN","Invalid driver mode!");
			break;
	}
		
}

err_t cb_command(mqtt_sn_pkt_t* topicData)
{
	mqtt_sn_pkt_t ack;
	err_t rc;
	ack.topicID = topicState.topicID;
	ack.dup = 0;
	ack.retained = 0;
	ack.qos = 0;
	
	if(strcmp((char*)topicData->payload,"ON") == 0)
	{
		ESP_LOGD("MAIN","Set LEDs on");
		ack.payload = (unsigned char*)"ON";
		ack.len = strlen("ON");
		rc = mqtt_sn_client_publish_registered(&ack);
		
		updateFading(1);
		
		if(rc!=ERR_OK) ESP_LOGE("MAIN","ACK state failed: %d",rc);
		return ERR_OK;
	}
	
	if(strcmp((char*)topicData->payload,"OFF") == 0)
	{
		ESP_LOGD("MAIN","Set LEDs off");
		ack.payload = (unsigned char*)"OFF";
		ack.len = strlen("OFF");
		rc = mqtt_sn_client_publish_registered(&ack);

		updateFading(0);
		
		if(rc!=ERR_OK) ESP_LOGE("MAIN","ACK state failed: %d",rc);
		return ERR_OK;
	}
	
	ESP_LOGE("MAIN","Error, unknown command!");
	return ERR_VAL;
}
err_t cb_rgb(mqtt_sn_pkt_t* topicData)
{
	mqtt_sn_pkt_t ack;
	err_t rc;	
	char *configPart;
	char sep[] = ",";
	
	configPart = strtok((char*)topicData->payload, sep);
	
	//1.) R (Red)
	if(configPart != NULL)
	{
		currentR = atoi(configPart);
		ESP_LOGI("MAIN","R: %d",currentR);
		configPart = strtok(NULL,sep);
	} else {
		ESP_LOGE("MAIN","error updating RGB: missing R value");
		return ERR_VAL;
	}
	
	//2.) G (Green)
	if(configPart != NULL)
	{
		currentG = atoi(configPart);
		ESP_LOGI("MAIN","G: %d",currentG);
		configPart = strtok(NULL,sep);
	} else {
		ESP_LOGE("MAIN","error updating RGB: missing G value");
		return ERR_VAL;
	}
	
	//3.) B (Blue)
	if(configPart != NULL)
	{
		currentB = atoi(configPart);
		ESP_LOGI("MAIN","B: %d",currentB);
		configPart = strtok(NULL,sep);
	} else {
		ESP_LOGE("MAIN","error updating RGB: missing B value");
		return ERR_VAL;
	}

	
	updateFading(1);
		
	ack.topicID = topicBrightnessState.topicID;
	ack.dup = 0;
	ack.retained = 0;
	ack.qos = 0;
	ack.payload = topicData->payload;
	ack.len = strlen((char*)topicData->payload);
	rc = mqtt_sn_client_publish_registered(&ack);
	if(rc!=ERR_OK) ESP_LOGE("MAIN","ACK RGB failed: %d",rc);
	
	ESP_LOGD("MAIN","Set RGB: %d,%d,%d",currentR,currentG,currentB);
	
	return ERR_OK;
}

err_t cb_config(mqtt_sn_pkt_t* topicData)
{
	//TBD: use config only if this device is addressed (via BT MAC)
	//TBD: pairing & authentication
	//TBD: authenticate config update...
	
	//<MAC>;<1>;<2>;<3>;<4>;<Mode>;<Fade Time [ms](optional, default: 2000ms)>
	//1-4: +/R/G/B/-/W
	//Mode:
	//1: RGB
	//2: 2x single color
	
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
			//test the output mode character for validity
			switch(configPart[0])
			{
				case '+':
				case 'R':
				case 'G':
				case 'B':
				case '-':
				case 'W':
					ESP_LOGI("MAIN","output %d: %c",i+1,configPart[0]);
					outputMode[i] = configPart[0];
					break;
				default:
					ESP_LOGE("MAIN","invalid config, output mode wrong: %c",configPart[0]);
					break;
			}
			
			configPart = strtok(NULL,sep);
		} else {
			ESP_LOGE("MAIN","invalid config, output %d missing",i+1);
			return ERR_VAL;
		}
	}
	
	//3.) output mode
	if(configPart != NULL)
	{
		//test the output mode character for validity
		switch(configPart[0])
		{
			case '1':
			case '2':
				ESP_LOGI("MAIN","output mode: %c",configPart[0]);
				driverMode = configPart[0]-'0';
				break;
			default:
				ESP_LOGE("MAIN","invalid config, wrong driver mode: %c",configPart[0]);
				break;
		}
		
		configPart = strtok(NULL,sep);
	} else {
		ESP_LOGE("MAIN","invalid config, mode missing");
		return ERR_VAL;
	}
	
	//4.) fade time
	if(configPart != NULL)
	{
		ledcFadeTime = atoi(configPart);
		ESP_LOGI("MAIN","fade duration: %d",ledcFadeTime);
		configPart = strtok(NULL,sep);
	}

	return ERR_OK;
}
err_t cb_brightness(mqtt_sn_pkt_t* topicData)
{
	mqtt_sn_pkt_t ack;
	err_t rc;
	currentBrightness = atoi((char*)topicData->payload);
	
	updateFading(1);
		
	ack.topicID = topicBrightnessState.topicID;
	ack.dup = 0;
	ack.retained = 0;
	ack.qos = 0;
	ack.payload = topicData->payload;
	ack.len = strlen((char*)topicData->payload);
	rc = mqtt_sn_client_publish_registered(&ack);
	if(rc!=ERR_OK) ESP_LOGE("MAIN","ACK brightness failed: %d",rc);
	
	ESP_LOGD("MAIN","Set brightness: %d",currentBrightness);
	
	return ERR_OK;
}


void hw_configuration()
{
	//TBD: ADC
	//analogSetAttenuation(ADC_11db);		// ADC configuration
	
	int ch;
	
	ledc_timer_config_t ledc_timer = {
        .bit_num = PWMresolution, //set timer counter bit number
        .freq_hz = PWMfrequency,              //set frequency of pwm
        .speed_mode = OUTPUT_MODE,   //timer mode,
        .timer_num = OUTPUT_TIMER    //timer index
    };
    
    //configure timer for ledc channel
    ledc_timer_config(&ledc_timer);

    for (ch = 0; ch < 8; ch++) {
        ledc_channel_config_t ledc_channel = {
            //set LEDC channel 0
            .channel = ledc_ch[ch].channel,
            //set the duty for initialization.(duty range is 0 ~ ((2**bit_num)-1)
            .duty = 0,
            //GPIO number
            .gpio_num = ledc_ch[ch].io,
            //GPIO INTR TYPE, as an example, we enable fade_end interrupt here.
            .intr_type = LEDC_INTR_FADE_END,
            //set LEDC mode, from ledc_mode_t
            .speed_mode = ledc_ch[ch].mode,
            //set LEDC timer source, if different channel use one timer,
            //the frequency and bit_num of these channels should be the same
            .timer_sel = ledc_ch[ch].timer_idx,
        };
        //set the configuration
        ledc_channel_config(&ledc_channel);
    }
    
    //initialize fade service.
    ledc_fade_func_install(0);
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
	mqtt_sn_topic_subscribe_cfg_t topicSubBrightness;
	mqtt_sn_topic_subscribe_cfg_t topicSubRGB;
	mqtt_sn_topic_subscribe_cfg_t topicSubCommand;
	mqtt_sn_topic_subscribe_cfg_t topicSubConfig;

	topicSubBrightness.topicID = 0;
	topicSubRGB.topicID = 0;
	topicSubCommand.topicID = 0;
	topicSubConfig.topicID = 0;
	
	topicSubBrightness.topicName = topicRegisterBrightness;
	topicSubRGB.topicName = topicRegisterRGB;
	topicSubCommand.topicName = topicRegisterCommand;
	topicSubConfig.topicName = topicRegisterConfig;
	
	topicSubBrightness.cb = cb_brightness;
	topicSubRGB.cb = cb_rgb;
	topicSubCommand.cb = cb_command;
	topicSubConfig.cb = cb_config;
	
	//load local BT address (used to create an IPv6 link-local address)
	//memcpy(buf,getBTLocalAddress(), sizeof(bd_addr_t));
	strcpy(local,getLocalIPv6Address());
	

	while(1)
	{
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
				//register for topic
				if(topicSubBrightness.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubBrightness);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing Brightness: %d",ret);
				} 
				
				//register for topic
				if(topicSubRGB.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubRGB);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing RGB: %d",ret);
				} 
				
				//register for topic
				if(topicSubCommand.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubCommand);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing command: %d",ret);
				} 
				
				//register for topic
				if(topicSubConfig.topicID == 0)
				{
					ret = mqtt_sn_client_add_subscribe_topic(&topicSubConfig);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error subscribing config: %d",ret);
				} 
				
				//register for publishing topic id for brightness status
				if(topicBrightnessState.topicID == 0)
				{
					ret = mqtt_sn_client_register_topic(&topicBrightnessState);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering brightness status: %d",ret);
				}
				
				//register for publishing topic id for RGB status
				if(topicRGBState.topicID == 0)
				{
					ret = mqtt_sn_client_register_topic(&topicRGBState);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering brightness RGB: %d",ret);
				}
				
				//register for publishing topic id for brightness status
				if(topicState.topicID == 0)
				{
					ret = mqtt_sn_client_register_topic(&topicState);
					if(ret != ERR_OK) ESP_LOGE("MAIN","Error registering general status: %d",ret);
				}
				
				
				/*if(level)
				{
					sendData.payload = data1;
					sendData.len = len1;
					mqtt_sn_client_publish_short(&sendData);
				} else {
					sendData.payload = data2;
					sendData.len = len2;
					mqtt_sn_client_publish_short(&sendData);
				}*/
			} else {
				printf("No CONNACK in 2s...");
			}
		}
	}
}
