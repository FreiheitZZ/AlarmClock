#ifndef _STANDBY_SEQUENCER_H
#define _STANDBY_SEQUENCER_H

typedef	struct{
	EStandByID	id;
	unsigned long	waitTime;
} SStandByWaitInfo;

extern	void	STBY_exeBootSequence(unsigned char * , SStandByWaitInfo *);
extern	void	STBY_exeShutdownSequence(unsigned char * , SStandByWaitInfo *);

#endif

