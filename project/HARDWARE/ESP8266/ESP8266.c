#include "esp8266.h"
#include "usart3.h"
#include "delay.h"
#include <stdlib.h>
#include <string.h>
#include "usart.h"

time NetTime;
	
extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN];

u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

u8 ESP8266_SendCmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	u3_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART3_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 

void ESP8266_Init(void)
{
  u8 state=0;
  
//	state = ESP8266_SendCmd("AT","OK",1000);
//	if(state) printf("AT ERR\r\n");
	
//  state = ESP8266_SendCmd("AT+CWMODE=3","OK",200);   //Set WiFi mode 3 SoftAP+Station mode
//	if(state) printf("CWMODE ERR\r\n");
	
//  state = ESP8266_SendCmd("AT+RST","OK",200);
//	if(state) printf("RST ERR\r\n");
	
  state = ESP8266_SendCmd("AT+CIPMUX=0","OK",500);
	if(state) printf("CIPMUX ERR\r\n");
	
//	ESP8266_SendCmd("AT+CWJAP=\"Xiaomi_796D\",\"39251230\"","OK",1000);  // Connect router
//	if(state) printf("CWJAP ERR\r\n");
	delay_ms(1000);
//	state = ESP8266_SendCmd("AT+CIPSTART=\"UDP\",\"1.cn.pool.ntp.org\",123","0,CONNECT",1000);
//	if(state) printf("CIPSTART ERR\r\n");
		delay_ms(1000);
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	u3_printf("AT+CIPSTART=\"UDP\",\"1.cn.pool.ntp.org\",123\r\n");
	delay_ms(1000);
//	if(USART3_RX_STA&0X8000)//接收到期待的应答结果
//	{
//		printf("ack:%s\r\n",USART3_RX_BUF);
//		USART3_RX_STA=0;
//	} 
	
}

void U1Putchar(char data)
{
	while((USART3->SR&0X40)==0){}
	USART3->DR=data;
}

void getNetworkTime()
{
	u32 timeOut=0xffffff, rlen;
	u8 packetBuffer[48];
	int i;
    int month[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	
	u32 yearSec, year=1900;
	
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	memset(packetBuffer,0,sizeof(packetBuffer));
	ESP8266_SendCmd("AT+CIPSEND=48","OK",100);
		
	packetBuffer[0] = 0xe3;  // LI, Version, Mode
	packetBuffer[1] = 0x00;            // Stratum, or type of clock
	packetBuffer[2] = 0x06;            // Polling Interval
	packetBuffer[3] = 0xEC;         // Peer Clock Precision
	packetBuffer[12] = 0x31; 
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 0x31;
	packetBuffer[15] = 0x34;
	USART3_RX_STA = 0;
	
	memset(USART3_RX_BUF,0,sizeof(USART3_RX_BUF));
	for(i=0; i<48; i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //等待上次传输完成 
			USART_SendData(USART3,(uint8_t)packetBuffer[i]); 	 //发送数据到串口3 
	}	
	while(!(USART3_RX_STA&0X8000));		//接收到一次数据了
	
	for(i=0; i<400; i++)
	{
		if(USART3_RX_BUF[i] == 0x24)
		{
			NetTime.secTemp = (u8)USART3_RX_BUF[i+32];
			NetTime.secTemp <<= 8;
			NetTime.secTemp |= (u8)USART3_RX_BUF[i+33];
			NetTime.secTemp <<= 8;
			NetTime.secTemp |= (u8)USART3_RX_BUF[i+34];
			NetTime.secTemp <<= 8;
			NetTime.secTemp |= (u8)USART3_RX_BUF[i+35];
			break;
		}
	}
	if(i >= 400)
		exit(-1);
	NetTime.secTemp += 28800;  //UTC/GMT+08:00 8h==28800sec
	while(1)
	{
		if(((0 == year%4) && (0 != year%100)) || 0==year%400)
		{
			yearSec = 31622400;
		}
		else yearSec = 31536000;
		if(NetTime.secTemp < yearSec) break;
		else
		{
			NetTime.secTemp -= yearSec;
			year++;
		}
	} // while(1)
	NetTime.year = year;

	if(((0 == year%4) && (0 != year%100)) || 0==year%400)
	{
		month[1] = 29;
	}

	for(i=0;i<12;i++)
	{
		if(NetTime.secTemp < month[i]*86400)  //There are 86400sec in 1 day;
			break;
		else
			NetTime.secTemp -= month[i]*86400;
	}

	NetTime.month = i;

	NetTime.day = NetTime.secTemp/86400 + 1;
	NetTime.secTemp = NetTime.secTemp % 86400;
	NetTime.hour = NetTime.secTemp/3600;
	NetTime.secTemp = NetTime.secTemp%3600;
	NetTime.min = NetTime.secTemp/60;
	NetTime.sec = NetTime.secTemp%60;

	printf("hour: %d\nmin: %d\nsec: %d\n", NetTime.hour, NetTime.min, NetTime.sec);
	

}







