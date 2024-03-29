#include "./TaskConfig.h"

////////////////////////////////////////////////

void TSK_Initial(void *pvParameters);
TaskHandle_t InitialTask_Handler;


void TSK_StandBy(void *pvParameters);
TaskHandle_t StandbyTask_Handler;


TaskHandle_t EventSetBit_Handler;
void eventsetbit_task(void *pvParameters);

TaskHandle_t EventGroupTask_Handler;
void eventgroup_task(void *pvParameters);


/*********************************************************************
	funcname：OS_createResources()
	contents：OS資源の生成を行う
	引    数：なし
	戻 り 値：なし
	備    考：
*********************************************************************/
void OS_createResources(void)
{
	unsigned char i;
	
    taskENTER_CRITICAL();           //ｽ�ﾈ�ﾁﾙｽ酩�
	for (i=STANDBY_TASK_NUM+1; i < TASK_NUM_MAX; i++) 
	{
		//ｴｴｽｨﾈﾎﾎ�
		xTaskCreate((TaskFunction_t )taskCfg[i].func,
					(const char*    )taskCfg[i].name,
					(uint16_t		)taskCfg[i].size,
					(void*			)taskCfg[i].para,
					(UBaseType_t    )taskCfg[i].prio,
					(TaskHandle_t*  )taskCfg[i].handler);
	}
    taskEXIT_CRITICAL();            //ﾍﾋｳ�ﾁﾙｽ酩�

}

/*********************************************************************
	funcname：OS_killResources()
	contents：OS資源の停止を行う
	引    数：なし
	戻 り 値：なし
	備    考：
*********************************************************************/
void OS_killResources(void)
{
	unsigned char i;
	
    taskENTER_CRITICAL();           //ｽ�ﾈ�ﾁﾙｽ酩�
	for (i=STANDBY_TASK_NUM+1; i < TASK_NUM_MAX; i++) 
	{
		vTaskDelete(*taskCfg[i].handler); //ﾉｾｳ�ﾈﾎﾎ�
	}
    taskEXIT_CRITICAL();            //ﾍﾋｳ�ﾁﾙｽ酩�

}



