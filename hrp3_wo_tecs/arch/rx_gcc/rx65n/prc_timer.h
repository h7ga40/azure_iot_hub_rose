/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2016 by Embedded and Real-Time Systems Laboratory
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
 *		タイマドライバ（RX65N CMTW0用）
 */

#ifndef TOPPERS_PRC_TIMER_H
#define TOPPERS_PRC_TIMER_H

#include <sil.h>

/*
 *  タイマ割込みハンドラ登録のための定数
 */
#define INHNO_TIMER		INT_CMWI0			/* 割込みハンドラ番号 */
#define INTNO_TIMER		INT_CMWI0			/* 割込み番号 */
#define INHNO_TIMER_SWI	INT_SWINT			/* 割込みハンドラ番号 （ソフトウエア割込み） */
#define INTNO_TIMER_SWI	INT_SWINT			/* 割込み番号 （ソフトウエア割込み） */
#define INTPRI_TIMER	(TMAX_INTPRI - 1)	/* 割込み優先度 */
#define INTATR_TIMER	TA_NULL				/* 割込み属性 */

/*
 *  タイムウィンドウタイマ割込みハンドラ登録のための定数
 *
 *  タイムウィンドウタイマ割込みの優先度は，高分解能タイマ割込みと同じ
 *  にしなければならない．
 */
#define INHNO_TWDTIMER	INT_CMWI1			/* 割込みハンドラ番号 */
#define INTNO_TWDTIMER	INT_CMWI1			/* 割込み番号 */
#define INTPRI_TWDTIMER	INTPRI_TIMER		/* 割込み優先度 */
#define INTATR_TWDTIMER	TA_NULL				/* 割込み属性 */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  高分解能タイマの起動処理
 */
extern void	target_hrt_initialize(EXINF exinf);

/*
 *  高分解能タイマの停止処理
 */
extern void	target_hrt_terminate(EXINF exinf);

/*
 *  高分解能タイマの現在のカウント値の読出し
 */
Inline HRTCNT
target_hrt_get_current(void)
{
#if TCYC_HRTCNT == 572662306U
	uint32_t cnt = sil_rew_mem((void*)CMTW0_CMWCNT_ADDR);
	if (cnt == 0xFFFFFFFFU) return (TCYC_HRTCNT - 1U);
	return((HRTCNT)CMWCNT_TO_HRTCNT(cnt));
#else
#error TCYC_HRTCNT must be 572662306.
#endif
}

/*
 *  高分解能タイマへの割込みタイミングの設定
 *
 *  高分解能タイマを，hrtcntで指定した値カウントアップしたら割込みを発
 *  生させるように設定する．
 */
extern void target_hrt_set_event(HRTCNT hrtcnt);

/*
 *  高分解能タイマ割込みの要求
 */
Inline void
target_hrt_raise_event(void)
{
	raise_int(INTNO_TIMER_SWI);
}

/*
 *  高分解能タイマ割込みハンドラ
 */
extern void	target_hrt_handler(void);

/*
 *  タイムウィンドウタイマの初期化処理
 */
extern void target_twdtimer_initialize(EXINF exinf);

/*
 *  タイムウィンドウタイマの停止処理
 */
extern void target_twdtimer_terminate(EXINF exinf);

/*
 *  タイムウィンドウタイマの動作開始
 */
Inline void
target_twdtimer_start(PRCTIM twdtim)
{
	uint32_t cmwcnt = ((twdtim == 0) ? 1U : PRCTIM_TO_CMWCNT(twdtim));

	/*
	 * CMWCORを設定
	 */
	sil_wrw_mem((void*)CMTW1_CMWCOR_ADDR, cmwcnt);

	/*
	 * タイマ動作開始
	 */
	*CMTW1_CMWSTR_ADDR |= CMTWn_CMWSTR_STR_BIT;
}

/*
 *  タイムウィンドウタイマの停止
 */
Inline PRCTIM
target_twdtimer_stop(void)
{
	uint32_t current, compare;

	/*
	 *  現在のカウント値を読み
	 */
	current = sil_rew_mem((void*)CMTW1_CMWCNT_ADDR);
	compare = sil_rew_mem((void*)CMTW1_CMWCOR_ADDR);

	/*
	 * タイマ停止
	 */
	*CMTW1_CMWSTR_ADDR &= ~CMTWn_CMWSTR_STR_BIT;

	/*
	 * タイマカウンタ設定
	 */
	*CMTW1_CMWCNT_ADDR = 0U;

	/*
	 * タイマ割込み要求をクリアする．
	 */
	clear_int(INTNO_TWDTIMER);

	if (current >= compare) {
		return 0;
	}

	return CMWCNT_TO_PRCTIM(compare - current);
}

/*
 *  タイムウィンドウタイマの現在値の読出し
 */
Inline PRCTIM
target_twdtimer_get_current(void)
{
	uint32_t current, compare;

	/*
	 *  現在のカウント値を読み
	 */
	current = sil_rew_mem((void*)CMTW1_CMWCNT_ADDR);
	compare = sil_rew_mem((void*)CMTW1_CMWCOR_ADDR);

	if (current >= compare) {
		return 0;
	}

	return CMWCNT_TO_PRCTIM(compare - current);
}

/*
 *  タイムウィンドウ割込みハンドラ
 */
extern void target_twdtimer_handler(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_PRC_TIMER_H */
