#include <string.h>				/* memcpy(),memset()etc */
//#include "../../Platform/RTOS/TaskConfig.h"
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t StandbyTask_Handler;

void TSK_StandBy(void *pvParameters)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	unsigned char   standbyout_fac;
	unsigned char  i,j,cnt;
	
	OS_createResources();

	vTaskDelete(StandbyTask_Handler); //删除开始任务

#if 0
STANBY:

	s_eStandByStatus = STBY_STS_NONE;

	OS_killResources();

	OS_startTickTimer();

	STBY_checkStandbyIn();
	
	OS_stopTickTimer();

	
	INI_initSuspend();

	
	__DI() ;


	PLT_initInterrupt(PLT_MODE_STANDBY);	/* 妱傝崬傒惂屼儗僕僗僞偺弶婜壔	*/
	PLT_initPort(PLT_MODE_STANDBY);		/* 億乕僩弶婜壔			*/
	WDT_clearWatchDog();		/* WDT傪僋儕傾 */
	/*MUT_onAmpMute();*/
	/*************************************************************
		僗僞儞僶僀儌乕僪拞
	*************************************************************/
	/* 僗僞儞僶僀拞傪曐帩偡傞婜娫偺愝掕(STANDBY PASS梡) */
	s_standByTimeCount = STBY_TIME_STANDBY_COUNT;
	s_PllClk_Status = 1;
	/*s_STBill_Set_Flag = 0;*/
	s_standbyMode =STBY_IN_TRACING;
	
	/*劇劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劉
	丂劆堏峴慜偺嵟廔妋擣 & 堏峴屻偺戅応敾掕 劆
	丂劋劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劊*/
	standbyout_fac = 0;
	while (1){
		if(SNS_getSenseStatus(SNS_REQ_BACKUP) == SNS_STS_ON)
		{
			/* 僗僞儞僶僀暅婣忦審偺妋擣 */
			/* 僗僞儞僶僀暅婣忦審偺妋擣 */
			if ((STBY_checkStandByOUT() == TRUE ) && ( s_standByTimeCount <= 0 )) {

				/*劇劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劉
				丂劆崅懍敪怳儌乕僪佀掅掽攞PLL崅懍敪怳   劆
				丂劋劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劅劊*/
				WDT_clearWatchDog();		/* WDT傪僋儕傾 */
				PLT_set_pll_start();
				standbyout_fac = 2;
				s_standbyMode = STBY_OUT_FIX ;
				break;
			}

			if(STBY_IN_TRACING == s_standbyMode)
			{
				
				if ( s_standByTimeCount > 0 ){
					STBY_SubStandbyMode();
					PLT_set_pll_start();
				}
				else
				{
					bCPU_TM3IE = 0;
					s_standbyMode = STBY_IN_FIX ;
					STBY_exeStandByMode();
					s_standbyMode = STBY_OUT_TRACING ;
					bCPU_TM3IE = 1;
					bCPU_TM4IE = 1;
					/*s_STBill_Set_Flag = 0;*/
				}
					
				if((TRUE == STBY_checkTimerOverFlow())&&(STBY_IN_TRACING == s_standbyMode))
				{
					if ( s_standByTimeCount > 0 ) {
						s_standByTimeCount = s_standByTimeCount -80;
					}
				}
					

				if(PLT_checkPortActivation(ASENS) == TRUE)
				{
					bCPU_TM3IE = 1;
					s_standByTimeCount = STBY_TIME_STANDBY_COUNT-80;
					s_standbyMode = STBY_OUT_TRACING ;
				}
				else{
					
					s_standbyMode = STBY_IN_TRACING ;
				}

				
			}
			else if(STBY_IN_FIX == s_standbyMode)
			{

			}
			else if(STBY_OUT_TRACING == s_standbyMode)
			{

				if(TRUE == STBY_checkTimerOverFlow())
				{
					if ( s_standByTimeCount > 0 ) {
						s_standByTimeCount = s_standByTimeCount -80;
					}
				}

				if(PLT_checkPortActivation(ASENS) == TRUE)
				{

				}
				else{
					s_standByTimeCount = STBY_TIME_STANDBY_COUNT;
					s_standbyMode = STBY_IN_TRACING ;
				}

			}
			else if(STBY_OUT_FIX == s_standbyMode)
			{

			}


			
		}
		else
		{
			s_standByTimeCount = STBY_TIME_STANDBY_COUNT;
			s_standbyMode = STBY_IN_FIX ;
			STBY_exeStandByMode();
			s_standbyMode = STBY_OUT_TRACING ;
		}
		
		if(TRUE == STBY_checkTimerOverFlow())
		{
			unCPU_G2ICR = 0x400F;					/* 妱崬傒梫媮僋儕傾 */
			SNS_execStandbyPortSense();
		}
		TIM_countUpTime_MCU_RTC();
		WDT_clearWatchDog();
	}

	PLT_initPort(PLT_MODE_NORMAL);		/* 億乕僩弶婜壔			*/
	PLT_initInterrupt(PLT_MODE_NORMAL);	/* 妱傝崬傒惂屼儗僕僗僞偺弶婜壔	*/

	__EI();	/* 偁偔傑偱傕僨僶僢僌梡		*/
	
	INI_initResume();
	
	OS_createResources();

	memset(&s_aStandByEvent[0] , 0 , sizeof(s_aStandByEvent));

	ext_tsk();
#endif
}


