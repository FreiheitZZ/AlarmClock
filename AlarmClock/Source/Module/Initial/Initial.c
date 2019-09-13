#include "./Initial.h"

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


void INI_initReset(void)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char	cnt;

	/*************************************************************
		時計設定
	*************************************************************/
	//PLT_setClockTimerMode();	/* 時計設定（暫定）		*/

	/*************************************************************
		初期化
	*************************************************************/
	for (cnt = 0 ; cnt < sizeof(initResetCallFunc) / sizeof(void *) ; cnt++){
		initResetCallFunc[cnt]();
	}
	
}

void INI_initResume(void)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char	cnt;
	/*************************************************************
		初期化
	*************************************************************/
	for (cnt = 0 ; cnt < sizeof(execResumeCallFunc) / sizeof(void *) ; cnt++){
		execResumeCallFunc[cnt]();
	}
}

void INI_initSuspend(void)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char	cnt;
	/*************************************************************
		初期化
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



