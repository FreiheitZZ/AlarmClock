#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "beep.h"
#include "key.h"
#include "usart3.h"
#include "ESP8266.h"
#include "..\MANAGE\CLOCK\Clock.h"


//ALIENTEK 探索者STM32F407开发板 实验15
//RTC实时时钟 实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

extern time NetTime;

//USB power IO初始化
void USB_Power_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);//GPIOA15设置低，灯灭

}

int main(void)
{  
	u8 LCD_Display_flag = 1;
	u8 res;
	u8 key_value;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	Alarm_TimeTypeDef Alarm_TimeStruct;
	
	u8 tbuf[40];
	u8 t=0;
	u8 usbPowerState = 0;	
	
	Alarm_TimeStruct.hour = 5;
	Alarm_TimeStruct.minute = 30;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart3_init(115200);
	
	usmart_dev.init(84); 	//初始化USMART	
	LED_Init();					  //初始化LED
 	LCD_Init();					  //初始化LCD
	My_RTC_Init();		 		//初始化RTC
	BEEP_Init();
	KEY_Init();
	USB_Power_Init();
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//配置WAKE UP中断,1秒钟中断一次
	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2014/5/5");

	POINT_COLOR = BLUE;
	sprintf((char*)tbuf, "Alarm time: %02d : %02d %s", Alarm_TimeStruct.hour, Alarm_TimeStruct.minute, (Alarm_TimeStruct.h12 == 0)?"AM":"PM");
	LCD_ShowString(30,300,300,16,24,tbuf);
	
	printf("WiFi checking\n");
	ESP8266_Init();
	printf("WiFi Init OK\n");
  	while(1) 
	{	
		POINT_COLOR = RED;
		t++;
		if((t%10)==0)	//每100ms更新一次显示数据
		{
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
			LCD_ShowString(30,140,210,16,16,tbuf);	
			
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
			LCD_ShowString(30,160,210,16,16,tbuf);	
			sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.RTC_WeekDay); 
			LCD_ShowString(30,180,210,16,16,tbuf);
		} 
		if((RTC_TimeStruct.RTC_Hours == Alarm_TimeStruct.hour) && 
		   (RTC_TimeStruct.RTC_Minutes == Alarm_TimeStruct.minute))
		{
			GPIO_SetBits(GPIOF,GPIO_Pin_8);   //BEEP引脚拉高， 等同BEEP=1;
		}
		delay_ms(10);
		GPIO_ResetBits(GPIOF,GPIO_Pin_8);   //BEEP引脚拉高， 等同BEEP=1;
		delay_ms(10);
		
		key_value = KEY_Scan(0);
		if(key_value == WKUP_PRES)
		{
			if(LCD_Display_flag)
			{
				LCD_Display_flag = 0;
				LCD_LED = 0;
			}
			else
			{
				LCD_Display_flag = 1;
				LCD_LED = 1;
			}
		}
		if(key_value == KEY0_PRES)
		{
			getNetworkTime();
			res = RTC_Set_Time(NetTime.hour, NetTime.min, NetTime.sec, 0);
			printf("\r\nset time = %s\r\n",res?"Success":"Fail");
		}
		if(key_value == KEY2_PRES)
		{
			usbPowerState++;
			if(usbPowerState%2)
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_15);//GPIOA15设置低，灯灭
				printf("USB power on\r\n");
				POINT_COLOR = RED;
				sprintf((char*)tbuf,"USB power on"); 
				LCD_ShowString(30,400,300,16,24,tbuf);	
			}
			else
			{
				GPIO_ResetBits(GPIOA,GPIO_Pin_15);
				printf("USB power off\r\n");
				LCD_Fill(30,400,300,424,WHITE);
			}
		}
	}	
}
