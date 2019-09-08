#include <string.h>
#include "./RingBuffer.h"

static SRingBufferInfo	s_aRingBufferInfo[RB_NUM_MAXENTRY];
static unsigned char	s_entryRest = RB_NUM_MAXENTRY;

static unsigned char	RB_getEntryNo(void);

extern unsigned char
RB_reqEntry(unsigned char * pEntryAddr, unsigned long entrySize)
{
	unsigned char	entryNo;
	unsigned char	index;
	
	entryNo = RB_getEntryNo();
	
	if (entryNo != RB_VAL_ENTRY_NG){
		
		index = (entryNo-1);		
		s_aRingBufferInfo[index].pAddr = pEntryAddr;
		s_aRingBufferInfo[index].size  = entrySize;
		
		RB_initRingBuffer(entryNo);
	}
	
	return	entryNo;
}

extern void
RB_initRingBuffer(unsigned char entryNo)
{
	unsigned char	index;
	if((entryNo != RB_VAL_ENTRY_NG)&&(entryNo <= RB_NUM_MAXENTRY)){
		
		index = (entryNo-1);		
		s_aRingBufferInfo[index].status	= RB_STS_EMPTY;
		s_aRingBufferInfo[index].posR	= 0x00000000;
		s_aRingBufferInfo[index].posW	= 0x00000000;
		memset(s_aRingBufferInfo[index].pAddr, 0x00, s_aRingBufferInfo[index].size);
	}
}

extern ERBStatus
RB_getBufferStatus(unsigned char entryNo)
{
	ERBStatus	eStatus;
	unsigned char	index;
	
	index = (entryNo-1);
	eStatus = s_aRingBufferInfo[index].status;
	if(eStatus != RB_STS_FULL){
		eStatus = (s_aRingBufferInfo[index].posW==s_aRingBufferInfo[index].posR?RB_STS_EMPTY:RB_STS_NORMAL);
	}
	
	return	eStatus;
}

extern unsigned long
RB_getBufferRemainSize (unsigned char entryNo)
{
	ERBStatus	eStatus;
	unsigned char	index;
	
	index = (entryNo-1);
	eStatus = s_aRingBufferInfo[index].status;
	if(eStatus != RB_STS_FULL){
		eStatus = (s_aRingBufferInfo[index].posW==s_aRingBufferInfo[index].posR?RB_STS_EMPTY:RB_STS_NORMAL);
		if (eStatus != RB_STS_EMPTY)
		{
			return ((s_aRingBufferInfo[index].posW>s_aRingBufferInfo[index].posR)?\
					(s_aRingBufferInfo[index].size-(s_aRingBufferInfo[index].posW-s_aRingBufferInfo[index].posR)):\
					(s_aRingBufferInfo[index].posR-s_aRingBufferInfo[index].posW));
		}
		else
		{
			return (s_aRingBufferInfo[index].size);
		}
	}
	else
	{
		return 0;
	}
}

extern void
RB_putRingBuffer(unsigned char entryNo ,unsigned char data)
{
	unsigned char	index;
	
	index = (entryNo-1);
	if(s_aRingBufferInfo[index].status != RB_STS_FULL){
		s_aRingBufferInfo[index].pAddr[s_aRingBufferInfo[index].posW] = data;
		s_aRingBufferInfo[index].posW++;
		if(s_aRingBufferInfo[index].posW == s_aRingBufferInfo[index].size){
			s_aRingBufferInfo[index].posW = 0x00000000;
		}
		s_aRingBufferInfo[index].status = (s_aRingBufferInfo[index].posW==s_aRingBufferInfo[index].posR?RB_STS_FULL:RB_STS_NORMAL);
	}
}

extern unsigned char
RB_getRingBuffer(unsigned char entryNo)
{
	unsigned char	data = 0x00;
	unsigned char	index;
	
	index = (entryNo-1);
	if(s_aRingBufferInfo[index].status != RB_STS_EMPTY){
		data = s_aRingBufferInfo[index].pAddr[s_aRingBufferInfo[index].posR];
		s_aRingBufferInfo[index].posR++;
		if(s_aRingBufferInfo[index].posR == s_aRingBufferInfo[index].size){
			s_aRingBufferInfo[index].posR = 0x00000000;
		}
		if(s_aRingBufferInfo[index].status == RB_STS_FULL){
			s_aRingBufferInfo[index].status = RB_STS_NORMAL;
		}
	}
	
	return	data;
}

static unsigned char
RB_getEntryNo(void)
{
	unsigned char	entryNo = RB_VAL_ENTRY_NG;
	
	if(s_entryRest > 0x00){
		entryNo = s_entryRest;
		s_entryRest--;
	}
	
	return	entryNo;
}

