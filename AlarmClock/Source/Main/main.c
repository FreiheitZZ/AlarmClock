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

void TSK_Initial(void *pvParameters);
TaskHandle_t InitialTask_Handler;

TaskHandle_t EventSetBit_Handler;
void eventsetbit_task(void *pvParameters);

TaskHandle_t EventGroupTask_Handler;
void eventgroup_task(void *pvParameters);

////////////////////////////////////////////////
struct taskDefine
{
	TaskFunction_t	func;		//������
	const char*		name;		//��������
	uint16_t		size;		//�����ջ��С
	void*			para;		//���ݸ��������Ĳ���
	UBaseType_t		prio;		//�������ȼ�
	TaskHandle_t*	handler;	//������
};

enum
{
	INITIAL_TASK_NUM,
	SETBIT_TASK_NUM,
	GROUP_TASK_NUM,
	TASK_NUM_MAX,
};

const struct taskDefine taskCfg[TASK_NUM_MAX] = 
{
	{TSK_Initial,		"start_task",		256,	NULL,	1,	&InitialTask_Handler},
	{eventsetbit_task,	"eventsetbit_task",	256,	NULL,	2,	&EventSetBit_Handler},
	{eventgroup_task,	"eventgroup_task",	256,	NULL,	3,	&EventGroupTask_Handler},

};


////////////////////////////////////////////////////////
#define EVENTBIT_0	(1<<0)				//�¼�λ
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };
static	unsigned char		s_aRxData[400];	/* ��M�o�b�t�@(�����O�o�b�t�@)	*/

extern unsigned char		s_EntryNo_usart1;
extern unsigned char		s_EntryNo_usart2;

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);					//��ʼ����ʱ����
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )taskCfg[INITIAL_TASK_NUM].func,
                (const char*    )taskCfg[INITIAL_TASK_NUM].name,
                (uint16_t       )taskCfg[INITIAL_TASK_NUM].size,
                (void*          )taskCfg[INITIAL_TASK_NUM].para,
                (UBaseType_t    )taskCfg[INITIAL_TASK_NUM].prio,
                (TaskHandle_t*  )taskCfg[INITIAL_TASK_NUM].handler);
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void TSK_Initial(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
	
	uart_init(115200);     				//��ʼ������
	LED_Init();		        			//��ʼ��LED�˿�
	KEY_Init();							//��ʼ������
	EXTIX_Init();						//��ʼ���ⲿ�ж�
	LCD_Init();							//��ʼ��LCD
	my_mem_init(SRAMIN);            	//��ʼ���ڲ��ڴ��

	s_EntryNo_usart1 = RB_reqEntry(&s_aRxData[0], sizeof(s_aRxData));
		
    POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"ATK STM32F103/407");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 18-4");
	LCD_ShowString(30,50,200,16,16,"Task Notify Event Group");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2016/11/25");

	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,130,234,314);	//������
	POINT_COLOR = BLUE;
	LCD_ShowString(30,110,220,16,16,"Event Group Value:0");
	
	//���������¼�λ������
    xTaskCreate((TaskFunction_t )taskCfg[SETBIT_TASK_NUM].func,
                (const char*    )taskCfg[SETBIT_TASK_NUM].name,
                (uint16_t       )taskCfg[SETBIT_TASK_NUM].size,
                (void*          )taskCfg[SETBIT_TASK_NUM].para,
                (UBaseType_t    )taskCfg[SETBIT_TASK_NUM].prio,
                (TaskHandle_t*  )taskCfg[SETBIT_TASK_NUM].handler);	
    //�����¼���־�鴦������
	xTaskCreate((TaskFunction_t )taskCfg[GROUP_TASK_NUM].func,
				(const char*	)taskCfg[GROUP_TASK_NUM].name,
				(uint16_t		)taskCfg[GROUP_TASK_NUM].size,
				(void*			)taskCfg[GROUP_TASK_NUM].para,
				(UBaseType_t	)taskCfg[GROUP_TASK_NUM].prio,
				(TaskHandle_t*	)taskCfg[GROUP_TASK_NUM].handler); 

    vTaskDelete(InitialTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//�����¼�λ������
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
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
								(uint32_t		)EVENTBIT_1,			//Ҫ���µ�bit
								(eNotifyAction	)eSetBits);				//����ָ����bit
					break;
				case KEY2_PRES:
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
								(uint32_t		)EVENTBIT_2,			//Ҫ���µ�bit
								(eNotifyAction	)eSetBits);				//����ָ����bit
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
        vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}

//�¼���־�鴦������
void eventgroup_task(void *pvParameters)
{
	u8 num=0,enevtvalue;
	static u8 event0flag,event1flag,event2flag;
	uint32_t NotifyValue;
	BaseType_t err;
	
	while(1)
	{
		//��ȡ����ֵ֪ͨ
		err=xTaskNotifyWait((uint32_t	)0x00,				//���뺯����ʱ���������bit
							(uint32_t	)ULONG_MAX,			//�˳�������ʱ��������е�bit
							(uint32_t*	)&NotifyValue,		//��������ֵ֪ͨ
							(TickType_t	)portMAX_DELAY);	//����ʱ��
		
		if(err==pdPASS)	   //����֪ͨ��ȡ�ɹ�
		{
			if((NotifyValue&EVENTBIT_0)!=0)			//�¼�0����	
			{
				event0flag=1;	
			}				
			else if((NotifyValue&EVENTBIT_1)!=0)	//�¼�1����	
			{
				event1flag=1;
			}
			else if((NotifyValue&EVENTBIT_2)!=0)	//�¼�2����	
			{
				event2flag=1;	
			}
	
			enevtvalue=event0flag|(event1flag<<1)|(event2flag<<2);	//ģ���¼���־��ֵ
			printf("����ֵ֪ͨΪ:%d\r\n",enevtvalue);
			LCD_ShowxNum(174,110,enevtvalue,1,16,0);				//��LCD����ʾ��ǰ�¼�ֵ
			
			if((event0flag==1)&&(event1flag==1)&&(event2flag==1))	//�����¼���ͬʱ����
			{
				num++;
				LED1=!LED1;	
				LCD_Fill(6,131,233,313,lcd_discolor[num%14]);
				event0flag=0;								//��־����
				event1flag=0;
				event2flag=0;
			}
		}
		else
		{
			printf("����֪ͨ��ȡʧ��\r\n");
		}

	}
}

