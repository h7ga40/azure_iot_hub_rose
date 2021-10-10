/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2015-2020 by Embedded and Real-Time Systems Laboratory
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
 *  $Id$
 */

/* 
 *		タイムウィンドウ通知に関するテスト(1)
 *
 * 【テストの目的】
 *
 *  タイムウィンドウ通知の機能をテストする．
 *
 * 【テスト項目】
 *
 *	(A) タイムウィンドウ通知が期待したタイミングで実行されること
 *	(B) システム動作モードにより，実行されるタイムウィンドウ通知が切り
 *		換わること
 *	(C) タイムウィンドウ通知がタイムウィンドウを使い切った場合の処理
 *
 * 【使用リソース】
 *
 *	システム周期: (20 * TEST_TIME_CP)
 *	SOM1: 2つのタイムウィンドウ，初期システム動作モード
 *		最初タイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM1に割当て
 *			タイムウィンドウ通知で，TASK11を起動
 *		2番目のタイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM2に割当て
 *			タイムウィンドウ通知で，TASK2を起床
 *	SOM2: 3つのタイムウィンドウ
 *		最初タイムウィンドウ（長さ: 1，非常に短い）をDOM1に割当て
 *			タイムウィンドウ通知で，TASK13を起動
 *		2番目のタイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM2に割当て
 *			タイムウィンドウ通知で，TASK2を起床
 *		3番目のタイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM1に割当て
 *	TASK11: DOM1，高優先度
 *	TASK12: DOM1，中優先度
 *	TASK13: DOM1，高優先度
 *	TASK2: DOM2，高優先度，TA_ACT属性
 *	CYC11: DOM1，TASK12を起動，TA_STA属性，
 *		   周期：(40 * TEST_TIME_CP)
 *		   初期起動：(20 * TEST_TIME_CP)
 *
 * 【テストシーケンス】
 *
 *	// 1回目のシステム周期（システム動作モード：SOM1）
 *	// タイムウィンドウ for DOM1
 *	// タイムウィンドウ通知が実行される（TASK11を起動）
 *	== TASK11-1 ==
 *	1:	ext_tsk()
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK2を起床）
 *	== TASK2 ==
 *	2:	can_wup(TSK_SELF) -> 1U
 *	3:	slp_tsk()
 *	// アイドルウィンドウ
 *	// 2回目のシステム周期（システム動作モード：SOM1）
 *	// タイムウィンドウ for DOM1
 *	// タイムウィンドウ通知が実行される（TASK11を起動）
 *	// CYC11が実行される（TASK12を起動）
 *	== TASK11-2 ==
 *	4:	ext_tsk()
 *	== TASK12-1 ==
 *	5:	ext_tsk()
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK2を起床）
 *	== TASK2 ==
 *	6:	chg_som(SOM2)
 *		slp_tsk()
 *	// アイドルウィンドウ
 *	// 3回目のシステム周期（システム動作モード：SOM2）
 *	// タイムウィンドウ for DOM1
 *	// タイムウィンドウ通知が実行される（TASK13を起動）
 *	== TASK13-1 ==
 *		WAIT_RESET(task13_flag)				// TASK13が先に実行されることの対策
 *	// ここでタイムウィンドウを使い切る
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK2を起床）
 *	== TASK2 ==
 *	7:	ref_tsk(TASK13, &rtsk)
 *		assert(rtsk.tskstat == TTS_RDY)
 *		SET(task13_flag)
 *	8:	slp_tsk()
 *	// タイムウィンドウ for DOM1
 *	== TASK13-1 ==
 *	9:	ext_tsk()
 *	// アイドルウィンドウ
 *	// 4回目のシステム周期（システム動作モード：SOM2）
 *	// タイムウィンドウ for DOM1
 *	// タイムウィンドウ通知が実行される（TASK13を起動）
 *	// CYC11が実行される（TASK12を起動）
 *	== TASK13-2 ==
 *		WAIT_RESET(task13_flag)				// TASK13が先に実行されることの対策
 *	// ここでタイムウィンドウを使い切る
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK2を起床）
 *	== TASK2 ==
 *	10:	ref_tsk(TASK13, &rtsk)
 *		assert(rtsk.tskstat == TTS_RDY)
 *		SET(task13_flag)
 *	11:	slp_tsk()
 *	// タイムウィンドウ for DOM1
 *	== TASK13-2 ==
 *	12:	ext_tsk()
 *	== TASK12-2 ==
 *	13:	ext_tsk()
 *	// アイドルウィンドウ
 *	// タイムウィンドウ for DOM1
 *	// タイムウィンドウ通知が実行される（TASK13を起動）
 *	== TASK13-3 ==
 *		WAIT_RESET(task13_flag)				// TASK13が先に実行されることの対策
 *	// ここでタイムウィンドウを使い切る
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK2を起床）
 *	== TASK2 ==
 *	14:	END
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

volatile bool_t	task13_flag = false;

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

static uint_t	task11_count = 0;

void
task11(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task11_count) {
	case 1:
		test_start(__FILE__);

		check_point(1);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(4);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task12_count = 0;

void
task12(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task12_count) {
	case 1:
		check_point(5);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		check_point(13);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

static uint_t	task13_count = 0;

void
task13(EXINF exinf)
{
	ER_UINT	ercd;

	switch (++task13_count) {
	case 1:
		WAIT_RESET(task13_flag);

		check_point(9);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 2:
		WAIT_RESET(task13_flag);

		check_point(12);
		ercd = ext_tsk();
		check_ercd(ercd, E_OK);

		check_assert(false);

	case 3:
		WAIT_RESET(task13_flag);

		check_assert(false);

	default:
		check_assert(false);
	}
	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;
	T_RTSK	rtsk;

	check_point(2);
	ercd = can_wup(TSK_SELF);
	check_ercd(ercd, 1U);

	check_point(3);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = chg_som(SOM2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(7);
	ercd = ref_tsk(TASK13, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RDY);

	SET(task13_flag);

	check_point(8);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(10);
	ercd = ref_tsk(TASK13, &rtsk);
	check_ercd(ercd, E_OK);

	check_assert(rtsk.tskstat == TTS_RDY);

	SET(task13_flag);

	check_point(11);
	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_finish(14);
	check_assert(false);
}
