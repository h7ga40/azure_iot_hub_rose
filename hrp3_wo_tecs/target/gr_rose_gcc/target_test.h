/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2007-2016 by Embedded and Real-Time Systems Laboratory
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
 *		テストプログラムのターゲット依存部（Renesas Starter Kit+ for RX65N-2MB用）
 */

#ifndef TOPPERS_TARGET_TEST_H
#define TOPPERS_TARGET_TEST_H

#include "rx65n.h"

/*
 *  サンプルプログラム／テストプログラムで使用する割込みに関する定義
 */
#define INTNO1				INT_SWINT2
#define INTNO1_INTATR		TA_ENAINT
#define INTNO1_INTPRI		-15
#define intno1_clear()		// エッジなので不要

#if defined(TOPPERS_ML_MANUAL)
#define TASK1_STK		(0x1000U)
#define TASK1_STKSZ		(TASK2_STK - TASK1_STK)
#define TASK2_STK		(0x1400U)
#define TASK2_STKSZ		(ALM_TASK_STK - TASK2_STK)
#define ALM_TASK_STK	(0x1800U)
#define ALM_TASK_STKSZ	(TASK3_STK - ALM_TASK_STK)
#define TASK3_STK		(0x1c00U)
#define TASK3_STKSZ		(0x0400U)
#endif

/*
 *  コアで共通な定義（チップ依存部は飛ばす）
 */
#include "core_test.h"

#endif /* TOPPERS_TARGET_TEST_H */
