/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2015-2019 by Embedded and Real-Time Systems Laboratory
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
 *		タイムウィンドウ通知に関するテスト(2)
 *
 * 【テストの目的】
 *
 *  タイムウィンドウ通知に関する不具合に関してテストする．
 *
 * 【テスト項目】
 *
 *	以下の不具合が，修正前のバージョンでは発生し，修正後のバージョンで
 *	は治っていることをテストする．
 *
 *	(1) タスクから呼ばれたサービスコールで，システム周期／タイムウィン
 *		ドウの実行開始処理を行う場合に，その処理途中で優先度の高いタス
 *		クが実行可能になると切り換わってしまう不具合
 *	(2) 同じ状況で，タイムウィンドウ通知やタイムイベント通知の実行時に，
 *		保護ドメインが正しく設定されない不具合
 *
 *	(A) ena_dspでテスト
 *	  - タスク1がタスク切換え禁止状態で，タイムウィンドウ切換え時刻を
 *		越えて実行した後，タスク切換えを許可する（ena_dsp）
 *	  - タスク切換えの許可のサービスコール（ena_dsp）で，タイムウィン
 *		ドウの実行開始処理が行われ，タイムウィンドウ通知とユーザドメイ
 *		ンのタイムイベント処理が実行される
 *	  - タイムウィンドウ通知でタスク1より優先度が高く，カーネルドメイ
 *		ンに属するタスク2を実行可能にする．不具合があると，この時点で
 *		タスク2に切り換わってしまう
 *	  - その後に実行されるタイムイベント処理で，タスク2より優先度が高
 *		くカーネルドメインに属するタスク3を実行可能にする
 *	  - 不具合がなければ，タイムウィンドウの実行開始処理の後には，タス
 *		ク3が実行され，タスク2の実行はタスク3の後になる
 *
 *	(B) chg_somでテスト
 *	  - カーネルドメインに属するタスク4が，システム周期停止モードから
 *		に他のモードに切り換える（chg_som）
 *	  - システム周期モードの変更のサービスコール（chg_som）で，システ
 *		ム周期とタイムウィンドウの実行開始処理が行われ，タイムウィンド
 *		ウ通知とユーザドメインのタイムイベント処理が実行される
 *	  - タイムウィンドウ通知でタスク4より優先度が高く，カーネルドメイ
 *		ンに属するタスク5を実行可能にする．不具合があると，この時点で
 *		タスク5に切り換わってしまう
 *	  - その後に実行されるタイムイベント処理で，タスク5より優先度が高
 *		くカーネルドメインに属するタスク6を実行可能にする
 *	  - 不具合がなければ，タイムウィンドウの実行開始処理の後には，タス
 *		ク6が実行され，タスク5の実行はタスク6の後になる
 *
 * 【使用リソース】
 *
 *	システム周期: (20 * TEST_TIME_CP)
 *	SOM1: 2つのタイムウィンドウ，初期システム動作モード
 *		最初のタイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM1に割当て
 *		2番目のタイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM2に割当て
 *			タイムウィンドウ通知で，TASK2を起動
 *	SOM2: 1つのタイムウィンドウ
 *		最初のタイムウィンドウ（長さ: 5 * TEST_TIME_CP）をDOM2に割当て
 *			タイムウィンドウ通知で，TASK2を起動
 *	TASK1: DOM1，低優先度
 *	TASK2: カーネルドメイン，中優先度
 *	TASK3: カーネルドメイン，高優先度
 *	TASK4: DOM1，低優先度
 *	TASK5: カーネルドメイン，中優先度
 *	TASK6: カーネルドメイン，高優先度
 *	CYC1: DOM2，TASK3を起動，TA_STA属性
 *		  周期：(20 * TEST_TIME_CP)
 *		  初期起動：(5 * TEST_TIME_CP)
 *	ALM1: DOM2，TASK6を起動
 *
 * 【テストシーケンス】
 *
 *	// 1回目のシステム周期（システム動作モード：SOM1）
 *	// タイムウィンドウ for DOM1
 *	== TASK1 ==
 *	1:	dis_dsp()
 *	2:	DO(while(!get_pending_twdswitch()))
 *	// タイムウィンドウ切換え割込みが発生
 *	3:	ena_dsp()
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK2を起動）
 *	// CYC1が実行される（TASK3を起動）
 *	== TASK3 ==
 *	4:	ext_tsk()
 *	== TASK2 ==
 *	5:	chg_som(TSOM_STP)
 *	6:	act_tsk(TASK4)
 *		ext_tsk()
 *	== TASK4 ==
 *	7:	stp_cyc(CYC1)
 *		sta_alm(ALM1, 20 * TEST_TIME_CP)
 *		dly_tsk(20 * TEST_TIME_CP)
 *	// システム周期停止モードに入る
 *	8:	get_som(&somid)
 *		assert(somid == TSOM_STP)
 *	9:	chg_som(SOM2)
 *	// タイムウィンドウ for DOM2
 *	// タイムウィンドウ通知が実行される（TASK5を起動）
 *	// ALM1が実行される（TASK6を起動）
 *	== TASK6 ==
 *	10:	ext_tsk()
 *	== TASK5 ==
 *	11:	END
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

/*
 *  タイムウィンドウ切換えが保留されているかのチェック
 */
ER_UINT extsvc1_routine(intptr_t nsec, intptr_t par2, intptr_t par3,
									intptr_t par4, intptr_t par5, ID cdmid)
{
	extern bool_t	_kernel_pending_twdswitch;

	return((ER_UINT) _kernel_pending_twdswitch);
}

static bool_t
get_pending_twdswitch(void)
{
	return((bool_t) cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0));
}

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
task1(EXINF exinf)
{
	ER_UINT	ercd;

	test_start(__FILE__);

	check_point(1);
	ercd = dis_dsp();
	check_ercd(ercd, E_OK);

	check_point(2);
	while(!get_pending_twdswitch());

	check_point(3);
	ercd = ena_dsp();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(5);
	ercd = chg_som(TSOM_STP);
	check_ercd(ercd, E_OK);

	check_point(6);
	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(4);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;
	ID		somid;

	check_point(7);
	ercd = stp_cyc(CYC1);
	check_ercd(ercd, E_OK);

	ercd = sta_alm(ALM1, 20 * TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	ercd = dly_tsk(20 * TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	check_point(8);
	ercd = get_som(&somid);
	check_ercd(ercd, E_OK);

	check_assert(somid == TSOM_STP);

	check_point(9);
	ercd = chg_som(SOM2);
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task5(EXINF exinf)
{

	check_finish(11);
	check_assert(false);
}

void
task6(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(10);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}
