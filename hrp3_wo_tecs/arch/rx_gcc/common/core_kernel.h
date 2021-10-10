/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2021 by Embedded and Real-Time Systems Laboratory
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
 *		kernel.hのコア依存部（RX用）
 *
 *  このヘッダファイルは，target_kernel.h（または，そこからインクルード
 *  されるファイル）のみからインクルードされる．他のファイルから直接イ
 *  ンクルードしてはならない．
 */

#ifndef TOPPERS_CORE_KERNEL_H
#define TOPPERS_CORE_KERNEL_H

/*
 *  CPU例外ハンドラ番号の定義
 */
#define EXCNO_SCYCOVR	2	/* システム周期オーバラン例外 */
#define EXCNO_PRIV_INST	20	/* 特権命令例外 */
#define EXCNO_ACCESS	21	/* アクセス例外 */
#define EXCNO_UNDEF		23	/* 未定義命令例外 */
#define EXCNO_FLOAT		25	/* 浮動小数点例外 */
#define EXCNO_NMI		30	/* ノンマスカブル割込み */

/*
 *  開発環境（GNU開発環境）依存のメモリオブジェクト属性
 */
#define TA_KEEP			UINT_C(0x1000)		/* セクションにKEEP指定を付ける */

/*
 *  CPU例外ハンドラ番号の属性
 */
#define TA_DIRECT		1

/*
 *  サポートする機能の定義
 */
#define TOPPERS_TARGET_SUPPORT_DIS_INT		/* dis_int */
#define TOPPERS_TARGET_SUPPORT_ENA_INT		/* ena_int */
#define TOPPERS_TARGET_SUPPORT_CLR_INT		/* clr_int */
#define TOPPERS_TARGET_SUPPORT_RAS_INT		/* ras_int */
#define TOPPERS_TARGET_SUPPORT_PRB_INT		/* prb_int */
#define TOPPERS_TARGET_SUPPORT_ML_AUTO		/* 自動メモリ配置 */
#define TOPPERS_TARGET_SUPPORT_ML_MANUAL	/* 手動メモリ配置 */
#define TOPPERS_TARGET_SUPPORT_ATT_MOD		/* ATT_MOD */

/*
 *  カーネル管理の割込み優先度の範囲
 *
 *  TMIN_INTPRIの定義を変更することで，どのレベルよりも高い割込み優先度
 *  を持つものをカーネル管理外の割込みとするかを変更できる．
 *
 *  TMIN_INTPRIに設定できる値は，-15～-1の範囲である．例えばTMIN_INTPRI
 *  を-14に設定すると，NMIに加えてレベル15の割込みがカーネル管理外となる．
 *  TMIN_INTPRIを-15に設定すると，NMI以外にカーネル管理外の割込みを
 *  設けないことになる．
 */
#ifndef TMIN_INTPRI
#define TMIN_INTPRI		( -15 )		/* 割込み優先度の最小値（最高値）*/
#endif /* TMIN_INTPRI */
#define TMAX_INTPRI		( -1 )		/* 割込み優先度の最大値（最低値） */

#endif /* TOPPERS_CORE_KERNEL_H */
