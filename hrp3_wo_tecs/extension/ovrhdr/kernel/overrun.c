/*
 *  TOPPERS/HRP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      High Reliable system Profile Kernel
 * 
 *  Copyright (C) 2005-2018 by Embedded and Real-Time Systems Laboratory
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
 *		オーバランハンドラ機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "overrun.h"

#ifdef TOPPERS_SUPPORT_OVRHDR
#include "target_timer.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_OVR_ENTER
#define LOG_OVR_ENTER(p_runtsk)
#endif /* LOG_OVR_ENTER */

#ifndef LOG_OVR_LEAVE
#define LOG_OVR_LEAVE(p_runtsk)
#endif /* LOG_OVR_LEAVE */

#ifndef LOG_STA_OVR_ENTER
#define LOG_STA_OVR_ENTER(tskid, ovrtim)
#endif /* LOG_STA_OVR_ENTER */

#ifndef LOG_STA_OVR_LEAVE
#define LOG_STA_OVR_LEAVE(ercd)
#endif /* LOG_STA_OVR_LEAVE */

#ifndef LOG_STP_OVR_ENTER
#define LOG_STP_OVR_ENTER(tskid)
#endif /* LOG_STP_OVR_ENTER */

#ifndef LOG_STP_OVR_LEAVE
#define LOG_STP_OVR_LEAVE(ercd)
#endif /* LOG_STP_OVR_LEAVE */

#ifndef LOG_REF_OVR_ENTER
#define LOG_REF_OVR_ENTER(tskid, pk_rovr)
#endif /* LOG_REF_OVR_ENTER */

#ifndef LOG_REF_OVR_LEAVE
#define LOG_REF_OVR_LEAVE(ercd, pk_rovr)
#endif /* LOG_REF_OVR_LEAVE */

/*
 *  オーバランタイマの動作開始
 */
#ifdef TOPPERS_ovrsta
#ifndef OMIT_OVRTIMER_START

void
ovrtimer_start(void)
{
	if (p_runtsk->staovr) {
		target_ovrtimer_start(p_runtsk->leftotm);
	}
}

#endif /* OMIT_OVRTIMER_START */
#endif /* TOPPERS_ovrsta */

/*
 *  オーバランタイマの停止
 */
#ifdef TOPPERS_ovrstp
#ifndef OMIT_OVRTIMER_STOP

void
ovrtimer_stop(void)
{
	if (p_runtsk != NULL && p_runtsk->staovr) {
		p_runtsk->leftotm = target_ovrtimer_stop();
	}
}

#endif /* OMIT_OVRTIMER_STOP */
#endif /* TOPPERS_ovrstp */

/*
 *  オーバランハンドラの動作開始
 */
#ifdef TOPPERS_sta_ovr

ER
sta_ovr(ID tskid, PRCTIM ovrtim)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_STA_OVR_ENTER(tskid, ovrtim);
	CHECK_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	if (tskid == TSK_SELF && !sense_context()) {
		p_tcb = p_runtsk;
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));
		p_tcb = get_tcb(tskid);
	}
	CHECK_PAR(0U < ovrtim && ovrtim <= TMAX_OVRTIM);
	CHECK_ACPTN(p_tcb->p_tinib->acvct.acptn2);

	lock_cpu();
	if (!sense_context() && p_tcb == p_runtsk) {
		if (p_runtsk->staovr) {
			(void) target_ovrtimer_stop();
		}
		target_ovrtimer_start(ovrtim);
	}
	p_tcb->staovr = true;
	p_tcb->leftotm = ovrtim;
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_STA_OVR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_ovr */

/*
 *  オーバランハンドラの動作停止
 */
#ifdef TOPPERS_stp_ovr

ER
stp_ovr(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;

	LOG_STP_OVR_ENTER(tskid);
	CHECK_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	if (tskid == TSK_SELF && !sense_context()) {
		p_tcb = p_runtsk;
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));
		p_tcb = get_tcb(tskid);
	}
	CHECK_ACPTN(p_tcb->p_tinib->acvct.acptn2);

	lock_cpu();
	if (!sense_context() && p_tcb == p_runtsk) {
		if (p_runtsk->staovr) {
			(void) target_ovrtimer_stop();
		}
	}
	p_tcb->staovr = false;
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_STP_OVR_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_ovr */

/*
 *  オーバランハンドラの状態参照
 */
#ifdef TOPPERS_ref_ovr

ER
ref_ovr(ID tskid, T_ROVR *pk_rovr)
{
	TCB		*p_tcb;
	ER		ercd;
    
	LOG_REF_OVR_ENTER(tskid, pk_rovr);
	CHECK_TSKCTX_UNL();
	CHECK_OBJ(ovrinib.ovrhdr != NULL);
	if (tskid == TSK_SELF) {
		p_tcb = p_runtsk;
	}
	else {
		CHECK_ID(VALID_TSKID(tskid));
		p_tcb = get_tcb(tskid);
	}
	CHECK_MACV_WRITE(pk_rovr, T_ROVR);
	CHECK_ACPTN(p_tcb->p_tinib->acvct.acptn4);

	lock_cpu();
	if (p_tcb->staovr) {
		pk_rovr->ovrstat = TOVR_STA;
		if (p_tcb == p_runtsk) {
			pk_rovr->leftotm = target_ovrtimer_get_current();
		}
		else {
			pk_rovr->leftotm = p_tcb->leftotm;
		}
	}
	else {
		pk_rovr->ovrstat = TOVR_STP;
	}
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_REF_OVR_LEAVE(ercd, pk_rovr);
	return(ercd);
}

#endif /* TOPPERS_ref_ovr */

/*
 *  オーバランハンドラ起動ルーチン
 */
#ifdef TOPPERS_ovrcal

void
call_ovrhdr(void)
{
	assert(sense_context());
	assert(!sense_lock());
	assert(ovrinib.ovrhdr != NULL);

	lock_cpu();
	if (p_runtsk != NULL && p_runtsk->staovr && p_runtsk->leftotm == 0U) {
		p_runtsk->staovr = false;
		unlock_cpu();

		LOG_OVR_ENTER(p_runtsk);
		((OVRHDR)(ovrinib.ovrhdr))(TSKID(p_runtsk),
											p_runtsk->p_tinib->exinf);
		LOG_OVR_LEAVE(p_runtsk);
	}
	else {
		/*
		 *  このルーチンが呼び出される前に，オーバランハンドラの起動が
		 *  キャンセルされた場合
		 */
		unlock_cpu();
	}
}

#endif /* TOPPERS_ovrcal */
#endif /* TOPPERS_SUPPORT_OVRHDR */
