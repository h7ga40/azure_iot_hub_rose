/*
 *  TOPPERS/HRP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      High Reliable system Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2019 by Embedded and Real-Time Systems Laboratory
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
 *		優先度データキュー機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "domain.h"
#include "task.h"
#include "wait.h"
#include "pridataq.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_ACRE_PDQ_ENTER
#define LOG_ACRE_PDQ_ENTER(pk_cpdq)
#endif /* LOG_ACRE_PDQ_ENTER */

#ifndef LOG_ACRE_PDQ_LEAVE
#define LOG_ACRE_PDQ_LEAVE(ercd)
#endif /* LOG_ACRE_PDQ_LEAVE */

#ifndef LOG_SAC_PDQ_ENTER
#define LOG_SAC_PDQ_ENTER(pdqid, p_acvct)
#endif /* LOG_SAC_PDQ_ENTER */

#ifndef LOG_SAC_PDQ_LEAVE
#define LOG_SAC_PDQ_LEAVE(ercd)
#endif /* LOG_SAC_PDQ_LEAVE */

#ifndef LOG_DEL_PDQ_ENTER
#define LOG_DEL_PDQ_ENTER(pdqid)
#endif /* LOG_DEL_PDQ_ENTER */

#ifndef LOG_DEL_PDQ_LEAVE
#define LOG_DEL_PDQ_LEAVE(ercd)
#endif /* LOG_DEL_PDQ_LEAVE */

#ifndef LOG_SND_PDQ_ENTER
#define LOG_SND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_SND_PDQ_ENTER */

#ifndef LOG_SND_PDQ_LEAVE
#define LOG_SND_PDQ_LEAVE(ercd)
#endif /* LOG_SND_PDQ_LEAVE */

#ifndef LOG_PSND_PDQ_ENTER
#define LOG_PSND_PDQ_ENTER(pdqid, data, datapri)
#endif /* LOG_PSND_PDQ_ENTER */

#ifndef LOG_PSND_PDQ_LEAVE
#define LOG_PSND_PDQ_LEAVE(ercd)
#endif /* LOG_PSND_PDQ_LEAVE */

#ifndef LOG_TSND_PDQ_ENTER
#define LOG_TSND_PDQ_ENTER(pdqid, data, datapri, tmout)
#endif /* LOG_TSND_PDQ_ENTER */

#ifndef LOG_TSND_PDQ_LEAVE
#define LOG_TSND_PDQ_LEAVE(ercd)
#endif /* LOG_TSND_PDQ_LEAVE */

#ifndef LOG_RCV_PDQ_ENTER
#define LOG_RCV_PDQ_ENTER(pdqid, p_data, p_datapri)
#endif /* LOG_RCV_PDQ_ENTER */

#ifndef LOG_RCV_PDQ_LEAVE
#define LOG_RCV_PDQ_LEAVE(ercd, p_data, p_datapri)
#endif /* LOG_RCV_PDQ_LEAVE */

#ifndef LOG_PRCV_PDQ_ENTER
#define LOG_PRCV_PDQ_ENTER(pdqid, p_data, p_datapri)
#endif /* LOG_PRCV_PDQ_ENTER */

#ifndef LOG_PRCV_PDQ_LEAVE
#define LOG_PRCV_PDQ_LEAVE(ercd, p_data, p_datapri)
#endif /* LOG_PRCV_PDQ_LEAVE */

#ifndef LOG_TRCV_PDQ_ENTER
#define LOG_TRCV_PDQ_ENTER(pdqid, p_data, p_datapri, tmout)
#endif /* LOG_TRCV_PDQ_ENTER */

#ifndef LOG_TRCV_PDQ_LEAVE
#define LOG_TRCV_PDQ_LEAVE(ercd, p_data, p_datapri)
#endif /* LOG_TRCV_PDQ_LEAVE */

#ifndef LOG_INI_PDQ_ENTER
#define LOG_INI_PDQ_ENTER(pdqid)
#endif /* LOG_INI_PDQ_ENTER */

#ifndef LOG_INI_PDQ_LEAVE
#define LOG_INI_PDQ_LEAVE(ercd)
#endif /* LOG_INI_PDQ_LEAVE */

#ifndef LOG_REF_PDQ_ENTER
#define LOG_REF_PDQ_ENTER(pdqid, pk_rpdq)
#endif /* LOG_REF_PDQ_ENTER */

#ifndef LOG_REF_PDQ_LEAVE
#define LOG_REF_PDQ_LEAVE(ercd, pk_rpdq)
#endif /* LOG_REF_PDQ_LEAVE */

/*
 *  優先度データキューの数
 */
#define tnum_pdq	((uint_t)(tmax_pdqid - TMIN_PDQID + 1))
#define tnum_spdq	((uint_t)(tmax_spdqid - TMIN_PDQID + 1))

/*
 *  優先度データキューIDから優先度データキュー管理ブロックを取り出すた
 *  めのマクロ
 */
#define INDEX_PDQ(pdqid)	((uint_t)((pdqid) - TMIN_PDQID))
#define get_pdqcb(pdqid)	(&(pdqcb_table[INDEX_PDQ(pdqid)]))

/*
 *  優先度データキュー機能の初期化
 */
#ifdef TOPPERS_pdqini

static void
initialize_pdqcb(PDQCB *p_pdqcb, PDQINIB *p_pdqinib, const DOMINIB *p_dominib)
{
	p_pdqinib->pdqatr = TA_NOEXS;
	p_pdqcb->p_pdqinib = (const PDQINIB *) p_pdqinib;
	queue_insert_prev(&(p_dominib->p_domcb->free_pdqcb),
										&(p_pdqcb->swait_queue));
}

void
initialize_pridataq(void)
{
	uint_t			i, j, k;
	ID				domid;
	PDQCB			*p_pdqcb;
	const DOMINIB	*p_dominib;

	for (i = 0; i < tnum_spdq; i++) {
		p_pdqcb = &(pdqcb_table[i]);
		queue_initialize(&(p_pdqcb->swait_queue));
		p_pdqcb->p_pdqinib = &(pdqinib_table[i]);
		queue_initialize(&(p_pdqcb->rwait_queue));
		p_pdqcb->count = 0U;
		p_pdqcb->p_head = NULL;
		p_pdqcb->unused = 0U;
		p_pdqcb->p_freelist = NULL;
	}

	queue_initialize(&(dominib_kernel.p_domcb->free_pdqcb));
	for (j = 0; j < dominib_kernel.tnum_apdqid; i++, j++) {
		initialize_pdqcb(&(pdqcb_table[i]), &(apdqinib_table[j]),
													&dominib_kernel);
	}
	for (domid = TMIN_DOMID; domid <= tmax_domid; domid++) {
		p_dominib = get_dominib(domid);
		queue_initialize(&(p_dominib->p_domcb->free_pdqcb));
		for (k = 0; k < p_dominib->tnum_apdqid; i++, j++, k++) {
			initialize_pdqcb(&(pdqcb_table[i]), &(apdqinib_table[j]),
															p_dominib);
		}
	}
	for (k = 0; k < dominib_none.tnum_apdqid; i++, j++, k++) {
		initialize_pdqcb(&(pdqcb_table[i]), &(apdqinib_table[j]),
													&dominib_none);
	}
}

#endif /* TOPPERS_pdqini */

/*
 *  優先度データキュー管理領域へのデータの格納
 */
#ifdef TOPPERS_pdqenq

void
enqueue_pridata(PDQCB *p_pdqcb, intptr_t data, PRI datapri)
{
	PDQMB	*p_pdqmb;
	PDQMB	**pp_prev_next, *p_next;

	if (p_pdqcb->p_freelist != NULL) {
		p_pdqmb = p_pdqcb->p_freelist;
		p_pdqcb->p_freelist = p_pdqmb->p_next;
	}
	else {
		p_pdqmb = p_pdqcb->p_pdqinib->p_pdqmb + p_pdqcb->unused;
		p_pdqcb->unused++;
	}

	p_pdqmb->data = data;
	p_pdqmb->datapri = datapri;

	pp_prev_next = &(p_pdqcb->p_head);
	while ((p_next = *pp_prev_next) != NULL) {
		if (p_next->datapri > datapri) {
			break;
		}
		pp_prev_next = &(p_next->p_next);
	}
	p_pdqmb->p_next = p_next;
	*pp_prev_next = p_pdqmb;
	p_pdqcb->count++;
}

#endif /* TOPPERS_pdqenq */

/*
 *  優先度データキュー管理領域からのデータの取出し
 */
#ifdef TOPPERS_pdqdeq

void
dequeue_pridata(PDQCB *p_pdqcb, intptr_t *p_data, PRI *p_datapri)
{
	PDQMB	*p_pdqmb;

	p_pdqmb = p_pdqcb->p_head;
	p_pdqcb->p_head = p_pdqmb->p_next;
	p_pdqcb->count--;

	*p_data = p_pdqmb->data;
	*p_datapri = p_pdqmb->datapri;

	p_pdqmb->p_next = p_pdqcb->p_freelist;
	p_pdqcb->p_freelist = p_pdqmb;
}

#endif /* TOPPERS_pdqdeq */

/*
 *  優先度データキューへのデータ送信
 */
#ifdef TOPPERS_pdqsnd

bool_t
send_pridata(PDQCB *p_pdqcb, intptr_t data, PRI datapri)
{
	TCB		*p_tcb;

	if (!queue_empty(&(p_pdqcb->rwait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->rwait_queue));
		((WINFO_RPDQ *)(p_tcb->p_winfo))->data = data;
		((WINFO_RPDQ *)(p_tcb->p_winfo))->datapri = datapri;
		wait_complete(p_tcb);
		return(true);
	}
	else if (p_pdqcb->count < p_pdqcb->p_pdqinib->pdqcnt) {
		enqueue_pridata(p_pdqcb, data, datapri);
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_pdqsnd */

/*
 *  優先度データキューからのデータ受信
 */
#ifdef TOPPERS_pdqrcv

bool_t
receive_pridata(PDQCB *p_pdqcb, intptr_t *p_data, PRI *p_datapri)
{
	TCB		*p_tcb;
	intptr_t data;
	PRI		datapri;

	if (p_pdqcb->count > 0U) {
		dequeue_pridata(p_pdqcb, p_data, p_datapri);
		if (!queue_empty(&(p_pdqcb->swait_queue))) {
			p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->swait_queue));
			data = ((WINFO_SPDQ *)(p_tcb->p_winfo))->data;
			datapri = ((WINFO_SPDQ *)(p_tcb->p_winfo))->datapri;
			enqueue_pridata(p_pdqcb, data, datapri);
			wait_complete(p_tcb);
		}
		return(true);
	}
	else if (!queue_empty(&(p_pdqcb->swait_queue))) {
		p_tcb = (TCB *) queue_delete_next(&(p_pdqcb->swait_queue));
		*p_data = ((WINFO_SPDQ *)(p_tcb->p_winfo))->data;
		*p_datapri = ((WINFO_SPDQ *)(p_tcb->p_winfo))->datapri;
		wait_complete(p_tcb);
		return(true);
	}
	else {
		return(false);
	}
}

#endif /* TOPPERS_pdqrcv */

/*
 *  優先度データキューの生成
 */
#ifdef TOPPERS_acre_pdq

ER_UINT
acre_pdq(const T_CPDQ *pk_cpdq)
{
	PDQCB			*p_pdqcb;
	PDQINIB			*p_pdqinib;
	ATR				pdqatr;
	uint_t			pdqcnt;
	PRI				maxdpri;
	PDQMB			*p_pdqmb;
	ID				domid;
	const DOMINIB	*p_dominib;
	ACPTN			acptn;
	ER				ercd;

	LOG_ACRE_PDQ_ENTER(pk_cpdq);
	CHECK_TSKCTX_UNL();
	CHECK_MACV_READ(pk_cpdq, T_CPDQ);

	pdqatr = pk_cpdq->pdqatr;
	pdqcnt = pk_cpdq->pdqcnt;
	maxdpri = pk_cpdq->maxdpri;
	p_pdqmb = pk_cpdq->pdqmb;

	CHECK_VALIDATR(pdqatr, TA_TPRI|TA_DOMMASK);
	CHECK_PAR(VALID_DPRI(maxdpri));
	if (p_pdqmb != NULL) {
		CHECK_PAR(MB_ALIGN(p_pdqmb));
		CHECK_OBJ(valid_memobj_kernel(p_pdqmb, sizeof(PDQMB) * pdqcnt));
	}
	domid = get_atrdomid(pdqatr);
	if (domid == TDOM_SELF) {
		if (rundom == TACP_KERNEL) {
			domid = TDOM_KERNEL;
		}
		else {
			domid = p_runtsk->p_tinib->domid;
		}
		pdqatr = set_atrdomid(pdqatr, domid);
	}
	switch (domid) {
	case TDOM_KERNEL:
		p_dominib = &dominib_kernel;
		break;
	case TDOM_NONE:
		p_dominib = &dominib_none;
		break;
	default:
		CHECK_RSATR(VALID_DOMID(domid));
		p_dominib = get_dominib(domid);
		break;
	}
	CHECK_ACPTN(p_dominib->acvct.acptn1);

	lock_cpu();
	if (tnum_pdq == 0 || queue_empty(&(p_dominib->p_domcb->free_pdqcb))) {
		ercd = E_NOID;
	}
	else {
		if (pdqcnt != 0 && p_pdqmb == NULL) {
			p_pdqmb = malloc_mpk(sizeof(PDQMB) * pdqcnt, p_dominib);
			pdqatr |= TA_MBALLOC;
		}
		if (pdqcnt != 0 && p_pdqmb == NULL) {
			ercd = E_NOMEM;
		}
		else {
			p_pdqcb = (PDQCB *)
						queue_delete_next(&(p_dominib->p_domcb->free_pdqcb));
			p_pdqinib = (PDQINIB *)(p_pdqcb->p_pdqinib);
			p_pdqinib->pdqatr = pdqatr;
			p_pdqinib->pdqcnt = pdqcnt;
			p_pdqinib->maxdpri = maxdpri;
			p_pdqinib->p_pdqmb = p_pdqmb;

			acptn = default_acptn(domid);
			p_pdqinib->acvct.acptn1 = acptn;
			p_pdqinib->acvct.acptn2 = acptn;
			p_pdqinib->acvct.acptn3 = p_dominib->acvct.acptn1;
			p_pdqinib->acvct.acptn4 = acptn;

			queue_initialize(&(p_pdqcb->swait_queue));
			queue_initialize(&(p_pdqcb->rwait_queue));
			p_pdqcb->count = 0U;
			p_pdqcb->p_head = NULL;
			p_pdqcb->unused = 0U;
			p_pdqcb->p_freelist = NULL;
			ercd = PDQID(p_pdqcb);
		}
	}
	unlock_cpu();

  error_exit:
	LOG_ACRE_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_acre_pdq */

/*
 *  優先度データキューのアクセス許可ベクタの設定
 */
#ifdef TOPPERS_sac_pdq

ER
sac_pdq(ID pdqid, const ACVCT *p_acvct)
{
	PDQCB	*p_pdqcb;
	PDQINIB	*p_pdqinib;
	ER		ercd;

	LOG_SAC_PDQ_ENTER(pdqid, p_acvct);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_MACV_READ(p_acvct, ACVCT);
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn3)) {
		ercd = E_OACV;
	}
	else if (PDQID(p_pdqcb) <= tmax_spdqid) {
		ercd = E_OBJ;
	}
	else {
		p_pdqinib = (PDQINIB *)(p_pdqcb->p_pdqinib);
		p_pdqinib->acvct = *p_acvct;
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_SAC_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sac_pdq */

/*
 *  優先度データキューの削除
 */
#ifdef TOPPERS_del_pdq

ER
del_pdq(ID pdqid)
{
	PDQCB			*p_pdqcb;
	PDQINIB			*p_pdqinib;
	const DOMINIB	*p_dominib;
	ER				ercd;

	LOG_DEL_PDQ_ENTER(pdqid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn3)) {
		ercd = E_OACV;
	}
	else if (PDQID(p_pdqcb) <= tmax_spdqid) {
		ercd = E_OBJ;
	}
	else {
		init_wait_queue(&(p_pdqcb->swait_queue));
		init_wait_queue(&(p_pdqcb->rwait_queue));
		p_pdqinib = (PDQINIB *)(p_pdqcb->p_pdqinib);
		p_dominib = get_atrdominib(p_pdqinib->pdqatr);
		p_pdqinib->pdqatr = TA_NOEXS;
		queue_insert_prev(&(p_dominib->p_domcb->free_pdqcb),
										&(p_pdqcb->swait_queue));
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_DEL_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_del_pdq */

/*
 *  優先度データキューへの送信
 */
#ifdef TOPPERS_snd_pdq

ER
snd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB		*p_pdqcb;
	WINFO_SPDQ	winfo_spdq;
	ER			ercd;

	LOG_SND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_DISPATCH();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_PAR(TMIN_DPRI <= datapri);
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu_dsp();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn1)) {
		ercd = E_OACV;
	}
	else if (!(datapri <= p_pdqcb->p_pdqinib->maxdpri)) {
		ercd = E_PAR;
	}
	else if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (send_pridata(p_pdqcb, data, datapri)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		winfo_spdq.data = data;
		winfo_spdq.datapri = datapri;
		wobj_make_wait((WOBJCB *) p_pdqcb, TS_WAITING_SPDQ,
											(WINFO_WOBJ *) &winfo_spdq);
		dispatch();
		ercd = winfo_spdq.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_SND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_pdq */

/*
 *  優先度データキューへの送信（ポーリング）
 */
#ifdef TOPPERS_psnd_pdq

ER
psnd_pdq(ID pdqid, intptr_t data, PRI datapri)
{
	PDQCB	*p_pdqcb;
	ER		ercd;

	LOG_PSND_PDQ_ENTER(pdqid, data, datapri);
	CHECK_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_PAR(TMIN_DPRI <= datapri);
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn1)) {
		ercd = E_OACV;
	}
	else if (!(datapri <= p_pdqcb->p_pdqinib->maxdpri)) {
		ercd = E_PAR;
	}
	else if (send_pridata(p_pdqcb, data, datapri)) {
		if (p_runtsk != p_schedtsk) {
			if (!sense_context()) {
				dispatch();
			}
			else {
				request_dispatch_retint();
			}
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	unlock_cpu();

  error_exit:
	LOG_PSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_psnd_pdq */

/*
 *  優先度データキューへの送信（タイムアウトあり）
 */
#ifdef TOPPERS_tsnd_pdq

ER
tsnd_pdq(ID pdqid, intptr_t data, PRI datapri, TMO tmout)
{
	PDQCB		*p_pdqcb;
	WINFO_SPDQ	winfo_spdq;
	TMEVTB		tmevtb;
	ER			ercd;

	LOG_TSND_PDQ_ENTER(pdqid, data, datapri, tmout);
	CHECK_DISPATCH();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_PAR(TMIN_DPRI <= datapri);
	CHECK_PAR(VALID_TMOUT(tmout));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu_dsp();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn1)) {
		ercd = E_OACV;
	}
	else if (!(datapri <= p_pdqcb->p_pdqinib->maxdpri)) {
		ercd = E_PAR;
	}
	else if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (send_pridata(p_pdqcb, data, datapri)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		winfo_spdq.data = data;
		winfo_spdq.datapri = datapri;
		wobj_make_wait_tmout((WOBJCB *) p_pdqcb, TS_WAITING_SPDQ,
								(WINFO_WOBJ *) &winfo_spdq, &tmevtb, tmout);
		dispatch();
		ercd = winfo_spdq.winfo.wercd;
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_TSND_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tsnd_pdq */

/*
 *  優先度データキューからの受信
 */
#ifdef TOPPERS_rcv_pdq

ER
rcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri)
{
	PDQCB		*p_pdqcb;
	WINFO_RPDQ	winfo_rpdq;
	ER			ercd;

	LOG_RCV_PDQ_ENTER(pdqid, p_data, p_datapri);
	CHECK_DISPATCH();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_MACV_WRITE(p_data, intptr_t);
	CHECK_MACV_WRITE(p_datapri, PRI);
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu_dsp();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn2)) {
		ercd = E_OACV;
	}
	else if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (receive_pridata(p_pdqcb, p_data, p_datapri)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		make_wait(TS_WAITING_RPDQ, &(winfo_rpdq.winfo));
		queue_insert_prev(&(p_pdqcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_rpdq.p_pdqcb = p_pdqcb;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_rpdq.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo_rpdq.data;
			*p_datapri = winfo_rpdq.datapri;
		}
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_RCV_PDQ_LEAVE(ercd, p_data, p_datapri);
	return(ercd);
}

#endif /* TOPPERS_rcv_pdq */

/*
 *  優先度データキューからの受信（ポーリング）
 */
#ifdef TOPPERS_prcv_pdq

ER
prcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri)
{
	PDQCB	*p_pdqcb;
	ER		ercd;

	LOG_PRCV_PDQ_ENTER(pdqid, p_data, p_datapri);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_MACV_WRITE(p_data, intptr_t);
	CHECK_MACV_WRITE(p_datapri, PRI);
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn2)) {
		ercd = E_OACV;
	}
	else if (receive_pridata(p_pdqcb, p_data, p_datapri)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	unlock_cpu();

  error_exit:
	LOG_PRCV_PDQ_LEAVE(ercd, p_data, p_datapri);
	return(ercd);
}

#endif /* TOPPERS_prcv_pdq */

/*
 *  優先度データキューからの受信（タイムアウトあり）
 */
#ifdef TOPPERS_trcv_pdq

ER
trcv_pdq(ID pdqid, intptr_t *p_data, PRI *p_datapri, TMO tmout)
{
	PDQCB		*p_pdqcb;
	WINFO_RPDQ	winfo_rpdq;
	TMEVTB		tmevtb;
	ER			ercd;

	LOG_TRCV_PDQ_ENTER(pdqid, p_data, p_datapri, tmout);
	CHECK_DISPATCH();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_MACV_WRITE(p_data, intptr_t);
	CHECK_MACV_WRITE(p_datapri, PRI);
	CHECK_PAR(VALID_TMOUT(tmout));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu_dsp();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn2)) {
		ercd = E_OACV;
	}
	else if (p_runtsk->raster) {
		ercd = E_RASTER;
	}
	else if (receive_pridata(p_pdqcb, p_data, p_datapri)) {
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		ercd = E_TMOUT;
	}
	else {
		make_wait_tmout(TS_WAITING_RPDQ, &(winfo_rpdq.winfo), &tmevtb, tmout);
		queue_insert_prev(&(p_pdqcb->rwait_queue), &(p_runtsk->task_queue));
		winfo_rpdq.p_pdqcb = p_pdqcb;
		LOG_TSKSTAT(p_runtsk);
		dispatch();
		ercd = winfo_rpdq.winfo.wercd;
		if (ercd == E_OK) {
			*p_data = winfo_rpdq.data;
			*p_datapri = winfo_rpdq.datapri;
		}
	}
	unlock_cpu_dsp();

  error_exit:
	LOG_TRCV_PDQ_LEAVE(ercd, p_data, p_datapri);
	return(ercd);
}

#endif /* TOPPERS_trcv_pdq */

/*
 *  優先度データキューの再初期化
 */
#ifdef TOPPERS_ini_pdq

ER
ini_pdq(ID pdqid)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
    
	LOG_INI_PDQ_ENTER(pdqid);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn3)) {
		ercd = E_OACV;
	}
	else {
		init_wait_queue(&(p_pdqcb->swait_queue));
		init_wait_queue(&(p_pdqcb->rwait_queue));
		p_pdqcb->count = 0U;
		p_pdqcb->p_head = NULL;
		p_pdqcb->unused = 0U;
		p_pdqcb->p_freelist = NULL;
		if (p_runtsk != p_schedtsk) {
			dispatch();
		}
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_INI_PDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_pdq */

/*
 *  優先度データキューの状態参照
 */
#ifdef TOPPERS_ref_pdq

ER
ref_pdq(ID pdqid, T_RPDQ *pk_rpdq)
{
	PDQCB	*p_pdqcb;
	ER		ercd;
    
	LOG_REF_PDQ_ENTER(pdqid, pk_rpdq);
	CHECK_TSKCTX_UNL();
	CHECK_ID(VALID_PDQID(pdqid));
	CHECK_MACV_WRITE(pk_rpdq, T_RPDQ);
	p_pdqcb = get_pdqcb(pdqid);

	lock_cpu();
	if (p_pdqcb->p_pdqinib->pdqatr == TA_NOEXS) {
		ercd = E_NOEXS;
	}
	else if (VIOLATE_ACPTN(p_pdqcb->p_pdqinib->acvct.acptn4)) {
		ercd = E_OACV;
	}
	else {
		pk_rpdq->stskid = wait_tskid(&(p_pdqcb->swait_queue));
		pk_rpdq->rtskid = wait_tskid(&(p_pdqcb->rwait_queue));
		pk_rpdq->spdqcnt = p_pdqcb->count;
		ercd = E_OK;
	}
	unlock_cpu();

  error_exit:
	LOG_REF_PDQ_LEAVE(ercd, pk_rpdq);
	return(ercd);
}

#endif /* TOPPERS_ref_pdq */
