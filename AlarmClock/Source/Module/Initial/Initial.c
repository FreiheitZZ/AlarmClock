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



