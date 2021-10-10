/*
 *  TOPPERS/HRP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      High Reliable system Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2020 by Embedded and Real-Time Systems Laboratory
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
 *		周期通知機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "domain.h"
#include "cyclic.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_CYC_ENTER
#define LOG_CYC_ENTER(p_cyccb)
#endif /* LOG_CYC_ENTER */

#ifndef LOG_CYC_LEAVE
#define LOG_CYC_LEAVE(p_cyccb)
#endif /* LOG_CYC_LEAVE */

#ifndef LOG_ACRE_CYC_ENTER
#define LOG_ACRE_CYC_ENTER(pk_ccyc)
#endif /* LOG_ACRE_CYC_ENTER */

#ifndef LOG_ACRE_CYC_LEAVE
#define LOG_ACRE_CYC_LEAVE(ercd)
#endif /* LOG_ACRE_CYC_LEAVE */

#ifndef LOG_SAC_CYC_ENTER
#define LOG_SAC_CYC_ENTER(cycid, p_acvct)
#endif /* LOG_SAC_CYC_ENTER */

#ifndef LOG_SAC_CYC_LEAVE
#define LOG_SAC_CYC_LEAVE(ercd)
#endif /* LOG_SAC_CYC_LEAVE */

#ifndef LOG_DEL_CYC_ENTER
#define LOG_DEL_CYC_ENTER(cycid)
#endif /* LOG_DEL_CYC_ENTER */

#ifndef LOG_DEL_CYC_LEAVE
#define LOG_DEL_CYC_LEAVE(ercd)
#endif /* LOG_DEL_CYC_LEAVE */

#ifndef LOG_STA_CYC_ENTER
#define LOG_STA_CYC_ENTER(cycid)
#endif /* LOG_STA_CYC_ENTER */

#ifndef LOG_STA_CYC_LEAVE
#define LOG_STA_CYC_LEAVE(ercd)
#endif /* LOG_STA_CYC_LEAVE */

#ifndef LOG_STP_CYC_ENTER
#define LOG_STP_CYC_ENTER(cycid)
#endif /* LOG_STP_CYC_ENTER */

#ifndef LOG_STP_CYC_LEAVE
#define LOG_STP_CYC_LEAVE(ercd)
#endif /* LOG_STP_CYC_LEAVE */

#ifndef LOG_REF_CYC_ENTER
#define LOG_REF_CYC_ENTER(cycid, pk_rcyc)
#endif /* LOG_REF_CYC_ENTER */

#ifndef LOG_REF_CYC_LEAVE
#define LOG_REF_CYC_LEAVE(ercd, pk_rcyc)
#endif /* LOG_REF_CYC_LEAVE */

/*
 *  周期通知の数
 */
#define tnum_cyc	((uint_t)(tmax_cycid - TMIN_CYCID + 1))
#define tnum_scyc	((uint_t)(tmax_scycid - TMIN_CYCID + 1))

/*
 *  周期通知IDから周期通知管理ブロックを取り出すためのマクロ
 */
#define INDEX_CYC(cycid)	((uint_t)((cycid) - TMIN_CYCID))
#define get_cyccb(cycid)	(&(cyccb_table[INDEX_CYC(cycid)]))

/*
 *  周期通知機能の初期化
 *
 *  未使用の周期通知管理ブロックのキューを作るために，周期通知管理ブロッ
 *  クの先頭にはキューにつなぐための領域がないため，タイムイベントブロッ
 *  ク（tmevtb）の領域を用いる．
 */
#ifdef TOPPERS_cycini

static void
initialize_cyccb(CYCCB *p_cyccb, CYCINIB *p_cycinib, const DOMINIB *p_dominib)
{
	p_cycinib->p_tmevt_heap = p_dominib->p_tmevt_heap;
	p_cycinib->cycatr = TA_NOEXS;
	p_cyccb->p_cycinib = (const CYCINIB *) p_cycinib;
	p_cyccb->tmevtb.callback = (CBACK) call_cyclic;
	p_cyccb->tmevtb.arg = (void *) p_cyccb;
	queue_insert_prev(&(p_dominib->p_domcb->free_cyccb),
								((QUEUE *) &(p_cyccb->tmevtb)));
}

void
initialize_cyclic(void)
{
	uint_t			i, j, k;
	ID				domid;
	CYCCB			*p_cyccb;
	const DOMINIB	*p_dominib;

	for (i = 0; i < tnum_scyc; i++) {
		p_cyccb = &(cyccb_table[i]);
		p_cyccb->p_cycinib = &(cycinib_table[i]);
		p_cyccb->tmevtb.callback = (CBACK) call_cyclic;
		p_cyccb->tmevtb.arg = (void *) p_cyccb;
		if ((p_cyccb->p_cycinib->cycatr & TA_STA) != 0U) {
			/*
			 *  初回の起動のためのタイムイベントを登録する［ASPD1035］
			 *  ［ASPD1062］．
			 */
			p_cyccb->cycsta = true;
			p_cyccb->tmevtb.evttim = (EVTTIM)(p_cyccb->p_cycinib->cycphs);
			tmevtb_register(&(p_cyccb->tmevtb),
									p_cyccb->p_cycinib->p_tmevt_heap);
		}
		else {
			p_cyccb->cycsta = false;
		}
	}

	queue_initialize(&(dominib_kernel.p_domcb->free_cyccb));
	for (j = 0; j < dominib_kernel.tnum_acycid; i++, j++) {
		initialize_cyccb(&(cyccb_table[i]), &(acycinib_table[j]),
													&dominib_kernel);
	}
	for (domid = TMIN_DOMID; domid <= tmax_domid; domid++) {
		p_dominib = get_dominib(domid);
		queue_initialize(&(p_dominib->p_domcb->free_cyccb));
		for (k = 0; k < p_dominib->tnum_acycid; i++, j++, k++) {
			initialize_cyccb(&(cyccb_table[i]), &(acycinib_table[j]),
															p_dominib);
		}
	}
}

#endif /* TOPPERS_cycini */

/*
 *  周期通知の生成
 */
#ifdef TOPPERS_acre_cyc

ER_UINT
acre_cyc(const T_CCYC *pk_ccyc)
{
	CYCCB			*p_cyccb;
	CYCINIB			*p_cycinib;
	ATR				cycatr;
	RELTIM			cyctim, cycphs;
	ID				domid;
	const DOMINIB	*p_dominib;
	T_NFYINFO		*p_nfyinfo;
	ACPTN			acptn;
	ER				ercd;

	LOG_ACRE_CYC_ENTER(pk_ccyc);
	CHECK_TSKCTX_UNL();
	CHECK_MACV_READ(pk_ccyc, T_CCYC);

	cycatr = pk_ccyc->cycatr;
	cyctim = pk_ccyc->cyctim;
	cycphs = pk_ccyc->cycphs;

	CHECK_VALIDATR(cycatr, TA_STA|TA_DOMMASK);
	CHECK_PAR(0 < cyctim && cyctim <= TMAX_RELTIM);
	CHECK_PAR(0 <= cycphs && cycphs <= TMAX_RELTIM);
	domid = get_atrdomid(cycatr);
	if (domid == TDOM_SELF) {
		if (rundom == TACP_KERNEL) {
			domid = TDOM_KERNEL;
		}
		else {
			domid = p_runtsk->p_tinib->domid;
		}
		cycatr = set_atrdomid(cycatr, domid);
	}
	if (domid == TDOM_KERNEL) {
		p_dominib = &dominib_kernel;
	}
	else {
		CHECK_RSATR(VALID_DOMID(domid));
		p_dominib = get_dominib(domid);
	}
	ercd = check_nfyinfo(&(pk_ccyc->nfyinfo), p_dominib->domptn);
	if (ercd != E_OK) {
		goto error_exit;
	}
	CHECK_ACPTN(p_dominib->acvct.acptn1);
	
	lock_cpu();
	if (tnum_cyc == 0 || queue_empty(&(p_dominib->p_domcb->free_cyccb))) {
		ercd = E_NOID;
	}
	else {
		p_cyccb = (CYCCB *)
			(((char *) queue_delete_next(&(p_dominib->p_domcb->free_cyccb)))
													- offsetof(CYCCB, tmevtb));
		p_cycinib = (CYCINIB *)(p_cyccb->p_cycinib);
		p_cycinib->cycatr = cycatr;
		if (pk_ccyc->nfyinfo.nfymode == TNFY_HANDLER) {
			p_cycinib->exinf = pk_ccyc->nfyinfo.nfy.handler.exinf;
			p_cycinib->nfyhdr = (NFYHDR)(pk_ccyc->nfyinfo.nfy.handler.tmehdr);
		}
		else {
			p_nfyinfo = &acyc_nfyinfo_table[p_cycinib - acycinib_table];
			*p_nfyinfo = pk_ccyc->nfyinfo;
			p_cycinib->exinf = (EXINF) p_nfyinfo;
			p_cycinib->nfyhdr = notify_handler;
		}
		p_cycinib->cyctim = cyctim;
		p_cycinib->cycphs = cycphs;

		acptn = default_acptn(domid);
		p_cycinib->acvct.acptn1 = acptn;
		p_cycinib->acvct.acptn2 = acptn;
		p_cycinib->acvct.acptn3 = p_dominib->acvct.acptn1;
		p_cycinib->acvct.acptn4 = acptn;

		if ((p_cyccb->p_cycinib->cycatr & TA_STA) != 0U) {
			p_cyccb->cycsta = true;
			tmevtb_enqueue_reltim(&(p_cyccb->tmevtb),
									p_cyccb->p_cycinib->cycphs,
									p_cyccb->p_cycinib->p_tmevt_heap);
		}
		else {
			p_cyccb->cycsta = false;
		}
		ercd = CYCID(p_cyccb);
	}
	unlock_cpu();

  error_exit:
	LOG_ACRE_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_cyc */

/*
 *  周期通知のアクセス許可ベクタの設定
 */
#ifdef TOPPERS_sac_cyc

ER
sac_cyc(ID cycid, const ACVCT *p_acvct)
{
	CYCCB	*p_cyccb;
	CYCINIB	*p_cycinib;
	ER		ercd;

	LOG_SAC_CYC_ENTER(cycid, p_acvct);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	CHECK_MACV_READ(p_acvct, ACVCT);
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_cyccb->p_cycinib->acvct.acptn3)) {
		ercd = E_OACV;
	}
	else if (CYCID(p_cyccb) <= tmax_scycid) {
		ercd = E_OBJ;
	}
	else {
		p_cycinib = (CYCINIB *)(p_cyccb->p_cycinib);
		p_cycinib->acvct = *p_acvct;
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_SAC_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sac_cyc */

/*
 *  周期通知の削除
 */
#ifdef TOPPERS_del_cyc

ER
del_cyc(ID cycid)
{
	CYCCB			*p_cyccb;
	CYCINIB			*p_cycinib;
	const DOMINIB	*p_dominib;
	ER				ercd;

	LOG_DEL_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_cyccb->p_cycinib->acvct.acptn3)) {
		ercd = E_OACV;
	}
	else if (CYCID(p_cyccb) <= tmax_scycid) {
		ercd = E_OBJ;
	}
	else {
		if (p_cyccb->cycsta) {
			p_cyccb->cycsta = false;
			tmevtb_dequeue(&(p_cyccb->tmevtb),
									p_cyccb->p_cycinib->p_tmevt_heap);
		}

		p_cycinib = (CYCINIB *)(p_cyccb->p_cycinib);
		p_dominib = get_atrdominib(p_cycinib->cycatr);
		p_cycinib->cycatr = TA_NOEXS;
		queue_insert_prev(&(p_dominib->p_domcb->free_cyccb),
									((QUEUE *) &(p_cyccb->tmevtb)));
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_DEL_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_cyc */

/*
 *  周期通知の動作開始
 */
#ifdef TOPPERS_sta_cyc

ER
sta_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_STA_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_cyccb->p_cycinib->acvct.acptn1)) {
		ercd = E_OACV;
	}
	else {
		if (p_cyccb->cycsta) {
			tmevtb_dequeue(&(p_cyccb->tmevtb),
									p_cyccb->p_cycinib->p_tmevt_heap);
		}
		else {
			p_cyccb->cycsta = true;
		}
	}
	/*
	 *  初回の起動のためのタイムイベントを登録する［ASPD1036］．
	 */
	tmevtb_enqueue_reltim(&(p_cyccb->tmevtb), p_cyccb->p_cycinib->cycphs,
									p_cyccb->p_cycinib->p_tmevt_heap);
	ercd = E_OK;
	unlock_cpu();

  error_exit:
	LOG_STA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_cyc */

/*
 *  周期通知の動作停止
 */
#ifdef TOPPERS_stp_cyc

ER
stp_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;

	LOG_STP_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_cyccb->p_cycinib->acvct.acptn2)) {
		ercd = E_OACV;
	}
	else {
		if (p_cyccb->cycsta) {
			p_cyccb->cycsta = false;
			tmevtb_dequeue(&(p_cyccb->tmevtb),
									p_cyccb->p_cycinib->p_tmevt_heap);
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_STP_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_cyc */

/*
 *  周期通知の状態参照
 */
#ifdef TOPPERS_ref_cyc

ER
ref_cyc(ID cycid, T_RCYC *pk_rcyc)
{
	CYCCB	*p_cyccb;
	ER		ercd;
    
	LOG_REF_CYC_ENTER(cycid, pk_rcyc);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_CYCID(cycid));
	CHECK_MACV_WRITE(pk_rcyc, T_RCYC);
	p_cyccb = get_cyccb(cycid);

	lock_cpu();
	if (p_cyccb->p_cycinib->cycatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_cyccb->p_cycinib->acvct.acptn4)) {
		ercd = E_OACV;
	}
	else {
		if (p_cyccb->cycsta) {
			pk_rcyc->cycstat = TCYC_STA;
			pk_rcyc->lefttim = tmevt_lefttim(&(p_cyccb->tmevtb));
		}
		else {
			pk_rcyc->cycstat = TCYC_STP;
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_REF_CYC_LEAVE(ercd, pk_rcyc);
	return(ercd);
}

#endif /* TOPPERS_ref_cyc */

/*
 *  周期通知起動ルーチン
 */
#ifdef TOPPERS_cyccal

void
call_cyclic(CYCCB *p_cyccb)
{
	/*
	 *  次回の起動のためのタイムイベントを登録する［ASPD1037］．
	 *
	 *  tmevtb_enqueueを用いるのが素直であるが，この関数は高分解能タイ
	 *  マ割込みの処理中でのみ呼び出されるため，tmevtb_registerを用い
	 *  ている．
	 */
	p_cyccb->tmevtb.evttim += p_cyccb->p_cycinib->cyctim;	/*［ASPD1038］*/
	tmevtb_register(&(p_cyccb->tmevtb), p_cyccb->p_cycinib->p_tmevt_heap);

	/*
	 *  通知ハンドラを，CPUロック解除状態で呼び出す．
	 *
	 *  周期通知の生成／削除はタスクからしか行えないため，周期通知初期
	 *  化ブロックをCPUロック解除状態で参照しても問題ない．
	 */
	unlock_cpu();

	LOG_CYC_ENTER(p_cyccb);
	(*(p_cyccb->p_cycinib->nfyhdr))(p_cyccb->p_cycinib->exinf);
	LOG_CYC_LEAVE(p_cyccb);

	if (!sense_lock()) {
		lock_cpu();
	}
}

#endif /* TOPPERS_cyccal */
