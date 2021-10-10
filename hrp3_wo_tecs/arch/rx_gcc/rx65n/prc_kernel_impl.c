/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 */

/*
 *		カーネルのプロセッサ依存部（RX65N用）
 */

#include "kernel_impl.h"

/*
 *  割込み要求ラインの属性の設定
 *
 *  ASPカーネルでの利用を想定して，パラメータエラーはアサーションでチェッ
 *  クしている．cfg_intサービスコールを設ける場合には，エラーを返すよう
 *  にすべきであろう．
 */
void
config_int( INTNO intno, ATR intatr, PRI intpri )
{
	uint8_t		irqmd;
	uint8_t		iprno;

	assert( VALID_INTNO(intno) );

	assert( ( TIPM_LOCK_ALL <= intpri ) && ( intpri <= TMAX_INTPRI ) );
#if TIPM_LOCK != TIPM_LOCK_ALL
	if( intatr == TA_NONKERNEL ){
		assert( intpri < TMIN_INTPRI );
 	}
	else{
		assert( TMIN_INTPRI <= intpri );
	}
#endif

	/*
	 *  割込みのマスク
	 *
	 *  割込みを受け付けたまま，レベルトリガ／エッジトリガの設定や，割
	 *  込み優先度の設定を行うのは危険なため，割込み属性にかかわらず，
	 *  一旦マスクする．
	 */
	( void )disable_int( intno );

	/*
	 *  レベルトリガ/エッジトリガの設定
	 *
	 *  IRQのみサポートする
	 */
	if( INT_IRQ_MIN <= intno && intno <= INT_IRQ_MAX ){
		if( ( intatr & TA_POSEDGE ) != 0U ){
			irqmd = 2;
		}
		else if( ( intatr & TA_NEGEDGE ) != 0U ){
			irqmd = 1;
		}
		else if( ( intatr & TA_BOTHEDGE ) != 0U ){
			irqmd = 3;
		}
		else if( ( intatr & TA_LOWLEVEL ) != 0U ){
			irqmd = 0;
		}
		else{
			/*
			 *  コンフィギュレータにてチェックしているため,
			 *  ここへくることはありえない.
			 */
			assert(false);
			irqmd = 0U;
		}

		*ICU_IRQCRn_ADDR(intno - INT_IRQ_MIN) = irqmd << ICU_IRQCRn_IRQMD_SHIFT;
	}

	/*
	 *  割込みレベルの設定
	 *
	 *  割込み番号に対応したIPR値を設定する.
	 */
	if (intno < 32) {
		switch (intno) {
		case 16:
		case 18:
			iprno = 0;
			break;
		case 21:
			iprno = 1;
			break;
		case 23:
			iprno = 2;
			break;
		case 26:
		case 27:
			iprno = 3;
			break;
		case 28:
			iprno = 4;
			break;
		case 29:
			iprno = 5;
			break;
		case 30:
			iprno = 6;
			break;
		case 31:
			iprno = 7;
			break;
		default:
			assert(false);
			iprno = 0;
		}
	} else {
		iprno = intno;
	}
	*ICU_IPRr_ADDR(iprno) = (uint8_t)(-intpri);

	/*
	 *  割込みのマスク解除
 	 */
	if( ( intatr & TA_ENAINT ) != 0U ){
		( void )enable_int( intno );
	}
}


/*
 *  プロセッサ依存の初期化
 */
void
prc_initialize(void)
{
	/*
	 *  コア依存の初期化
	 */
	core_initialize();

	/*
	 *  バナー出力のため、SIO初期化を呼び出す
	 */
	sio_initialize();
}
