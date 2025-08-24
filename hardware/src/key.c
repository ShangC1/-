#include "stm32f10x.h"

//Ӳ������
#include "key.h"
#include "delay.h"
#include "led.h"
#include "Motor.h"

/*
************************************************************
*	�������ƣ�	Key_Init
*
*	�������ܣ�	key��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Key_Init(void)
{

	GPIO_InitTypeDef GPIO_Initstructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//��GPIOB��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU;				//����Ϊ���
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;						//����ʼ����Pin��
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;				//�ɳ��ص����Ƶ��
	
	GPIO_Init(GPIOB, &GPIO_Initstructure);							//��ʼ��GPIOB
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1 | EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_IRQHandler(void)
{
	DelayXms(10);
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		if(led_info.Led_Status == LED_ON)Led_Set(LED_OFF);
		else Led_Set(LED_ON);
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI15_10_IRQHandler(void)
{
	DelayXms(10);
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		fan_speed += 20;
		if(fan_speed > 100)
		{
			fan_speed = 0;
		}
		Motor_SetSpeed(fan_speed);
	}
	EXTI_ClearITPendingBit(EXTI_Line11);
}
