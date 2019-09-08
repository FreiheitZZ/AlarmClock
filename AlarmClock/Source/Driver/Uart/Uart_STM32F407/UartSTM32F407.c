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
	funcname丗MN103_entryUart()
	contents丗UART偺仜ch傪巊梡偡傞(UartA:0乣5丄UartB:6,7)
	堷    悢丗 unsigned char	UART ch		<IN>
		   unsigned long	儃乕儗乕僩	<IN>
		   EUARTParityType	僷儕僥傿	<IN>
		   unsigned char	BIT挿		<IN>
		   unsigned char	僗僩僢僾BIT	<IN>
		   ID (*)(void)		庴怣僴儞僪儔	<IN>
	栠 傝 抣丗側偟
	旛    峫丗側偟
*********************************************************************/
extern void
F407_entryUart( unsigned char	ch,			/* ch		*/
		unsigned long	baudrate,		/* 儃乕儗乕僩	*/
		F407ParityType	eParity,		/* 僷儕僥傿	*/
		unsigned char	bitLength,		/* BIT挿	*/
		unsigned char	stopbit		/* 僗僩僢僾BIT	*/
			 )
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		UART偺仜ch傪巊梡偡傞
	*************************************************************/
	F407_changeMode(ch, F407_MODE_UART);	/* 億乕僩儌乕僪傪乽UART乿偵愗傝懼偊	*/
	F407_setUartConfigration(ch,baudrate, eParity,bitLength,stopbit);
	F407_setUartITConfig(ch);
}

/*********************************************************************
	funcname丗MN103_entryUart()
	contents丗UART偺仜ch傪枹巊梡偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗側偟
	旛    峫丗側偟
*********************************************************************/
extern void
F407_exitUart(unsigned char ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		UART偺仜ch傪枹巊梡偡傞
	*************************************************************/
	F407_changeMode(ch, F407_MODE_PORT);	/* 億乕僩儌乕僪傪乽PORT乿偵愗傝懼偊	*/
}

/*********************************************************************
	funcname丗MN103_stopUart()
	contents丗仜ch偺摦嶌傪掆巭偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗側偟
	旛    峫丗
		亂奐巒/掆巭弴彉亃
		    奐巒帪丗妱傝崬傒偺嫋壜仺UART摦嶌奐巒仺憲庴怣摦嶌奐巒
		    掆巭帪丗憲庴怣摦嶌掆巭仺UART摦嶌掆巭仺妱傝崬傒偺嬛巭
*********************************************************************/
extern void
F407_stopUart(F407UARTChannel ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	/* nothing */
	/*************************************************************
		仜ch偺摦嶌傪掆巭偡傞
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
	funcname丗MN103_transmitData()
	contents丗偙偺僨乕僞傪憲怣偡傞
	堷    悢丗unsigned char		UART ch		<IN>
		  unsigned char*	僨乕僞傾僪儗僗	<IN>
	栠 傝 抣丗側偟
	旛    峫丗側偟
*********************************************************************/
extern void
F407_transmitData(F407UARTChannel ch ,unsigned char * pData ,unsigned int len)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		憲怣偡傞
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
	funcname丗MN103_checkTransmitReady()
	contents丗憲怣壜擻偐偳偆偐傪僠僃僢僋偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗BOOL		憲怣壜擻偐偳偆偐
		  TRUE :	憲怣壜擻
		  FALSE:	憲怣晄壜擻
	旛    峫丗側偟
*********************************************************************/
extern unsigned char 
F407_checkTransmitReady(unsigned char ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	unsigned char result = 0;	/* 弶婜抣丗憲怣晄壜擻 */

	return result;
}

/*********************************************************************
	funcname丗MN103_getReceiveData()
	contents丗庴怣僨乕僞傪庢摼偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗側偟
	旛    峫丗側偟
*********************************************************************/
extern unsigned char
F407_getReceiveData(unsigned char ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	unsigned char	data = 0x00;	/* 庴怣僨乕僞default丗0x00 */
	
	return	data;
}

/*********************************************************************
	funcname丗MN103_getUARTTransferState()
	contents丗UART揮憲忬懺傪庢摼偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗unsigned char		CH枅偺揮憲忬懺
			0x00 : 憲怣壜擻
			0x01 : 憲怣晄壜擻(0x00埲奜偱偁傟偽憲怣晄壜擻偲埖偭偰偄傞)
	旛    峫丗側偟
*********************************************************************/
extern unsigned char
F407_getUARTTransferState(unsigned char ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	unsigned char	status = 0x00;	/* 弶婜抣丗憲怣壜擻 */
	
	/*************************************************************
		忬懺儗僕僗僞撉傒弌偟
	*************************************************************/
	return	status;
}

/*********************************************************************
	funcname丗MN103_getErrorStatus()
	contents丗庴怣僄儔乕忣曬傪庢摼偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗unsigned char		CH枅偺僄儔乕忣曬
	旛    峫丗Bit2丗僷儕僥傿僄儔乕丄Bit1丗僼儗乕儈儞僌僄儔乕丄Bit0丗僆乕僶乕儔儞僄儔乕
	僨乕僞傪庴怣偟偨嵺偼昁偢僄儔乕忣曬偺庢摼傕峴偭偰偔偩偝偄丅
	
*********************************************************************/
extern unsigned char
F407_getErrorStatus(unsigned char ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	unsigned char	error = 0x00;
	
	/*************************************************************
		忬懺儗僕僗僞撉傒弌偟
	*************************************************************/
	return	error;
}

/*********************************************************************
	funcname丗MN103_changeMode()
	contents丗UART儌乕僪偲PORT儌乕僪偺愗傝懼偊傪偡傞
	堷    悢丗unsigned char		UART ch		<IN>
		  EMN103PortMode		億乕僩儌乕僪	<IN>
	栠 傝 抣丗側偟
	旛    峫丗撪晹偱摦嶌傪掆巭偟偰偐傜愗傝懼偊偰傑偡丅
*********************************************************************/
static	void
F407_changeMode(unsigned char ch, F407PortMode mode)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	/* nothing */
	
	/*************************************************************
		摦嶌傪掆巭偡傞
	*************************************************************/
	F407_stopUart(ch);

	
	/*************************************************************
		儌乕僪愗懼
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
	funcname丗MN103_setPortMode()
	contents丗億乕僩儌乕僪偵偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗側偟
	旛    峫丗昁偢摦嶌傪掆巭偟偰偐傜愝掕傪愗傝懼偊傞偙偲丅
*********************************************************************/
static	void
F407_setPortMode(F407UARTChannel ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/*************************************************************
		億乕僩儌乕僪偵偡傞
	*************************************************************/
	F407_stopUart(ch);

	switch (ch)
	{
		case F407_UART_CH1:
			if(F407_UART_CH1_PORT == F407_UART_CH1_PORT_PA_9_10)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOA, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOA,GPIO_Pin_9 | GPIO_Pin_10);//设置低
			}
			else if(F407_UART_CH1_PORT == F407_UART_CH1_PORT_PB_5_6)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOB, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOB,GPIO_Pin_5 | GPIO_Pin_6);//设置低
			}
			else {;}
			break;
		case F407_UART_CH2:
			if(F407_UART_CH2_PORT == F407_UART_CH2_PORT_PA_2_3)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOA, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOA,GPIO_Pin_2 | GPIO_Pin_3);//设置低
			}
			else if(F407_UART_CH2_PORT == F407_UART_CH2_PORT_PD_5_6)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOD, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOD,GPIO_Pin_5 | GPIO_Pin_6);//设置低
			}
			else {;}
			break;
		case F407_UART_CH3:
			if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PB_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOB, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOB,GPIO_Pin_10 | GPIO_Pin_11);//设置低
			}
			else if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PD_8_9)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOD, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOD,GPIO_Pin_8 | GPIO_Pin_9);//设置低
			}
			else if(F407_UART_CH3_PORT == F407_UART_CH3_PORT_PC_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOC, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOC,GPIO_Pin_10 | GPIO_Pin_11);//设置低
			}
			else {;}
			break;
		case F407_UART_CH4:
			if(F407_UART_CH4_PORT == F407_UART_CH4_PORT_PA_0_1)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOA, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1);//设置低
			}
			else if(F407_UART_CH4_PORT == F407_UART_CH4_PORT_PC_10_11)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOC, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOC,GPIO_Pin_10 | GPIO_Pin_11);//设置低
			}
			else {;}
			break;
		case F407_UART_CH5:
			if(F407_UART_CH5_PORT == F407_UART_CH5_PORT_PC_12_PD_2)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOC, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOC,GPIO_Pin_12);//设置低
				
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOD, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);//设置低
			}
			else {;}
			break;		
		case F407_UART_CH6:
			if(F407_UART_CH6_PORT == F407_UART_CH6_PORT_PC_6_7)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOC, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7);//设置低
			}
			else if(F407_UART_CH6_PORT == F407_UART_CH6_PORT_PG_9_14)
			{
				RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
				GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
				GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
				GPIO_Init(GPIOG, &GPIO_InitStructure);//输出化
				GPIO_ResetBits(GPIOG,GPIO_Pin_9 | GPIO_Pin_14);//设置低
			}
			else {;}
			break;
		default:
			break;
	}
}

/*********************************************************************
	funcname丗MN103_setUARTMode()
	contents丗UART儌乕僪偵偡傞
	堷    悢丗unsigned char		UART ch		<IN>
	栠 傝 抣丗側偟
	旛    峫丗昁偢摦嶌傪掆巭偟偰偐傜愝掕傪愗傝懼偊傞偙偲丅
*********************************************************************/
static	void
F407_setUARTMode(F407UARTChannel ch)
{
	/*************************************************************
		帺摦曄悢掕媊
	*************************************************************/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*************************************************************
		UART儌乕僪偵偡傞
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
	funcname丗MN103_setUartConfigration()
	contents丗忦審愝掕
	堷    悢丗 unsigned char	UART ch		<IN>
		   unsigned long	儃乕儗乕僩	<IN>
		   EUARTParityType	僷儕僥傿	<IN>
		   unsigned char	BIT挿		<IN>
		   unsigned char	僗僩僢僾BIT	<IN>
	栠 傝 抣丗側偟
	旛    峫丗側偟
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
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
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
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
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
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
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
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能UART4时钟
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
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//使能UART5时钟
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
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART1时钟
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
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
			
			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
			break;
		case F407_UART_CH2:
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
			
			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
			break;
		case F407_UART_CH3:
			USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
			
			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
			break;
		case F407_UART_CH4:
			USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启相关中断
			
			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
			break;
		case F407_UART_CH5:
			USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断
			
			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
			break;
		case F407_UART_CH6:
			USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断
			
			//Usart1 NVIC 配置
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级7
			NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级0
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		//IRQ通道使能
			NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器、
			break;
		default:
			break;
	}
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		RB_putRingBuffer(s_EntryNo_usart1, Res);
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif

} 

void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
		RB_putRingBuffer(s_EntryNo_usart2, Res);
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
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

