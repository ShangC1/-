#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "dht11.h"
#include "OLED.h"
#include "Motor.h"

//C库
#include <string.h>


#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.96\",1883\r\n"


/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
    Key_Init();										//Key初始化
	
	Led_Init();									//LED初始化
	
	OLED_Init();									//OLED初始化
	
	Motor_Init();
	
	OLED_ShowString(1, 1, "Monitor");
	
	while(DHT11_Init())
	{
		UsartPrintf(USART_DEBUG, "DHT11 Error \r\n");
		OLED_ShowString(2, 1, "DHT11 Error");
		DelayMs(1000);
	}
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/

u8 temp,humi;

int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266

	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		DelayXms(500);
	UsartPrintf(USART_DEBUG, "Connect MQTTs Success! \r\n");
	while(OneNet_DevLink())			//接入OneNET
		DelayXms(500);
	
	OneNET_Subscribe();
	
	OLED_ShowString(2, 1, "Temp:   Celsius");
	OLED_ShowString(3, 1, "Humi:  %");
	OLED_ShowString(4, 1, "LED:    Fan:");
	
	while(1)
	{
		if(++timeCount >= 100)									//发送间隔5s
		{
			DHT11_Read_Data(&temp,&humi);
			OLED_ShowNum(2, 6, temp, 2);
			OLED_ShowNum(3, 6, humi, 2);
			OLED_ShowString(4, 5, led_info.Led_Status ? "ON " : "OFF");
			OLED_ShowNum(4, 13, fan_speed, 3);
			UsartPrintf(USART_DEBUG, "temp %d ,humi %d fan %d\r\n",temp,humi,fan_speed);
			
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		DelayMs(10);
	}

}
