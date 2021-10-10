/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2011 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2013      by Mitsuhiro Matsuura
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
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
 *		コンパイラ依存定義
 */

#ifndef KERNEL_INLINE_SYMBOL_H
#define KERNEL_INLINE_SYMBOL_H

/*
 *	共通インクルードのInline関数のシンボル登録
 */

/* include/ */

/* queue.h	*/
#pragma inline queue_initialize
#pragma inline queue_insert_prev
#pragma inline queue_insert_next
#pragma inline queue_delete
#pragma inline queue_delete_next
#pragma inline queue_empty

/* sil.h	*/
#pragma inline sil_reb_mem
#pragma inline sil_wrb_mem
#pragma inline sil_reh_mem
#pragma inline sil_wrh_mem
#pragma inline sil_reh_lem
#pragma inline sil_wrh_lem
#pragma inline sil_reh_bem
#pragma inline sil_wrh_bem
#pragma inline sil_rew_mem
#pragma inline sil_wrw_mem
#pragma inline sil_rew_lem
#pragma inline sil_wrw_lem
#pragma inline sil_rew_bem
#pragma inline sil_wrw_bem

/* t_syslog.h	*/
#pragma inline t_syslog_0
#pragma inline t_syslog_1
#pragma inline t_syslog_2
#pragma inline t_syslog_3
#pragma inline t_syslog_4
#pragma inline t_syslog_5
#pragma inline t_syslog_6
#pragma inline syslog
#pragma inline t_perror

/*
 *	ターゲット非依存部のInline関数のシンボル登録
 */

/* kernel/ */

/* mutex.c */
#pragma inline mutex_calc_priority

/* task.c */
#pragma inline bitmap_search
#pragma inline primap_empty
#pragma inline primap_search
#pragma inline primap_set
#pragma inline primap_clear

/* task.h */
#pragma inline set_dspflg

/* time_event.c	*/
#pragma inline tmevtb_insert
#pragma inline tmevtb_delete
#pragma inline tmevtb_delete_top
#pragma inline calc_current_evttim_ub

/* wait.c	*/
#pragma inline wobj_queue_insert

/* wait.h	*/
#pragma inline queue_insert_tpri
#pragma inline make_wait
#pragma inline make_non_wait
#pragma inline wait_dequeue_wobj
#pragma inline wait_dequeue_tmevtb
#pragma inline wait_tskid
#pragma inline wobj_change_priority

/* sample1/ */

/* sample1.c */
#pragma inline svc_perror

/* syssvc/ */

/* serial.c	*/
#pragma inline serial_snd_chr

#endif /* KERNEL_INLINE_SYMBOL_H */
