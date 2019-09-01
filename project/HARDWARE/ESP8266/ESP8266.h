#ifndef __ESP8266_H
#define __ESP8266_H

#include "sys.h"

typedef struct
{
    u32 secTemp;
    u32 year;
    u32 month;
    u32 day;
    u32 hour;
    u32 min;
    u32 sec;

}time;


void ESP8266_Init(void);
void getNetworkTime(void);



#endif