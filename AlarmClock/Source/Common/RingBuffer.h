#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#define	RB_NUM_MAXENTRY		10
#define	RB_VAL_ENTRY_NG		0x00

typedef	enum{
	RB_STS_NORMAL = 0,
	RB_STS_EMPTY,
	RB_STS_FULL
}	ERBStatus;

typedef	struct{
	ERBStatus	status;
	unsigned char *	pAddr;
	unsigned long	size;
	unsigned long	posR;
	unsigned long	posW;
}	SRingBufferInfo;

extern unsigned char	RB_reqEntry		(unsigned char *, unsigned long);
extern void		RB_initRingBuffer	(unsigned char);
extern ERBStatus	RB_getBufferStatus	(unsigned char);
extern unsigned long RB_getBufferRemainSize (unsigned char entryNo);
extern void		RB_putRingBuffer	(unsigned char, unsigned char);
extern unsigned char	RB_getRingBuffer	(unsigned char);

#endif
