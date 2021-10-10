/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2006-2017 by Embedded and Real-Time Systems Laboratory
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
 *		タイマドライバ（RX65N CMTW0タイマ用）
 */


#include "kernel_impl.h"
#include "time_event.h"
#include "target_timer.h"

/*
 *  タイマの起動処理
 */
void
target_hrt_initialize(EXINF exinf)
{
	/*
	 * モジュールストップ機能の設定
	 */
	sil_wrh_mem((void *)SYSTEM_PRCR_ADDR, SYSTEM_PRCR_PRKEY_BITS | SYSTEM_PRCR_PRC1_BIT);	/* 書込み許可 */
	*SYSTEM_MSTPCRA_ADDR &= ~(SYSTEM_MSTPCRA_MSTPA1_BIT);
	sil_wrh_mem((void *)SYSTEM_PRCR_ADDR, SYSTEM_PRCR_PRKEY_BITS);	/* 書込み禁止 */

	/*
	 * タイマ停止
	 */
	*CMTW0_CMWSTR_ADDR &= ~CMTWn_CMWSTR_STR_BIT;

	/*
 	 * CMWCNTカウンタのクリア禁止 (i.e. Free Running)
 	 * NOTE: CMWCR.CKS（クロック選択）はPLL初期化の中で設定済み
	 */
	*CMTW0_CMWCR_ADDR |= 0x1 << CMTWn_CMWCR_CCLR_SHIFT;

	/*
	 * タイマカウンタ設定
	 */
	*CMTW0_CMWCNT_ADDR = 0U;

	/*
	 * コンペア動作を許可
	 * NOTE: リセット後のCMWCORは最大値
	 */
	*CMTW0_CMWIOR_ADDR = CMTWn_CMWIOR_CMWE_BIT;

	/*
	 * 割り込み要求先をCPUに設定
	 */
	*ICU_DTCERn_ADDR(INTNO_TIMER) = 0x0;

	/*
	 * タイマ動作開始前の割込み要求をクリア
	 */
	clear_int(INTNO_TIMER);
	clear_int(INTNO_TIMER_SWI);

	/*
	 * コンペアマッチ割り込み要求を許可
	 */
	*CMTW0_CMWCR_ADDR |= CMTWn_CMWSTR_CMWIE_BIT;

	/*
	 * タイマ動作開始
	 */
	*CMTW0_CMWSTR_ADDR |= CMTWn_CMWSTR_STR_BIT;
}

/*
 *  タイマの停止処理
 */
void
target_hrt_terminate(EXINF exinf)
{
	/*
	 * タイマ停止
	 */
	*CMTW0_CMWSTR_ADDR &= ~CMTWn_CMWSTR_STR_BIT;

	/*
	 * タイマ割込み要求をクリアする．
	 */
	clear_int(INTNO_TIMER);
	clear_int(INTNO_TIMER_SWI);
}

/*
 *  高分解能タイマへの割込みタイミングの設定
 */
void
target_hrt_set_event(HRTCNT hrtcnt)
{
	uint32_t cnt = HRTCNT_TO_CMWCNT(hrtcnt);
	uint32_t current;

	/*
	 *  現在のカウント値を読み，hrtcnt後に割込みが発生するように設定する．
	 */
	current = sil_rew_mem((void*)CMTW0_CMWCNT_ADDR);
	sil_wrw_mem((void*)CMTW0_CMWCOR_ADDR, current + cnt);

	/*
	 *  上で現在のカウント値を読んで以降に，cnt以上カウントアップしてい
	 *  た場合には，割込みを発生させる．
	 */
	if ((sil_rew_mem((void*)CMTW0_CMWCNT_ADDR) - current >= cnt) && !probe_int(INTNO_TIMER)) {
		raise_int(INTNO_TIMER_SWI);
	}
}

/*
 *  タイマ割込みハンドラ
 */
void
target_hrt_handler(void)
{
	/*
	 *  高分解能タイマ割込みを処理する．
	 */
	signal_time();
}

/*
 *  タイムウィンドウタイマの初期化処理
 */
void
target_twdtimer_initialize(EXINF exinf)
{
	/*
	 * モジュールストップ機能の設定
	 */
	sil_wrh_mem((void *)SYSTEM_PRCR_ADDR, SYSTEM_PRCR_PRKEY_BITS | SYSTEM_PRCR_PRC1_BIT);	/* 書込み許可 */
	*SYSTEM_MSTPCRA_ADDR &= ~(SYSTEM_MSTPCRA_MSTPA0_BIT);
	sil_wrh_mem((void *)SYSTEM_PRCR_ADDR, SYSTEM_PRCR_PRKEY_BITS);	/* 書込み禁止 */

	/*
	 * タイマ停止
	 */
	*CMTW1_CMWSTR_ADDR &= ~CMTWn_CMWSTR_STR_BIT;

	/*
 	 * CMWCNTカウンタのクリア禁止 (i.e. Free Running)
 	 * NOTE: CMWCR.CKS（クロック選択）はPLL初期化の中で設定済み
	 */
	*CMTW1_CMWCR_ADDR |= 0x1 << CMTWn_CMWCR_CCLR_SHIFT;

	/*
	 * タイマカウンタ設定
	 */
	*CMTW1_CMWCNT_ADDR = 0U;

	/*
	 * コンペア動作を許可
	 */
	*CMTW1_CMWIOR_ADDR = CMTWn_CMWIOR_CMWE_BIT;

	/*
	 * 割り込み要求先をCPUに設定
	 */
	*ICU_DTCERn_ADDR(INTNO_TWDTIMER) = 0x0;

	/*
	 * タイマ動作開始前の割込み要求をクリア
	 */
	clear_int(INTNO_TWDTIMER);

	/*
	 * コンペアマッチ割り込み要求を許可
	 */
	*CMTW1_CMWCR_ADDR |= CMTWn_CMWSTR_CMWIE_BIT;
}

/*
 *  タイムウィンドウタイマの停止処理
 */
void
target_twdtimer_terminate(EXINF exinf)
{
	/*
	 * タイマ停止
	 */
	*CMTW1_CMWSTR_ADDR &= ~CMTWn_CMWSTR_STR_BIT;

	/*
	 * タイマ割込み要求をクリアする．
	 */
	clear_int(INTNO_TWDTIMER);
}

/*
 *  タイムウィンドウ割込みハンドラ
 */
void
target_twdtimer_handler(void)
{
	/*
	 *  タイムウィンドウ切換え処理をする．
	 */
	twd_switch();
}
