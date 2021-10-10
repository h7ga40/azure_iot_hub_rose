/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2006-2020 by Embedded and Real-Time Systems Laboratory
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
 *		拡張サービスコール呼出し時間の評価(1)
 *
 *  拡張サービスコールの呼出しにかかる時間を計測するためのプログラム．
 */

#include <kernel.h>
#include <t_syslog.h>
#include "syssvc/syslog.h"
#include "syssvc/test_svc.h"
#include "syssvc/histogram.h"
#include "kernel_cfg.h"
#include "test_common.h"

/*
 *  実行時間分布集計サービスを直接呼び出すためのヘッダファイル
 */
#include "tHistogram_tecsgen.h"

/*
 *  待ち合わせ用のフラグ変数
 */
volatile bool_t task1_flag;

/*
 *  拡張サービスコールルーチン
 */
ER_UINT
extsvc1_routine(intptr_t par1, intptr_t par2, intptr_t par3,
								intptr_t par4, intptr_t par5, ID cdmid)
{
	return(E_OK);
}

/*
 *  アラームハンドラ1
 */
void
alarm1_handler(EXINF exinf)
{
	ER		ercd;

	ercd = begin_measure(1);
	check_ercd(ercd, E_OK);

	ercd = cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0);
	check_ercd(ercd, E_OK);

	ercd = end_measure(1);
	check_ercd(ercd, E_OK);

	SET(task1_flag);
}

/*
 *  アラームハンドラ2
 */
void
alarm2_handler(EXINF exinf)
{
	ER		ercd;

	ercd = eHistogram_beginMeasure(&tHistogram_CB_tab[0]);
	check_ercd(ercd, E_OK);

	ercd = cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0);
	check_ercd(ercd, E_OK);

	ercd = eHistogram_endMeasure(&tHistogram_CB_tab[0]);
	check_ercd(ercd, E_OK);

	SET(task1_flag);
}

/*
 *  タスク1（カーネルドメイン）
 */
void
task1(EXINF exinf)
{
	uint_t	i;
	ER		ercd;

	test_start(__FILE__);

	/*
	 *  (1-3-1) 非タスクコンテキスト（アラームハンドラを使用）から，拡
	 *  張サービスコールを呼び出した場合の評価
	 */
	ercd = init_hist(1);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_MEASURE; i++) {
		RESET(task1_flag);

		ercd = sta_alm(ALM1, 1U);
		check_ercd(ercd, E_OK);

		WAIT_WO_RESET(task1_flag);
	}

	syslog_0(LOG_NOTICE, "Extended SVC calling time from non-task context");
	ercd = print_hist(1);
	check_ercd(ercd, E_OK);

	/*
	 *  (1-3-2) カーネルドメインに属するタスクから，拡張サービスコール
	 *	を呼び出した場合の評価
	 */
	ercd = init_hist(1);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_MEASURE; i++) {
		ercd = begin_measure(1);
		check_ercd(ercd, E_OK);

		ercd = cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0);
		check_ercd(ercd, E_OK);

		ercd = end_measure(1);
		check_ercd(ercd, E_OK);
	}

	syslog_0(LOG_NOTICE, "Extended SVC calling time from kernel domain task " \
												"through software interrupt");
	ercd = print_hist(1);
	check_ercd(ercd, E_OK);

	ercd = act_tsk(TASK2);
	check_ercd(ercd, E_OK);

	ercd = slp_tsk();
	check_ercd(ercd, E_OK);

	/*
	 *  (1-3-4) 非タスクコンテキスト（アラームハンドラを使用）から，拡
	 *  張サービスコールを呼び出した場合の評価（時間分布集計サービスを，
	 *  関数呼出しで呼び出す）
	 */
	ercd = eHistogram_initialize(&tHistogram_CB_tab[0]);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_MEASURE; i++) {
		RESET(task1_flag);

		ercd = sta_alm(ALM2, 1U);
		check_ercd(ercd, E_OK);

		WAIT_WO_RESET(task1_flag);
	}

	syslog_0(LOG_NOTICE, "Extended SVC calling time from non-task context " \
													"through function call");
	ercd = eHistogram_print(&tHistogram_CB_tab[0]);
	check_ercd(ercd, E_OK);

	/*
	 *  (1-3-5) カーネルドメインに属するタスクから，拡張サービスコール
	 *  を呼び出した場合の評価（時間分布集計サービスを，関数呼出しで呼
	 *  び出す）
	 */
	ercd = eHistogram_initialize(&tHistogram_CB_tab[0]);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_MEASURE; i++) {
		ercd = eHistogram_beginMeasure(&tHistogram_CB_tab[0]);
		check_ercd(ercd, E_OK);

		ercd = cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0);
		check_ercd(ercd, E_OK);

		ercd = eHistogram_endMeasure(&tHistogram_CB_tab[0]);
		check_ercd(ercd, E_OK);
	}

	syslog_0(LOG_NOTICE, "Extended SVC calling time from kernel domain task " \
												"through software interrupt");
	syslog_0(LOG_NOTICE, "(mesurement trough function call)");
	ercd = eHistogram_print(&tHistogram_CB_tab[0]);
	check_ercd(ercd, E_OK);

	check_finish(0);
}

/*
 *  タスク2（ユーザドメイン）
 */
void
task2(EXINF exinf)
{
	uint_t	i;
	ER		ercd;

	/*
	 *  (1-3-3) ユーザドメインに属するタスクから，拡張サービスコールを
	 *	呼び出した場合の評価
	 */
	ercd = init_hist(1);
	check_ercd(ercd, E_OK);

	for (i = 0; i < NO_MEASURE; i++) {
		ercd = begin_measure(1);
		check_ercd(ercd, E_OK);

		ercd = cal_svc(TFN_EXTSVC1, 0, 0, 0, 0, 0);
		check_ercd(ercd, E_OK);

		ercd = end_measure(1);
		check_ercd(ercd, E_OK);
	}

	syslog_0(LOG_NOTICE, "Extended SVC calling time from user domain task");
	ercd = print_hist(1);
	check_ercd(ercd, E_OK);

	ercd = wup_tsk(TASK1);
	check_ercd(ercd, E_OK);
	check_assert(false);
}
