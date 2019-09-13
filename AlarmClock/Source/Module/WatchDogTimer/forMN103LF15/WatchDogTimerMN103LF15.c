#include "./WatchDogTimerMN103LF15.h"

/*********************************************************************
	funcname：clearWatchDog
	contents：ウォッチドッグ・タイマ・カウンタのクリア(開始)
	引    数：なし
	戻 り 値：なし
	備    考：なし
*********************************************************************/
extern	void clearWatchDog( void )
{

//	ucCPU_PRTKEY = 0xAA;				/* WD2CLRレジスタ書込み許可 */
//	ucCPU_WD2CLR = 0x55;				/* ウオッチドッグタイマ2クリアレジスタ カウンタクリア */
//	ucCPU_PRTKEY = 0xFF;				/* WDCTRレジスタ書込み許可 */

}

