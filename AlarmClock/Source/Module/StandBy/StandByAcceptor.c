#include <string.h>				/* memcpy(),memset()etc */

void TSK_StandBy(void)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char   standbyout_fac;
	unsigned char  i,j,cnt;
#if 0
STANBY:

	s_eStandByStatus = STBY_STS_NONE;

	OS_killResources();

	OS_startTickTimer();

	STBY_checkStandbyIn();
	
	OS_stopTickTimer();

	
	INI_initSuspend();

	
	__DI() ;


	PLT_initInterrupt(PLT_MODE_STANDBY);	/* 割り込み制御レジスタの初期化	*/
	PLT_initPort(PLT_MODE_STANDBY);		/* ポート初期化			*/
	WDT_clearWatchDog();		/* WDTをクリア */
	/*MUT_onAmpMute();*/
	/*************************************************************
		スタンバイモード中
	*************************************************************/
	/* スタンバイ中を保持する期間の設定(STANDBY PASS用) */
	s_standByTimeCount = STBY_TIME_STANDBY_COUNT;
	s_PllClk_Status = 1;
	/*s_STBill_Set_Flag = 0;*/
	s_standbyMode =STBY_IN_TRACING;
	
	/*┌──────────────────┐
	　│移行前の最終確認 & 移行後の退場判定 │
	　└──────────────────┘*/
	standbyout_fac = 0;
	while (1){
		if(SNS_getSenseStatus(SNS_REQ_BACKUP) == SNS_STS_ON)
		{
			/* スタンバイ復帰条件の確認 */
			/* スタンバイ復帰条件の確認 */
			if ((STBY_checkStandByOUT() == TRUE ) && ( s_standByTimeCount <= 0 )) {

				/*┌──────────────────┐
				　│高速発振モード⇒低逓倍PLL高速発振   │
				　└──────────────────┘*/
				WDT_clearWatchDog();		/* WDTをクリア */
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
			unCPU_G2ICR = 0x400F;					/* 割込み要求クリア */
			SNS_execStandbyPortSense();
		}
		TIM_countUpTime_MCU_RTC();
		WDT_clearWatchDog();
	}

	PLT_initPort(PLT_MODE_NORMAL);		/* ポート初期化			*/
	PLT_initInterrupt(PLT_MODE_NORMAL);	/* 割り込み制御レジスタの初期化	*/

	__EI();	/* あくまでもデバッグ用		*/
	
	INI_initResume();
	
	OS_createResources();

	memset(&s_aStandByEvent[0] , 0 , sizeof(s_aStandByEvent));

	ext_tsk();
#endif
}


