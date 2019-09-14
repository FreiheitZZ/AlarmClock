#ifndef TASK_CONFIG_H
#define TASK_CONFIG_H

#include "FreeRTOS.h"
#include "task.h"


////////////////////////////////////////////////
extern void TSK_Initial(void *pvParameters);
extern TaskHandle_t InitialTask_Handler;


extern void TSK_StandBy(void *pvParameters);
extern TaskHandle_t StandbyTask_Handler;


extern TaskHandle_t EventSetBit_Handler;
extern void eventsetbit_task(void *pvParameters);

extern TaskHandle_t EventGroupTask_Handler;
extern void eventgroup_task(void *pvParameters);


void	OS_startTickTimer	(void);
void	OS_stopTickTimer	(void);


////////////////////////////////////////////////

struct taskDefine
{
	TaskFunction_t	func;		//任务函数
	const char*		name;		//任务名称
	uint16_t		size;		//任务堆栈大小
	void*			para;		//传递给任务函数的参数
	UBaseType_t		prio;		//任务优先级
	TaskHandle_t*	handler;	//任务句柄
};

enum TaskNum
{
	INITIAL_TASK_NUM = 0,
	STANDBY_TASK_NUM,
	SETBIT_TASK_NUM,
	GROUP_TASK_NUM,
	TASK_NUM_MAX,
};

const struct taskDefine taskCfg[TASK_NUM_MAX] = 
{
	{TSK_Initial,		"Initial_task",		256,	NULL,	1,	&InitialTask_Handler},
	{TSK_StandBy,		"Standby_task",		256,	NULL,	1,	&StandbyTask_Handler},
	{eventsetbit_task,	"eventsetbit_task",	256,	NULL,	2,	&EventSetBit_Handler},
	{eventgroup_task,	"eventgroup_task",	256,	NULL,	3,	&EventGroupTask_Handler},

};

////////////////////////////////////////////////


#endif

