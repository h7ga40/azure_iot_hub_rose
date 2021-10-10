/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2019 by Embedded and Real-Time Systems Laboratory
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
 *		システム状態管理機能のテスト(2)
 *
 * 【テストの目的】
 *
 *  mrot_rdqを網羅的にテストする．
 *
 * 【テスト項目】
 *
 *	(A) mrot_rdqの静的エラーのテスト（タスクコンテキスト）
 *		(A-1) CPUロック状態からの呼出し［NGKI2695］
 *		(A-2) schednoが有効範囲外［NGKI2696］（小さすぎる）
 *		(A-3) schednoが有効範囲外［NGKI2696］（大きすぎる）
 *		(A-4) tskpriがが有効範囲外［NGKI2697］（小さすぎる）
 *		(A-5) tskpriがが有効範囲外［NGKI2697］（大きすぎる）
 *		(A-6) 操作が許可されていない［NGKI3778］
 *	(B) mrot_rdqの静的エラーのテスト（非タスクコンテキスト）
 *		(B-1) CPUロック状態からの呼出し［NGKI2695］
 *		(B-2) schednoが有効範囲外［NGKI2696］（小さすぎる）
 *		(B-3) schednoが有効範囲外［NGKI2696］（大きすぎる）
 *		(B-4) tskpriが有効範囲外［NGKI2697］（小さすぎる）
 *		(B-5) tskpriが有効範囲外［NGKI2697］（大きすぎる）
 *		(B-6) 操作が許可されていない［NGKI3778］エラーにならない
 *	(C) 対象スケジューリング単位が，カーネルドメイン
 *		(C-1) タスク数が0で，何も行わない［NGKI2700］
 *		(C-2) タスク数が1で，何も行わない［NGKI2700］
 *		(C-3) タスク数が2で，優先順位が最も高いタスクを，同じ優先度のタ
 *		      スクの中で優先順位を最も低くする［NGKI2699］
 *		(C-4) タスク数が3で，優先順位が最も高いタスクを，同じ優先度のタ
 *		      スクの中で優先順位を最も低くする［NGKI2699］
 *		(C-5) 優先順位を変更した結果，タスク切換えが起こる
 *	(D) 対象スケジューリング単位が，タイムウィンドウを割り当てられたユー
 *		ザドメイン
 *		(D-1) タスク数が0で，何も行わない［NGKI2700］
 *		(D-2) タスク数が1で，何も行わない［NGKI2700］
 *		(D-3) タスク数が2で，優先順位が最も高いタスクを，同じ優先度のタ
 *		      スクの中で優先順位を最も低くする［NGKI2699］
 *		(D-4) タスク数が3で，優先順位が最も高いタスクを，同じ優先度のタ
 *		      スクの中で優先順位を最も低くする［NGKI2699］
 *		(D-5) 優先順位を変更した結果，タスク切換えが起こる
 *	(E) 対象スケジューリング単位が，アイドルドメインにまとめられたユー
 *		ザドメイン［NGKI0631］
 *		(E-1) タスク数が0で，何も行わない［NGKI2700］
 *		(E-2) タスク数が1で，何も行わない［NGKI2700］
 *		(E-3) タスク数が2で，優先順位が最も高いタスクを，同じ優先度のタ
 *		      スクの中で優先順位を最も低くする［NGKI2699］
 *		(E-4) タスク数が3で，優先順位が最も高いタスクを，同じ優先度のタ
 *		      スクの中で優先順位を最も低くする［NGKI2699］
 *		(E-5) 優先順位を変更した結果，タスク切換えが起こる
 *	(F) スケジューリング単位番号にTDOM_SELF（＝0）を指定［NGKI0630］
 *		(F-1) 非タスクコンテキストから呼び出した時
 *		(F-2) カーネルドメインのタスクコンテキストから呼び出した時
 *		(F-3) ユーザドメインのタスクコンテキストから呼び出した時
 *		(F-4) 非タスクコンテキストから呼ばれた拡張サービスコールから呼
 *		      び出した時
 *		(F-5) カーネルドメインのタスクコンテキストから呼ばれた拡張サー
 *		      ビスコールから呼び出した時
 *		(F-6) ユーザドメインのタスクコンテキストから呼ばれた拡張サービ
 *		      スコールから呼び出した時
 *	(G) tskpriにTPRI_SELF（＝0）を指定［NGKI2701］
 *		(G-1) 非タスクコンテキストから呼び出した時
 *		(G-2) タスクコンテキストから呼び出した時
 *		(G-3) 非タスクコンテキストから呼ばれた拡張サービスコールから呼
 *		      び出した時
 *		(G-4) タスクコンテキストから呼ばれた拡張サービスコールから呼び
 *		      出した時
 *
 * 【使用リソース】
 *
 *	システム周期: TEST_TIME_CP×20
 *	DOM1: タイムウィンドウを割り当てられたユーザドメイン
 *			TEST_TIME_CP×10のタイムウィンドウを割り当てる
 *	DOM2: アイドルドメインにまとめられたユーザドメイン
 *	DOM3: アイドルドメインにまとめられたユーザドメイン
 *	TASK1: DOM1，高優先度，最初から起動
 *	TASK2: DOM1，中優先度
 *	TASK3: DOM1，中優先度
 *	TASK4: DOM1，中優先度
 *	TASK5: DOM2，高優先度，最初から起動
 *	TASK6: DOM2，中優先度
 *	TASK7: DOM2，中優先度
 *	TASK8: DOM3，中優先度
 *	TASK9: カーネルドメイン，中優先度
 *	TASK10: カーネルドメイン，中優先度
 *	TASK11: カーネルドメイン，中優先度
 *	ALM1: カーネルドメイン
 *
 * 【テストシーケンス】
 *
 *	== TASK1（優先度：高，DOM1）==
 *	1:	mrot_rdq(DOM1, MID_PRIORITY)					... (D-1)
 *	2:	act_tsk(TASK3)
 *	//	高：TASK1，中：TASK3，高：TASK5
 *		mrot_rdq(DOM1, MID_PRIORITY)					... (D-2)
 *	//	高：TASK1，中：TASK3，高：TASK5
 *		mget_nth(DOM1, MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK3)
 *	3:	act_tsk(TASK2)
 *	//	高：TASK1，中：TASK3→TASK2，高：TASK5
 *		mrot_rdq(TDOM_SELF, MID_PRIORITY)				... (D-3)(F-3)
 *	//	高：TASK1，中：TASK2→TASK3，高：TASK5
 *		mget_nth(DOM1, MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK2)
 *	4:	loc_cpu()
 *		mrot_rdq(DOM1, MID_PRIORITY) -> E_CTX			... (A-1)
 *		unl_cpu()
 *		mrot_rdq(DOMID_SMALL, MID_PRIORITY) -> E_ID		... (A-2)
 *		mrot_rdq(DOMID_LARGE, MID_PRIORITY) -> E_ID		... (A-3)
 *		mrot_rdq(DOM1, TPRI_SMALL_TASK) -> E_PAR		... (A-4)
 *		mrot_rdq(DOM1, TPRI_LARGE) -> E_PAR				... (A-5)
 *		mrot_rdq(DOM2, MID_PRIORITY) -> E_OACV			... (A-6)
 *		act_tsk(TASK4)
 *	//	高：TASK1，中：TASK2→TASK3→TASK4，高：TASK5
 *		slp_tsk()
 *	== TASK2（優先度：中，DOM1）==
 *	5:	mrot_rdq(DOM1, MID_PRIORITY)					... (D-4)(D-5)
 *	//	中：TASK3→TASK4→TASK2，高：TASK5
 *	== TASK3（優先度：中，DOM1）==
 *	6:	mrot_rdq(DOM1, TPRI_SELF)						... (G-2)
 *	//	中：TASK4→TASK2→TASK3，高：TASK5
 *	== TASK4（優先度：中，DOM1）==
 *	7:	sta_alm(ALM1, TEST_TIME_CP)
 *		WAIT(task4_flag)
 *	== ALM1 ==
 *	8:	loc_cpu()
 *		mrot_rdq(DOM1, MID_PRIORITY) -> E_CTX			... (B-1)
 *		unl_cpu()
 *		mrot_rdq(DOMID_SMALL, MID_PRIORITY) -> E_ID		... (B-2)
 *		mrot_rdq(DOMID_LARGE, MID_PRIORITY) -> E_ID		... (B-3)
 *		mrot_rdq(DOM1, TPRI_SMALL_INT) -> E_PAR			... (B-4)
 *		mrot_rdq(DOM1, TPRI_LARGE) -> E_PAR				... (B-5)
 *		mrot_rdq(DOM2, MID_PRIORITY)					... (B-6)
 *		mrot_rdq(DOM1, TPRI_SELF) -> E_PAR				... (G-1)
 *		SET(task4_flag)
 *		mrot_rdq(TDOM_KERNEL, MID_PRIORITY)				... (C-1)
 *		act_tsk(TASK9)
 *	//	中：TASK9⇒TASK4→TASK2→TASK3，高：TASK5
 *		mrot_rdq(TDOM_KERNEL, MID_PRIORITY)				... (C-2)
 *	//	中：TASK9⇒TASK4→TASK2→TASK3，高：TASK5
 *		act_tsk(TASK10)
 *	//	中：TASK9→TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *		mrot_rdq(TDOM_KERNEL, MID_PRIORITY)				... (C-3)
 *	//	中：TASK10→TASK9⇒TASK4→TASK2→TASK3，高：TASK5
 *		wup_tsk(TASK1)
 *	//	高：TASK1，中：TASK10→TASK9⇒TASK4→TASK2→TASK3，高：TASK5
 *		RETURN
 *	== TASK1（続き）==
 *	9:	cal_svc(TFN_EXTSVC3, 0, 0, 0, 0, 0) 
 *	== EXTSVC3 ==
 *	10:	mrot_rdq(TDOM_SELF, MID_PRIORITY)				... (F-6)
 *	//	高：TASK1，中：TASK9→TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *		RETURN(E_OK)
 *	== TASK1（続き）==
 *	11:	ext_tsk()
 *	//	中：TASK9→TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *	== TASK9（優先度：中，カーネルドメイン）==
 *	12:	act_tsk(TASK11)
 *	//	中：TASK9→TASK10→TASK11⇒TASK4→TASK2→TASK3，高：TASK5
 *		mrot_rdq(TDOM_KERNEL, MID_PRIORITY)				... (C-4)(C-5)
 *	//	中：TASK10→TASK11→TASK9⇒TASK4→TASK2→TASK3，高：TASK5
 *	== TASK10（優先度：中，カーネルドメイン）==
 *	13:	mrot_rdq(TDOM_SELF, MID_PRIORITY)				... (F-2)
 *	//	中：TASK11→TASK9→TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *	== TASK11（優先度：中，カーネルドメイン）==
 *	14:	cal_svc(TFN_EXTSVC2, 0, 0, 0, 0, 0) 
 *	== EXTSVC2 ==
 *	15:	mrot_rdq(TDOM_SELF, TPRI_SELF)					... (F-5)(G-4)
 *	//	中：TASK9→TASK10→TASK11⇒TASK4→TASK2→TASK3，高：TASK5
 *	== TASK9（続き）==
 *	16:	sta_alm(ALM2, TEST_TIME_CP)
 *		WAIT(task9_flag)
 *	== ALM2 ==
 *	17:	mrot_rdq(TDOM_SELF, MID_PRIORITY)				... (F-1)
 *	//	中：TASK10→TASK11→TASK9⇒TASK4→TASK2→TASK3，高：TASK5
 *		cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0) 
 *	== EXTSVC1 ==
 *	18:	mrot_rdq(TDOM_KERNEL, TPRI_SELF) -> E_PAR		... (G-3)
 *		mrot_rdq(TDOM_SELF, MID_PRIORITY)				... (F-4)
 *	//	中：TASK11→TASK9→TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *		RETURN(E_OK)
 *	== ALM2（続き）==
 *	19:	SET(task9_flag)
 *		RETURN
 *	== EXTSVC2（続き）==
 *	20:	RETURN(E_OK)
 *	== TASK11（続き）==
 *	21:	ext_tsk()
 *	//	中：TASK9→TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *	== TASK9（続き）==
 *	22:	ext_tsk()
 *	//	中：TASK10⇒TASK4→TASK2→TASK3，高：TASK5
 *	== TASK10（続き）==
 *	23:	ext_tsk()
 *	//	中：TASK4→TASK2→TASK3，高：TASK5
 *	== TASK4（続き）==
 *	24:	ext_tsk()
 *	//	中：TASK2→TASK3，高：TASK5
 *	== TASK2（続き）==
 *	25:	ext_tsk()
 *	//	中：TASK3，高：TASK5
 *	== TASK3（続き）==
 *	26:	ext_tsk()
 *	//	高：TASK5
 *	== TASK5（優先度：高，DOM2）==
 *	27:	mrot_rdq(DOM2, MID_PRIORITY)					... (E-1)
 *	28:	act_tsk(TASK7)
 *	//	高：TASK5，中：TASK7
 *		mrot_rdq(DOM2, MID_PRIORITY)					... (E-2)
 *	//	高：TASK5，中：TASK7
 *		mget_nth(DOM2, MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK7)
 *	29:	act_tsk(TASK6)
 *	//	高：TASK5，中：TASK7→TASK6
 *		mrot_rdq(TDOM_SELF, MID_PRIORITY)				... (E-3)(F-3)
 *	//	高：TASK5，中：TASK6→TASK7
 *		mget_nth(DOM2, MID_PRIORITY, 0U, &tskid)
 *		assert(tskid == TASK6)
 *		act_tsk(TASK8)
 *	//	高：TASK5，中：TASK6→TASK7→TASK8
 *		ext_tsk()
 *	//	中：TASK6→TASK7→TASK8
 *	== TASK6（優先度：中，DOM2）==
 *	30:	mrot_rdq(DOM2, MID_PRIORITY)					... (E-4)(E-5)
 *	//	中：TASK7→TASK8→TASK6
 *	== TASK7（優先度：中，DOM2）==
 *	31:	ext_tsk()
 *	//	中：TASK8→TASK6
 *	== TASK8（優先度：中，DOM1）==
 *	32:	ext_tsk()
 *	//	中：TASK6
 *	== TASK6（続き）==
 *	33:	END
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/test_svc.h"
#include "kernel_cfg.h"
#include "test_common.h"

volatile bool_t		task4_flag;
volatile bool_t		task9_flag;

/* DO NOT DELETE THIS LINE -- gentest depends on it. */

void
alarm1_handler(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(8);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOM1, MID_PRIORITY);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOMID_SMALL, MID_PRIORITY);
	check_ercd(ercd, E_ID);

	ercd = mrot_rdq(DOMID_LARGE, MID_PRIORITY);
	check_ercd(ercd, E_ID);

	ercd = mrot_rdq(DOM1, TPRI_SMALL_INT);
	check_ercd(ercd, E_PAR);

	ercd = mrot_rdq(DOM1, TPRI_LARGE);
	check_ercd(ercd, E_PAR);

	ercd = mrot_rdq(DOM2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOM1, TPRI_SELF);
	check_ercd(ercd, E_PAR);

	SET(task4_flag);

	ercd = mrot_rdq(TDOM_KERNEL, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK9);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(TDOM_KERNEL, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK10);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(TDOM_KERNEL, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);

	return;

	check_assert(false);
}

void
alarm2_handler(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(17);
	ercd = mrot_rdq(TDOM_SELF, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0);
	check_ercd(ercd, E_OK);

	check_point(19);
	SET(task9_flag);

	return;

	check_assert(false);
}

ER_UINT
extsvc1_routine(intptr_t par1, intptr_t par2, intptr_t par3,
								intptr_t par4, intptr_t par5, ID cdmid)
{
	ER_UINT	ercd;

	check_point(18);
	ercd = mrot_rdq(TDOM_KERNEL, TPRI_SELF);
	check_ercd(ercd, E_PAR);

	ercd = mrot_rdq(TDOM_SELF, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	return(E_OK);

	check_assert(false);
	return(E_SYS);
}

ER_UINT
extsvc2_routine(intptr_t par1, intptr_t par2, intptr_t par3,
								intptr_t par4, intptr_t par5, ID cdmid)
{
	ER_UINT	ercd;

	check_point(15);
	ercd = mrot_rdq(TDOM_SELF, TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(20);
	return(E_OK);

	check_assert(false);
	return(E_SYS);
}

ER_UINT
extsvc3_routine(intptr_t par1, intptr_t par2, intptr_t par3,
								intptr_t par4, intptr_t par5, ID cdmid)
{
	ER_UINT	ercd;

	check_point(10);
	ercd = mrot_rdq(TDOM_SELF, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	return(E_OK);

	check_assert(false);
	return(E_SYS);
}

void
task1(EXINF exinf)
{
	ER_UINT	ercd;
	ID		tskid;

	test_start(__FILE__);

	check_point(1);
	ercd = mrot_rdq(DOM1, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(2);
	ercd = act_tsk(TASK3);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOM1, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = mget_nth(DOM1, MID_PRIORITY, 0U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK3);

	check_point(3);
	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(TDOM_SELF, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = mget_nth(DOM1, MID_PRIORITY, 0U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK2);

	check_point(4);
	ercd = loc_cpu();
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOM1, MID_PRIORITY);
	check_ercd(ercd, E_CTX);

	ercd = unl_cpu();
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOMID_SMALL, MID_PRIORITY);
	check_ercd(ercd, E_ID);

	ercd = mrot_rdq(DOMID_LARGE, MID_PRIORITY);
	check_ercd(ercd, E_ID);

	ercd = mrot_rdq(DOM1, TPRI_SMALL_TASK);
	check_ercd(ercd, E_PAR);

	ercd = mrot_rdq(DOM1, TPRI_LARGE);
	check_ercd(ercd, E_PAR);

	ercd = mrot_rdq(DOM2, MID_PRIORITY);
	check_ercd(ercd, E_OACV);

	ercd = act_tsk(TASK4);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	check_point(9);
	ercd = cal_svc(TFN_EXTSVC3, 0, 0, 0, 0, 0);
	check_ercd(ercd, E_OK);

	check_point(11);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task10(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(13);
	ercd = mrot_rdq(TDOM_SELF, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(23);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task11(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(14);
	ercd = cal_svc(TFN_EXTSVC2, 0, 0, 0, 0, 0);
	check_ercd(ercd, E_OK);

	check_point(21);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task2(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(5);
	ercd = mrot_rdq(DOM1, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(25);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task3(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(6);
	ercd = mrot_rdq(DOM1, TPRI_SELF);
	check_ercd(ercd, E_OK);

	check_point(26);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task4(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(7);
	ercd = sta_alm(ALM1, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	WAIT(task4_flag);

	check_point(24);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task5(EXINF exinf)
{
	ER_UINT	ercd;
	ID		tskid;

	check_point(27);
	ercd = mrot_rdq(DOM2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(28);
	ercd = act_tsk(TASK7);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(DOM2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = mget_nth(DOM2, MID_PRIORITY, 0U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK7);

	check_point(29);
	ercd = act_tsk(TASK6);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(TDOM_SELF, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	ercd = mget_nth(DOM2, MID_PRIORITY, 0U, &tskid);
	check_ercd(ercd, E_OK);

	check_assert(tskid == TASK6);

	ercd = act_tsk(TASK8);
	check_ercd(ercd, E_OK);

	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task6(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(30);
	ercd = mrot_rdq(DOM2, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_finish(33);
	check_assert(false);
}

void
task7(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(31);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task8(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(32);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}

void
task9(EXINF exinf)
{
	ER_UINT	ercd;

	check_point(12);
	ercd = act_tsk(TASK11);
	check_ercd(ercd, E_OK);

	ercd = mrot_rdq(TDOM_KERNEL, MID_PRIORITY);
	check_ercd(ercd, E_OK);

	check_point(16);
	ercd = sta_alm(ALM2, TEST_TIME_CP);
	check_ercd(ercd, E_OK);

	WAIT(task9_flag);

	check_point(22);
	ercd = ext_tsk();
	check_ercd(ercd, E_OK);

	check_assert(false);
}
