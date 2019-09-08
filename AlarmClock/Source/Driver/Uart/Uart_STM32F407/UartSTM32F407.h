#ifndef _UARTSTM32F407_H_
#define _UARTSTM32F407_H_


#define		F407_UART_CH1_PORT		F407_UART_CH1_PORT_PA_9_10
#define		F407_UART_CH2_PORT		F407_UART_CH2_PORT_PA_2_3
#define		F407_UART_CH3_PORT		F407_UART_CH3_PORT_PB_10_11
#define		F407_UART_CH4_PORT		F407_UART_CH4_PORT_PA_0_1
#define		F407_UART_CH5_PORT		F407_UART_CH5_PORT_PC_12_PD_2
#define		F407_UART_CH6_PORT		F407_UART_CH6_PORT_PC_6_7

typedef enum{
	F407_UART_CH1_PORT_PA_9_10 = 0,
	F407_UART_CH1_PORT_PB_5_6
}	F407_UART_CH1_USE_PORT;

typedef enum{
	F407_UART_CH2_PORT_PA_2_3 = 0,
	F407_UART_CH2_PORT_PD_5_6
}	F407_UART_CH2_USE_PORT;

typedef enum{
	F407_UART_CH3_PORT_PB_10_11 = 0,
	F407_UART_CH3_PORT_PD_8_9,
	F407_UART_CH3_PORT_PC_10_11
}	F407_UART_CH3_USE_PORT;

typedef enum{
	F407_UART_CH4_PORT_PA_0_1 = 0,
	F407_UART_CH4_PORT_PC_10_11
}	F407_UART_CH4_USE_PORT;
	
typedef enum{
	F407_UART_CH5_PORT_PC_12_PD_2 = 0
}	F407_UART_CH5_USE_PORT;

typedef enum{
	F407_UART_CH6_PORT_PC_6_7 = 0,
	F407_UART_CH6_PORT_PG_9_14
}	F407_UART_CH6_USE_PORT;

typedef enum{
	F407_MODE_PORT = 0,
	F407_MODE_UART		
}	F407PortMode;

typedef enum{
	F407_UART_CH1 = 1,
	F407_UART_CH2,
	F407_UART_CH3,
	F407_UART_CH4,
	F407_UART_CH5,
	F407_UART_CH6
}	F407UARTChannel;

typedef enum{
	F407_PARITY_NON = 0,
	F407_PARITY_ODD,
	F407_PARITY_EVEN
}	F407ParityType;

typedef enum{
	F407_BITLENGTH_8BIT = 0,
	F407_BITLENGTH_9BIT
}	F407BitLengthType;

typedef enum{
	F407_STOPBIT_1 = 0,
	F407_STOPBIT_0_5,
	F407_STOPBIT_2,
	F407_STOPBIT_1_5
}	F407StopBitType;

typedef enum{
	F407_HARDWARE_FLOW_CONTROL_NONE = 0,
	F407_HARDWARE_FLOW_CONTROL_RTS,
	F407_HARDWARE_FLOW_CONTROL_CTS,
	F407_HARDWARE_FLOW_CONTROL_RTS_CTS
}	F407HardwareFlowControlType;

extern void		F407_entryUart		(unsigned char, unsigned long, F407ParityType, unsigned char, unsigned char);
extern void		F407_exitUart		(unsigned char);
extern void		F407_stopUart		(F407UARTChannel);
extern void		F407_transmitData(F407UARTChannel ch ,unsigned char * pData ,unsigned int len);
extern unsigned char	F407_checkTransmitReady	(unsigned char);
extern unsigned char	F407_getReceiveData	(unsigned char);
extern unsigned char		F407_getUARTTransferState(unsigned char);
extern unsigned char	F407_getErrorStatus	(unsigned char);

#endif/* end of  _UARTSTM32F407_H_ */

