#include "FreeRTOS.h"
#include "task.h"
//#include "../../Platform/RTOS/TaskConfig.h"
#include "./Initial.h"


#include "..\..\Common\sys.h"
#include "..\..\Common\delay.h"
#include "..\..\Common\usart.h"
#include "..\..\Device\led\led.h"
#include "..\..\Device\timer\timer.h"
#include "..\..\Device\lcd\lcd.h"
#include "..\..\Device\key\key.h"
#include "..\..\Device\beep\beep.h"
#include "..\..\Device\exti\exti.h"
#include "string.h"
#include "..\..\Common\malloc.h"

#include "limits.h"
#include "..\..\Common\ringbuffer.h"
#include "..\..\Driver\uart\uart.h"

extern struct taskDefine
{
	TaskFunction_t	func;		//������
	const char*		name;		//��������
	uint16_t		size;		//�����ջ��С
	void*			para;		//���ݸ��������Ĳ���
	UBaseType_t		prio;		//�������ȼ�
	TaskHandle_t*	handler;	//������
};

extern enum
{
	INITIAL_TASK_NUM,
	STANDBY_TASK_NUM,
	SETBIT_TASK_NUM,
	GROUP_TASK_NUM,
	TASK_NUM_MAX,
};

extern const struct taskDefine taskCfg[TASK_NUM_MAX];


void SYS_initReset(void);
void SYS_execResume(void);
void SYS_execSuspend(void);

static void (*const initResetCallFunc[])(void) = {
	{	SYS_initReset	},	/* System		*/
};

static void (*const execResumeCallFunc[])(void) = {
	{	SYS_execResume		},	/* System		*/
};

static void (*const execSuspendCallFunc[])(void) = {
	{	SYS_execSuspend		},	/* System		*/
};

static	unsigned char		s_aRxData[400];	/* ��M�o�b�t�@(�����O�o�b�t�@)	*/

extern unsigned char		s_EntryNo_usart1;
extern unsigned char		s_EntryNo_usart2;
extern TaskHandle_t InitialTask_Handler;


void initialTask_Create(void)
{
	//������ʼ����
	xTaskCreate((TaskFunction_t )taskCfg[INITIAL_TASK_NUM].func,
				(const char*	)taskCfg[INITIAL_TASK_NUM].name,
				(uint16_t		)taskCfg[INITIAL_TASK_NUM].size,
				(void*			)taskCfg[INITIAL_TASK_NUM].para,
				(UBaseType_t	)taskCfg[INITIAL_TASK_NUM].prio,
				(TaskHandle_t*	)taskCfg[INITIAL_TASK_NUM].handler);
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

	#if 1
	//���������¼�λ������
    xTaskCreate((TaskFunction_t )taskCfg[STANDBY_TASK_NUM].func,
                (const char*    )taskCfg[STANDBY_TASK_NUM].name,
                (uint16_t       )taskCfg[STANDBY_TASK_NUM].size,
                (void*          )taskCfg[STANDBY_TASK_NUM].para,
                (UBaseType_t    )taskCfg[STANDBY_TASK_NUM].prio,
                (TaskHandle_t*  )taskCfg[STANDBY_TASK_NUM].handler);	
	#endif

    vTaskDelete(InitialTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


void INI_initReset(void)
{
	/*************************************************************
		�����ϐ���`
	*************************************************************/
	unsigned char	cnt;

	/*************************************************************
		���v�ݒ�
	*************************************************************/
	//PLT_setClockTimerMode();	/* ���v�ݒ�i�b��j		*/

	/*************************************************************
		������
	*************************************************************/
	for (cnt = 0 ; cnt < sizeof(initResetCallFunc) / sizeof(void *) ; cnt++){
		initResetCallFunc[cnt]();
	}
	
}

void INI_initResume(void)
{
	/*************************************************************
		�����ϐ���`
	*************************************************************/
	unsigned char	cnt;
	/*************************************************************
		������
	*************************************************************/
	for (cnt = 0 ; cnt < sizeof(execResumeCallFunc) / sizeof(void *) ; cnt++){
		execResumeCallFunc[cnt]();
	}
}

void INI_initSuspend(void)
{
	/*************************************************************
		�����ϐ���`
	*************************************************************/
	unsigned char	cnt;
	/*************************************************************
		������
	*************************************************************/
	for (cnt = 0 ; cnt < sizeof(execSuspendCallFunc) / sizeof(void *) ; cnt++){
		execSuspendCallFunc[cnt]();
	}
}

void SYS_initReset(void)
{}

void SYS_execResume(void)
{}

void SYS_execSuspend(void)
{}



