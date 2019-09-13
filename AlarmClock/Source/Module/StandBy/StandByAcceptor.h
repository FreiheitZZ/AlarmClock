#ifndef STAND_BY_ACCEPTOR_H
#define STAND_BY_ACCEPTOR_H

#define	STBY_TIME_STANDBY_COUNT	(1000/2) /*wait 100ms*/

#define	STBY_TIMER_INVALID		0xffffffff

#define	STBY_TIME_DSENS_OFF_COUNT	(162/2)

typedef	enum{
	STBY_FLG_NO_STBY=1,                             	/*15*/
	STBY_FLG_NO_ADC,       		                      /*20*/
	STBY_FLG_NO_KEYC,       	                       /*21*/
	STBY_FLG_NO_SYSTEM,                           	/*25*/
	STBY_FLG_NO_MUTE,      	                      	/*26*/
	STBY_FLG_NO_ILM,       		                          /*32*/
	STBY_FLG_NO_MM,        	                        /*66*/
	STBY_FLG_NO_EEPROM,  		   	                     /*70*/
	STBY_FLG_NO_END
} EStandByFlgNo;

typedef	enum{
	STBY_ID_STBY ,                                 /*15*/
	STBY_ID_KEYC,                                  /*21*/
	STBY_ID_SYSTEM,                               /*25*/
	STBY_ID_MUTE,                                  /*26*/
	STBY_ID_ILM,          	                        /*32*/
	STBY_ID_MM,                                   /*66*/
	STBY_ID_EEPROM,	                               /*70*/
	STBY_ID_END
} EStandByID;


#endif

