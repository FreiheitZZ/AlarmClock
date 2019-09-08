#ifndef _UART_H
#define _UART_H

typedef enum{
	UART_PARITY_NON = 0,
	UART_PARITY_ODD,
	UART_PARITY_EVEN
}	EUARTParityType;

typedef enum{
	UART_BITLENGTH_8BIT = 0,
	UART_BITLENGTH_9BIT
}	EUARTBitLengthType;

typedef enum{
	UART_STOPBIT_1 = 0,
	UART_STOPBIT_0_5,
	UART_STOPBIT_2,
	UART_STOPBIT_1_5
}	EUARTStopBitType;

typedef enum{
	UART_MODE_STOP = 0,	/* 停止		*/
	UART_MODE_RECEIVE,	/* 受信のみ	*/
	UART_MODE_TRANSMIT,	/* 送信のみ	*/
	UART_MODE_INTERACTIVE	/* 送受信	*/
}	EUARTComMode;

typedef	enum{
	UART_ERR_NONE = 0,	/* エラーなし		*/
	UART_ERR_PARITY,	/* パリティエラー	*/
	UART_ERR_FRAMING,	/* フレーミングエラー	*/
	UART_ERR_OVERRUN	/* オーバーランエラー	*/
}	EUARTComError;

typedef	enum{
	DMA_STAT_STOP,		/* 転送停止状態（転送可能状態）	*/
	DMA_STAT_RUN		/* 転送中状態   (転送不可状態)	*/
}	EDMATransferState;

typedef	enum{
	UART_STAT_STOP,		/* 転送停止状態（転送可能状態）	*/
	UART_STAT_RUN		/* 転送中状態   (転送不可状態)	*/
}	EUARTTransferState;

typedef	enum{
	DMA_MODE_OFF = 0,	/* DMA未使用	*/
	DMA_MODE_ON_CH0,	/* DMA CH0使用	*/
	DMA_MODE_ON_CH1,	/* DMA CH1使用	*/
	DMA_MODE_ON_CH2,	/* DMA CH2使用	*/
	DMA_MODE_ON_CH3	/* DMA CH3使用	*/
}	EUARTDMAMode;

extern void			UART_entryUart		(unsigned char, unsigned long, EUARTParityType, unsigned char, unsigned char);
extern void			UART_exitUart		(unsigned char);
extern void			UART_stopUart		(unsigned char);
extern void			UART_transmitData(unsigned char ch ,char* fmt,...);
extern EDMATransferState	UART_getDMATransferState(unsigned char);
extern EUARTTransferState	UART_getUARTTransferState(unsigned char);
extern unsigned char		UART_getReceiveData	(unsigned char);
extern EUARTComError		UART_getErrorStatus	(unsigned char);

#endif
