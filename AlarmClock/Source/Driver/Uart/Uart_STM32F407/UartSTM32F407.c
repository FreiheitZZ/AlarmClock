#include "stm32f4xx_usart.h"
#include "./UartSTM32F407.h"
#include "..\..\..\Common\ringbuffer.h"

unsigned char		s_EntryNo_usart1;
unsigned char		s_EntryNo_usart2;

static	void	F407_changeMode(unsigned char, F407PortMode);
static	void	F407_setPortMode(unsigned char);
static	void	F407_setUARTMode(unsigned char);
static	void	F407_setUartConfigration(F407UARTChannel, unsigned long, unsigned int, unsigned int, unsigned int);
static	void	F407_setUartITConfig(F407UARTChannel ch);

static void UART_SendDataCH1(unsigned char *data, u16 len);
static void UART_SendDataCH2(unsigned char *data, u16 len);
static void UART_SendDataCH3(unsigned char *data, u16 len);
static void UART_SendDataCH4(unsigned char *data, u16 len);
static void UART_SendDataCH5(unsigned char *data, u16 len);
static void UART_SendDataCH6(unsigned char *data, u16 len);

/*********************************************************************
	funcname：MN103_entryUart()
	contents：UARTの○chを使用する(UartA:0〜5、UartB:6,7)
	引    数： unsigned char	UART ch		<IN>
		   unsigned long	ボーレート	<IN>
		   EUARTParityType	パリティ	<IN>
		   unsigned char	BIT長		<IN>
		   unsigned char	ストップBIT	<IN>
		   ID (*)(void)		受信ハンドラ	<IN>
	戻 り 値：なし
	備    考：なし
*********************************************************************/
extern void
F407_entryUart( unsigned char	ch,			/* ch		*/
		unsigned long	baudrate,		/* ボーレート	*/
		F407ParityType	eParity,		/* パリティ	*/
		unsigned char	bitLength,		/* BIT長	*/
		unsigned char	stopbit		/* ストップBIT	*/
			 )
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		UARTの○chを使用する
	*************************************************************/
	F407_changeMode(ch, F407_MODE_UART);	/* ポートモードを「UART」に切り替え	*/
	F407_setUartConfigration(ch,baudrate, eParity,bitLength,stopbit);
	F407_setUartITConfig(ch);
}

/*********************************************************************
	funcname：MN103_entryUart()
	contents：UARTの○chを未使用する
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：なし
	備    考：なし
*********************************************************************/
extern void
F407_exitUart(unsigned char ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		UARTの○chを未使用する
	*************************************************************/
	F407_changeMode(ch, F407_MODE_PORT);	/* ポートモードを「PORT」に切り替え	*/
}

/*********************************************************************
	funcname：MN103_stopUart()
	contents：○chの動作を停止する
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：なし
	備    考：
		【開始/停止順序】
		    開始時：割り込みの許可→UART動作開始→送受信動作開始
		    停止時：送受信動作停止→UART動作停止→割り込みの禁止
*********************************************************************/
extern void
F407_stopUart(F407UARTChannel ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	/* nothing */
	/*************************************************************
		○chの動作を停止する
	*************************************************************/
	switch ( ch ) {
	case F407_UART_CH1:
		USART_DeInit(USART1);
		break;
	case F407_UART_CH2:
		USART_DeInit(USART2);
		break;
	case F407_UART_CH3:
		USART_DeInit(USART3);
		break;
	case F407_UART_CH4:
		USART_DeInit(UART4);
		break;
	case F407_UART_CH5:
		USART_DeInit(UART5);
		break;
	case F407_UART_CH6:
		USART_DeInit(USART6);
		break;
	default:
		break;

	}
}

/*********************************************************************
	funcname：MN103_transmitData()
	contents：このデータを送信する
	引    数：unsigned char		UART ch		<IN>
		  unsigned char*	データアドレス	<IN>
	戻 り 値：なし
	備    考：なし
*********************************************************************/
extern void
F407_transmitData(F407UARTChannel ch ,unsigned char * pData ,unsigned int len)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		送信する
	*************************************************************/
	switch (ch)
	{
		case F407_UART_CH1:
			UART_SendDataCH1(pData, len);
			break;
		case F407_UART_CH2:
			UART_SendDataCH2(pData, len);
			break;
		case F407_UART_CH3:
			UART_SendDataCH3(pData, len);
			break;
		case F407_UART_CH4:
			UART_SendDataCH4(pData, len);
			break;
		case F407_UART_CH5:
			UART_SendDataCH5(pData, len);
			break;
		case F407_UART_CH6:
			UART_SendDataCH6(pData, len);
			break;
		default:
			break;
	}
}

/*********************************************************************
	funcname：MN103_checkTransmitReady()
	contents：送信可能かどうかをチェックする
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：BOOL		送信可能かどうか
		  TRUE :	送信可能
		  FALSE:	送信不可能
	備    考：なし
*********************************************************************/
extern unsigned char 
F407_checkTransmitReady(unsigned char ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char result = 0;	/* 初期値：送信不可能 */

	return result;
}

/*********************************************************************
	funcname：MN103_getReceiveData()
	contents：受信データを取得する
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：なし
	備    考：なし
*********************************************************************/
extern unsigned char
F407_getReceiveData(unsigned char ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char	data = 0x00;	/* 受信データdefault：0x00 */
	
	return	data;
}

/*********************************************************************
	funcname：MN103_getUARTTransferState()
	contents：UART転送状態を取得する
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：unsigned char		CH毎の転送状態
			0x00 : 送信可能
			0x01 : 送信不可能(0x00以外であれば送信不可能と扱っている)
	備    考：なし
*********************************************************************/
extern unsigned char
F407_getUARTTransferState(unsigned char ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char	status = 0x00;	/* 初期値：送信可能 */
	
	/*************************************************************
		状態レジスタ読み出し
	*************************************************************/
	return	status;
}

/*********************************************************************
	funcname：MN103_getErrorStatus()
	contents：受信エラー情報を取得する
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：unsigned char		CH毎のエラー情報
	備    考：Bit2：パリティエラー、Bit1：フレーミングエラー、Bit0：オーバーランエラー
	データを受信した際は必ずエラー情報の取得も行ってください。
	
*********************************************************************/
extern unsigned char
F407_getErrorStatus(unsigned char ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	unsigned char	error = 0x00;
	
	/*************************************************************
		状態レジスタ読み出し
	*************************************************************/
	return	error;
}

/*********************************************************************
	funcname：MN103_changeMode()
	contents：UARTモードとPORTモードの切り替えをする
	引    数：unsigned char		UART ch		<IN>
		  EMN103PortMode		ポートモード	<IN>
	戻 り 値：なし
	備    考：内部で動作を停止してから切り替えてます。
*********************************************************************/
static	void
F407_changeMode(unsigned char ch, F407PortMode mode)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		動作を停止する
	*************************************************************/
	F407_stopUart(ch);

	
	/*************************************************************
		モード切替
	*************************************************************/
	if ( mode == F407_MODE_PORT ) {
		F407_setPortMode(ch);
	}
	else if ( mode == F407_MODE_UART ) {
		F407_setUARTMode(ch);
	}
	else
	{
		F407_setUARTMode(ch);
	}
}

/*********************************************************************
	funcname：MN103_setPortMode()
	contents：ポートモードにする
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：なし
	備    考：必ず動作を停止してから設定を切り替えること。
*********************************************************************/
static	void
F407_setPortMode(F407UARTChannel ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/*************************************************************
		ポートモードにする
	*************************************************************/
	F407_stopUart(ch);

	switch (ch)
	{
		case F407_UART_CH1:
			if(F407_UART_CH1_PORT == F407_UART_CH1_PORT_PA_9_10)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ﾊｹﾄﾜGPIOAﾊｱﾖﾓ
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOA, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOA,GPIO_Pin_9 | GPIO_Pin_10);//ﾉ靹ﾃｵﾍ
			}
			else if(F407_UART_CH1_PORT == F407_UART_CH1_PORT_PB_5_6)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOB, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOB,GPIO_Pin_5 | GPIO_Pin_6);//ﾉ靹ﾃｵﾍ
			}
			else {;}
			break;
		case F407_UART_CH2:
			if(F407_UART_CH2_PORT == F407_UART_CH2_PORT_PA_2_3)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOA, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOA,GPIO_Pin_2 | GPIO_Pin_3);//ﾉ靹ﾃｵﾍ
			}
			else if(F407_UART_CH2_PORT == F407_UART_CH2_PORT_PD_5_6)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOD, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOD,GPIO_Pin_5 | GPIO_Pin_6);//ﾉ靹ﾃｵﾍ
			}
			else {;}
			break;
		case F407_UART_CH3:
			if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PB_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOB, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOB,GPIO_Pin_10 | GPIO_Pin_11);//ﾉ靹ﾃｵﾍ
			}
			else if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PD_8_9)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOD, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOD,GPIO_Pin_8 | GPIO_Pin_9);//ﾉ靹ﾃｵﾍ
			}
			else if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PC_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOC, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOC,GPIO_Pin_10 | GPIO_Pin_11);//ﾉ靹ﾃｵﾍ
			}
			else {;}
			break;
		case F407_UART_CH4:
			if(F407_UART_CH4_PORT == F407_UART_CH4_PORT_PA_0_1)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOA, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1);//ﾉ靹ﾃｵﾍ
			}
			else if(F407_UART_CH4_PORT == F407_UART_CH4_PORT_PC_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOC, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOC,GPIO_Pin_10 | GPIO_Pin_11);//ﾉ靹ﾃｵﾍ
			}
			else {;}
			break;
		case F407_UART_CH5:
			if(F407_UART_CH5_PORT == F407_UART_CH5_PORT_PC_12_PD_2)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOC, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOC,GPIO_Pin_12);//ﾉ靹ﾃｵﾍ
				
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOD, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);//ﾉ靹ﾃｵﾍ
			}
			else {;}
			break;		
		case F407_UART_CH6:
			if(F407_UART_CH6_PORT == F407_UART_CH6_PORT_PC_6_7)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOC, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7);//ﾉ靹ﾃｵﾍ
			}
			else if(F407_UART_CH6_PORT == F407_UART_CH6_PORT_PG_9_14)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//ﾆﾕﾍｨﾊ莎�ﾄ｣ﾊｽ
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//ﾍﾆﾍ�ﾊ莎�
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//ﾉﾏﾀｭ
				GPIO_Init(GPIOG, &GPIO_InitStructure);//ﾊ莎�ｻｯ
				GPIO_ResetBits(GPIOG,GPIO_Pin_9 | GPIO_Pin_14);//ﾉ靹ﾃｵﾍ
			}
			else {;}
			break;
		default:
			break;
	}
}

/*********************************************************************
	funcname：MN103_setUARTMode()
	contents：UARTモードにする
	引    数：unsigned char		UART ch		<IN>
	戻 り 値：なし
	備    考：必ず動作を停止してから設定を切り替えること。
*********************************************************************/
static	void
F407_setUARTMode(F407UARTChannel ch)
{
	/*************************************************************
		自動変数定義
	*************************************************************/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*************************************************************
		UARTモードにする
	*************************************************************/
	switch (ch){
		case F407_UART_CH1:
			if(F407_UART_CH1_PORT == F407_UART_CH1_PORT_PA_9_10)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
				GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
				GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOA,&GPIO_InitStructure);
			}
			else if(F407_UART_CH1_PORT == F407_UART_CH1_PORT_PB_5_6)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
				GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_USART1);
				GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOB,&GPIO_InitStructure);
			}
			else {;}
			break;
		case F407_UART_CH2:
			if(F407_UART_CH2_PORT == F407_UART_CH2_PORT_PA_2_3)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
				GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
				GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOA,&GPIO_InitStructure);
			}
			else if(F407_UART_CH2_PORT == F407_UART_CH2_PORT_PD_5_6)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
				GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
				GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOD,&GPIO_InitStructure);
			}
			else {;}
			break;
		case F407_UART_CH3:
			if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PB_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
				GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
				GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOB,&GPIO_InitStructure);
			}
			else if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PD_8_9)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
				GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
				GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOD,&GPIO_InitStructure);
			}
			else if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PC_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_USART3);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_USART3);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOC,&GPIO_InitStructure);
			}
			else {;}
			break;
		case F407_UART_CH4:
			if(F407_UART_CH4_PORT == F407_UART_CH4_PORT_PA_0_1)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
				GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4);
				GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOA,&GPIO_InitStructure);
			}
			else if(F407_UART_CH4_PORT == F407_UART_CH4_PORT_PC_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOC,&GPIO_InitStructure);
			}
			else {;}
			break;
		case F407_UART_CH5:
			if(F407_UART_CH5_PORT == F407_UART_CH5_PORT_PC_12_PD_2)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOC,&GPIO_InitStructure);
				
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
				GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOD,&GPIO_InitStructure);
			}
			else {;}
			break;		
		case F407_UART_CH6:
			if(F407_UART_CH6_PORT == F407_UART_CH6_PORT_PC_6_7)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);
				GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOC,&GPIO_InitStructure);
			}
			else if(F407_UART_CH6_PORT == F407_UART_CH6_PORT_PG_9_14)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
				GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_UART4);
				GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_UART4);
				
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
				GPIO_Init(GPIOG,&GPIO_InitStructure);
			}
			else {;}
			break;
		default:
			break;
	}
}

/*********************************************************************
	funcname：MN103_setUartConfigration()
	contents：条件設定
	引    数： unsigned char	UART ch		<IN>
		   unsigned long	ボーレート	<IN>
		   EUARTParityType	パリティ	<IN>
		   unsigned char	BIT長		<IN>
		   unsigned char	ストップBIT	<IN>
	戻 り 値：なし
	備    考：なし
*********************************************************************/
static void
F407_setUartConfigration(F407UARTChannel ch,
			unsigned long		baudrate,
			unsigned int		parity,
			unsigned int		bitLength,
			unsigned int		stopbit	)
{
	
	USART_InitTypeDef USART_InitStructure;
	
	switch (ch){
		case F407_UART_CH1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ﾊｹﾄﾜUSART1ﾊｱﾖﾓ
			USART_InitStructure.USART_BaudRate = baudrate;
			USART_InitStructure.USART_WordLength = bitLength;
			USART_InitStructure.USART_StopBits = stopbit;
			USART_InitStructure.USART_Parity = parity;
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART1, &USART_InitStructure);
			USART_Cmd(USART1, ENABLE);
			break;
		case F407_UART_CH2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ﾊｹﾄﾜUSART2ﾊｱﾖﾓ
			USART_InitStructure.USART_BaudRate = baudrate;
			USART_InitStructure.USART_WordLength = bitLength;
			USART_InitStructure.USART_StopBits = stopbit;
			USART_InitStructure.USART_Parity = parity;
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART2, &USART_InitStructure);
			USART_Cmd(USART2, ENABLE);
			break;
		case F407_UART_CH3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ﾊｹﾄﾜUSART3ﾊｱﾖﾓ
			USART_InitStructure.USART_BaudRate = baudrate;
			USART_InitStructure.USART_WordLength = bitLength;
			USART_InitStructure.USART_StopBits = stopbit;
			USART_InitStructure.USART_Parity = parity;
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART3, &USART_InitStructure);
			USART_Cmd(USART3, ENABLE);
			break;
		case F407_UART_CH4:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ﾊｹﾄﾜUART4ﾊｱﾖﾓ
			USART_InitStructure.USART_BaudRate = baudrate;
			USART_InitStructure.USART_WordLength = bitLength;
			USART_InitStructure.USART_StopBits = stopbit;
			USART_InitStructure.USART_Parity = parity;
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(UART4, &USART_InitStructure);
			USART_Cmd(UART4, ENABLE);
			break;
		case F407_UART_CH5:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ﾊｹﾄﾜUART5ﾊｱﾖﾓ
			USART_InitStructure.USART_BaudRate = baudrate;
			USART_InitStructure.USART_WordLength = bitLength;
			USART_InitStructure.USART_StopBits = stopbit;
			USART_InitStructure.USART_Parity = parity;
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(UART5, &USART_InitStructure);
			USART_Cmd(UART5, ENABLE);
			break;
		case F407_UART_CH6:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ﾊｹﾄﾜUSART1ﾊｱﾖﾓ
			USART_InitStructure.USART_BaudRate = baudrate;
			USART_InitStructure.USART_WordLength = bitLength;
			USART_InitStructure.USART_StopBits = stopbit;
			USART_InitStructure.USART_Parity = parity;
			USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART6, &USART_InitStructure);
			USART_Cmd(USART6, ENABLE);
			break;
		default:
			break;
	}
}

static void
F407_setUartITConfig(F407UARTChannel ch)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	switch (ch){
		case F407_UART_CH1:
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ｿｪﾆ�ﾏ犹ﾘﾖﾐｶﾏ
			
			//Usart1 NVIC ﾅ葷ﾃ
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ｴｮｿﾚ1ﾖﾐｶﾏﾍｨｵﾀ
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//ﾇﾀﾕｼﾓﾅﾏﾈｼｶ7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//ﾗﾓﾓﾅﾏﾈｼｶ0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQﾍｨｵﾀﾊｹﾄﾜ
			NVIC_Init(&NVIC_InitStructure); //ｸ�ｾﾝﾖｸｶｨｵﾄｲﾎﾊ�ｳ�ﾊｼｻｯVICｼﾄｴ貳�｡｢
			break;
		case F407_UART_CH2:
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ｿｪﾆ�ﾏ犹ﾘﾖﾐｶﾏ
			
			//Usart1 NVIC ﾅ葷ﾃ
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ｴｮｿﾚ1ﾖﾐｶﾏﾍｨｵﾀ
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//ﾇﾀﾕｼﾓﾅﾏﾈｼｶ7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//ﾗﾓﾓﾅﾏﾈｼｶ0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQﾍｨｵﾀﾊｹﾄﾜ
			NVIC_Init(&NVIC_InitStructure); //ｸ�ｾﾝﾖｸｶｨｵﾄｲﾎﾊ�ｳ�ﾊｼｻｯVICｼﾄｴ貳�｡｢
			break;
		case F407_UART_CH3:
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//ｿｪﾆ�ﾏ犹ﾘﾖﾐｶﾏ
			
			//Usart1 NVIC ﾅ葷ﾃ
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ｴｮｿﾚ1ﾖﾐｶﾏﾍｨｵﾀ
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//ﾇﾀﾕｼﾓﾅﾏﾈｼｶ7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//ﾗﾓﾓﾅﾏﾈｼｶ0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQﾍｨｵﾀﾊｹﾄﾜ
			NVIC_Init(&NVIC_InitStructure); //ｸ�ｾﾝﾖｸｶｨｵﾄｲﾎﾊ�ｳ�ﾊｼｻｯVICｼﾄｴ貳�｡｢
			break;
		case F407_UART_CH4:
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//ｿｪﾆ�ﾏ犹ﾘﾖﾐｶﾏ
			
			//Usart1 NVIC ﾅ葷ﾃ
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ｴｮｿﾚ1ﾖﾐｶﾏﾍｨｵﾀ
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//ﾇﾀﾕｼﾓﾅﾏﾈｼｶ7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//ﾗﾓﾓﾅﾏﾈｼｶ0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQﾍｨｵﾀﾊｹﾄﾜ
			NVIC_Init(&NVIC_InitStructure); //ｸ�ｾﾝﾖｸｶｨｵﾄｲﾎﾊ�ｳ�ﾊｼｻｯVICｼﾄｴ貳�｡｢
			break;
		case F407_UART_CH5:
			USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//ｿｪﾆ�ﾏ犹ﾘﾖﾐｶﾏ
			
			//Usart1 NVIC ﾅ葷ﾃ
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ｴｮｿﾚ1ﾖﾐｶﾏﾍｨｵﾀ
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//ﾇﾀﾕｼﾓﾅﾏﾈｼｶ7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//ﾗﾓﾓﾅﾏﾈｼｶ0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQﾍｨｵﾀﾊｹﾄﾜ
			NVIC_Init(&NVIC_InitStructure); //ｸ�ｾﾝﾖｸｶｨｵﾄｲﾎﾊ�ｳ�ﾊｼｻｯVICｼﾄｴ貳�｡｢
			break;
		case F407_UART_CH6:
			USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//ｿｪﾆ�ﾏ犹ﾘﾖﾐｶﾏ
			
			//Usart1 NVIC ﾅ葷ﾃ
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//ｴｮｿﾚ1ﾖﾐｶﾏﾍｨｵﾀ
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//ﾇﾀﾕｼﾓﾅﾏﾈｼｶ7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//ﾗﾓﾓﾅﾏﾈｼｶ0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQﾍｨｵﾀﾊｹﾄﾜ
			NVIC_Init(&NVIC_InitStructure); //ｸ�ｾﾝﾖｸｶｨｵﾄｲﾎﾊ�ｳ�ﾊｼｻｯVICｼﾄｴ貳�｡｢
			break;
		default:
			break;
	}
}

void USART1_IRQHandler(void)                	//ｴｮｿﾚ1ﾖﾐｶﾏｷ�ﾎ�ｳﾌﾐ�
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//ﾈ郢�SYSTEM_SUPPORT_OSﾎｪﾕ譽ｬﾔ�ﾐ靨ｪﾖｧｳﾖOS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //ｽﾓﾊﾕﾖﾐｶﾏ(ｽﾓﾊﾕｵｽｵﾄﾊ�ｾﾝｱﾘﾐ�ﾊﾇ0x0d 0x0aｽ睾ｲ)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//ｶﾁﾈ｡ｽﾓﾊﾕｵｽｵﾄﾊ�ｾﾝ
		RB_putRingBuffer(s_EntryNo_usart1, Res);
	}
#if SYSTEM_SUPPORT_OS 	//ﾈ郢�SYSTEM_SUPPORT_OSﾎｪﾕ譽ｬﾔ�ﾐ靨ｪﾖｧｳﾖOS.
	OSIntExit();  											 
#endif

} 

void USART2_IRQHandler(void)                	//ｴｮｿﾚ1ﾖﾐｶﾏｷ�ﾎ�ｳﾌﾐ�
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//ﾈ郢�SYSTEM_SUPPORT_OSﾎｪﾕ譽ｬﾔ�ﾐ靨ｪﾖｧｳﾖOS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //ｽﾓﾊﾕﾖﾐｶﾏ(ｽﾓﾊﾕｵｽｵﾄﾊ�ｾﾝｱﾘﾐ�ﾊﾇ0x0d 0x0aｽ睾ｲ)
	{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//ｶﾁﾈ｡ｽﾓﾊﾕｵｽｵﾄﾊ�ｾﾝ
		RB_putRingBuffer(s_EntryNo_usart2, Res);
	}
#if SYSTEM_SUPPORT_OS 	//ﾈ郢�SYSTEM_SUPPORT_OSﾎｪﾕ譽ｬﾔ�ﾐ靨ｪﾖｧｳﾖOS.
	OSIntExit();  											 
#endif
} 

void USART3_IRQHandler(void)
{
#if 0
	u8 res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		res =USART_ReceiveData(USART3);
		if((USART3_RX_STA&(1<<15))==0)
		{
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)
			{
				TIM_SetCounter(TIM7,0);
				if(USART3_RX_STA==0)
					TIM_Cmd(TIM7, ENABLE);
				USART3_RX_BUF[USART3_RX_STA++]=res;
			}else
			{
				USART3_RX_STA|=1<<15;
			}
		}
	}
#endif
}

static void UART_SendDataCH1(unsigned char *data, u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
		USART_SendData(USART1,*(data+j));
	}
}

static void UART_SendDataCH2(unsigned char *data, u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
		USART_SendData(USART2,*(data+j));
	}
}

static void UART_SendDataCH3(unsigned char *data, u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);
		USART_SendData(USART3,*(data+j));
	}
}

static void UART_SendDataCH4(unsigned char *data, u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);
		USART_SendData(UART4,*(data+j));
	}
}

static void UART_SendDataCH5(unsigned char *data, u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET);
		USART_SendData(UART5,*(data+j));
	}
}

static void UART_SendDataCH6(unsigned char *data, u16 len)
{
	u16 j;
	for(j=0;j<len;j++)
	{
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)==RESET);
		USART_SendData(USART6,*(data+j));
	}
}

