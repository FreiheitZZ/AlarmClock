#include "..\Common\sys.h"
#include "..\Common\delay.h"
#include "..\Common\usart.h"
#include "..\Device\led\led.h"
#include "..\Device\timer\timer.h"
#include "..\Device\lcd\lcd.h"
#include "..\Device\key\key.h"
#include "..\Device\beep\beep.h"
#include "..\Device\exti\exti.h"
#include "string.h"
#include "..\Common\malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"
#include "..\Common\ringbuffer.h"
#include "..\Driver\uart\uart.h"
//#include "..\Platform\RTOS\TaskConfig.h"

////////////////////////////////////////////////////////
#define EVENTBIT_0	(1<<0)				//事件位
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

extern unsigned char		s_EntryNo_usart1;
extern unsigned char		s_EntryNo_usart2;

extern TaskHandle_t EventGroupTask_Handler;
extern TaskHandle_t EventSetBit_Handler;

extern	void	initialTask_Create(void);

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);					//初始化延时函数
	
	initialTask_Create();
    vTaskStartScheduler();          //开启任务调度
}


//设置事件位的任务
void eventsetbit_task(void *pvParameters)
{
	u8 key,i;
	while(1)
	{
		if(EventGroupTask_Handler!=NULL)
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY1_PRES:
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
								(uint32_t		)EVENTBIT_1,			//要更新的bit
								(eNotifyAction	)eSetBits);				//更新指定的bit
					break;
				case KEY2_PRES:
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
								(uint32_t		)EVENTBIT_2,			//要更新的bit
								(eNotifyAction	)eSetBits);				//更新指定的bit
					break;	
			}
		}
		i++;
		if(i==50)
		{
			i=0;
			LED0=!LED0;
			while(RB_getBufferStatus(s_EntryNo_usart1) != RB_STS_EMPTY)
			{
				UART_transmitData(1, "%c",RB_getRingBuffer(s_EntryNo_usart1));
			}
		}
        vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
	u8 num=0,enevtvalue;
	static u8 event0flag,event1flag,event2flag;
	uint32_t NotifyValue;
	BaseType_t err;
	
	while(1)
	{
		//获取任务通知值
		err=xTaskNotifyWait((uint32_t	)0x00,				//进入函数的时候不清除任务bit
							(uint32_t	)ULONG_MAX,			//退出函数的时候清除所有的bit
							(uint32_t*	)&NotifyValue,		//保存任务通知值
							(TickType_t	)portMAX_DELAY);	//阻塞时间
		
		if(err==pdPASS)	   //任务通知获取成功
		{
			if((NotifyValue&EVENTBIT_0)!=0)			//事件0发生	
			{
				event0flag=1;	
			}				
			else if((NotifyValue&EVENTBIT_1)!=0)	//事件1发生	
			{
				event1flag=1;
			}
			else if((NotifyValue&EVENTBIT_2)!=0)	//事件2发生	
			{
				event2flag=1;	
			}
	
			enevtvalue=event0flag|(event1flag<<1)|(event2flag<<2);	//模拟事件标志组值
			printf("任务通知值为:%d\r\n",enevtvalue);
			LCD_ShowxNum(174,110,enevtvalue,1,16,0);				//在LCD上显示当前事件值
			
			if((event0flag==1)&&(event1flag==1)&&(event2flag==1))	//三个事件都同时发生
			{
				num++;
				LED1=!LED1;	
				LCD_Fill(6,131,233,313,lcd_discolor[num%14]);
				event0flag=0;								//标志清零
				event1flag=0;
				event2flag=0;
			}
		}
		else
		{
			printf("任务通知获取失败\r\n");
		}

	}
}

