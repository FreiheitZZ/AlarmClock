#include ".\exti.h"
#include "..\..\Common\delay.h" 
#include "..\KEY\key.h"
#include "FreeRTOS.h"
#include "task.h"

#define EVENTBIT_0 (1<<0)

//外部中断初始化程序
//初始化PE4为中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //按键对应的IO口初始化
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 连接到中断线4
	
	/* 配置EXTI_Line4 */
	EXTI_InitStructure.EXTI_Line =  EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//中断线使能
	EXTI_Init(&EXTI_InitStructure);							//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//抢占优先级6
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;	//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);							//配置	   
}

//任务句柄
extern TaskHandle_t EventGroupTask_Handler;
//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	
	delay_xms(20);	//消抖
	if(KEY0==0)	 
	{				 
		xTaskNotifyFromISR((TaskHandle_t	)EventGroupTask_Handler, 	//任务句柄
						   (uint32_t		)EVENTBIT_0, 				//要更新的bit
						   (eNotifyAction	)eSetBits, 					//更新指定的bit
						   (BaseType_t*		)xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line4);//清除LINE4上的中断标志位  
}

