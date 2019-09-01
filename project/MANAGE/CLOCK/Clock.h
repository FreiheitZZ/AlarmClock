#ifndef __CLOCK_H
#define __CLOCK_H

#include "rtc.h"
#include "sys.h"

typedef struct{
	unsigned char h12;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
}Alarm_TimeTypeDef;



#endif
