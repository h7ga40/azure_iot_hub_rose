/*
 *  TOPPERS/HRP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      High Reliable system Profile Kernel
 * 
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
 *		カーネルの整合性検査
 */

#include "tBitKernel_tecsgen.h"
#include "kernel/kernel_impl.h"
#include "kernel/task.h"
#include "kernel/wait.h"
#include "kernel/semaphore.h"
#include "kernel/eventflag.h"
#include "kernel/dataqueue.h"
#include "kernel/pridataq.h"
#include "kernel/mutex.h"
#include "kernel/messagebuf.h"
#include "kernel/mempfix.h"
#include "kernel/cyclic.h"
#include "kernel/alarm.h"
#include "kernel/time_event.h"
#include "kernel/check.h"

/*
 *   エラーコードの定義
 */
#define E_SYS_LINENO	ERCD(E_SYS, -(__LINE__))

/*
 *  ターゲット依存の検査のデフォルト定義
 */
#ifndef ON_SSTACK
#define ON_SSTACK(addr, size, p_tskinictxb)	(true)
#endif /* ON_SSTACK */

#ifndef VALID_TSKCTXB
#define VALID_TSKCTXB(p_tskctxb, p_tcb)		(true)
#endif /* BIT_TSKCTXB */

/*
 *  整合性検査に必要な定義
 */
#define p_top_tmevtn(p_tmevt_heap)	((p_tmevt_heap) + 1)
#define p_last_tmevtn(p_tmevt_heap)	((p_tmevt_heap)->p_last)

/*
 *  管理ブロックのアドレスの正当性のチェック
 */
#define VALID_SCHEDCB(p_schedcb) \
		((((char *)(p_schedcb)) - ((char *) schedcb_table))		  \
	 										% sizeof(SCHECB) == 0 \
			&& 0 <= ((p_schedcb) - schedcb_table)				  \
			&& ((p_schedcb) - schedcb_table) < tnum_schedcb

#define VALID_TCB(p_tcb) \
		((((char *)(p_tcb)) - ((char *) tcb_table)) % sizeof(TCB) == 0 \
			&& TMIN_TSKID <= TSKID(p_tcb) && TSKID(p_tcb) <= tmax_tskid)

#define VALID_SEMCB(p_semcb) \
		((((char *)(p_semcb)) - ((char *) semcb_table)) % sizeof(SEMCB) == 0 \
			&& TMIN_SEMID <= SEMID(p_semcb) && SEMID(p_semcb) <= tmax_semid)
				
#define VALID_FLGCB(p_flgcb) \
		((((char *)(p_flgcb)) - ((char *) flgcb_table)) % sizeof(FLGCB) == 0 \
			&& TMIN_FLGID <= FLGID(p_flgcb) && FLGID(p_flgcb) <= tmax_flgid)

#define VALID_DTQCB(p_dtqcb) \
		((((char *)(p_dtqcb)) - ((char *) dtqcb_table)) % sizeof(DTQCB) == 0 \
			&& TMIN_DTQID <= DTQID(p_dtqcb) && DTQID(p_dtqcb) <= tmax_dtqid)

#define VALID_PDQCB(p_pdqcb) \
		((((char *)(p_pdqcb)) - ((char *) pdqcb_table)) % sizeof(PDQCB) == 0 \
			&& TMIN_PDQID <= PDQID(p_pdqcb) && PDQID(p_pdqcb) <= tmax_pdqid)

#define VALID_MTXCB(p_mtxcb) \
		((((char *)(p_mtxcb)) - ((char *) mtxcb_table)) % sizeof(MTXCB) == 0 \
			&& TMIN_MTXID <= MTXID(p_mtxcb) && MTXID(p_mtxcb) <= tmax_mtxid)

#define VALID_MBFCB(p_mbfcb) \
		((((char *)(p_mbfcb)) - ((char *) mbfcb_table)) % sizeof(MBFCB) == 0 \
			&& TMIN_MBFID <= MBFID(p_mbfcb) && MBFID(p_mbfcb) <= tmax_mbfid)

#define VALID_MPFCB(p_mpfcb) \
		((((char *)(p_mpfcb)) - ((char *) mpfcb_table)) % sizeof(MPFCB) == 0 \
			&& TMIN_MPFID <= MPFID(p_mpfcb) && MPFID(p_mpfcb) <= tmax_mpfid)

#define VALID_TMEVTN(p_tmevtn, p_tmevt_heap) \
		((((char *)(p_tmevtn)) - ((char *) p_tmevt_heap)) % sizeof(TMEVTN) == 0\
			&& p_top_tmevtn(p_tmevt_heap) <= (p_tmevtn)						   \
			&& (p_tmevtn) <= p_last_tmevtn(p_tmevt_heap))

/*
 *  初期化ブロックのアドレスの正当性のチェック
 */
#define VALID_TWDINIB(p_twdinib) \
		((((char *) p_twdinib) - ((char *) twdinib_table)) \
				 										% sizeof(TWDINIB) == 0

#define VALID_SOMINIB(p_sominib) \
		((((char *) p_sominib) - ((char *) sominib_table)) \
				 										% sizeof(SOMINIB) == 0 \
			&& TMIN_SOMID <= SOMID(p_sominib) && SOMID(p_sominib) <= tmax_somid)

/*
 *  ミューテックスのプロトコルを判断するマクロ
 */
#define MTXPROTO_MASK			0x03U
#define MTXPROTO(p_mtxinib)		((p_mtxinib)->mtxatr & MTXPROTO_MASK)
#define MTX_CEILING(p_mtxinib)	(MTXPROTO(p_mtxinib) == TA_CEILING)

/*
 *  キューのチェックのための関数
 *
 *  p_queueにp_entryが含まれているかを調べる．含まれていればtrue，含ま
 *  れていない場合にはfalseを返す．ダブルリンクの不整合の場合にも，
 *  falseを返す．
 */
static bool_t
in_queue(QUEUE *p_queue, QUEUE *p_entry)
{
	QUEUE	*p_current, *p_next;

	p_current = p_queue->p_next;
	if (p_current->p_prev != p_queue) {
		return(false);					/* ダブルリンクの不整合 */
	}
	while (p_current != p_queue) {
		if (p_current == p_entry) {
			return(true);				/* p_entryが含まれていた */
		}

		/*
		 *  キューの次の要素に進む
		 */
		p_next = p_current->p_next;
		if (p_next->p_prev != p_current) {
			return(false);				 /* ダブルリンクの不整合 */
		}
		p_current = p_next;
	}
	return(false);
}

/*
 *  システムスタック上を指しているかの検査
 */
static bool_t
on_sstack(void *addr, size_t size, const TINIB *p_tinib)
{
#ifdef USE_TSKINICTXB
	/*
	 *  ターゲット依存の検査
	 */
	return(ON_SSTACK(addr, size, &(p_tinib->tskinictxb));
#else /* USE_TSKINICTXB */
	if (p_tinib->sstk <= addr && ((char *) addr) + size
								<= (char *)(p_tinib->sstk) + p_tinib->sstksz) {
		return(true);
	}
	return(false);
#endif /* USE_TSKINICTXB */
}

/*
 *  タイムイベントブロックの検査
 */
static bool_t
valid_tmevtb(TMEVTB *p_tmevtb, TMEVTN *p_tmevt_heap)
{
	/*
	 *  p_tmevtb->p_tmevtnの検査
	 */
	if (!VALID_TMEVTN(p_tmevtb->p_tmevtn, p_tmevt_heap)) {
		return(false);
	}
	return(true);
}

/*
 *  スケジューリング単位管理ブロックの整合性検査
 */
#ifndef PRIMAP_BIT
#define	PRIMAP_BIT(pri)		(1U << (pri))
#endif /* PRIMAP_BIT */

Inline bool_t
primap_empty(SCHEDCB *p_schedcb)
{
#ifndef PRIMAP_LEVEL_2
	return(p_schedcb->ready_primap == 0U);
#else /* PRIMAP_LEVEL_2 */
	return(p_schedcb->ready_primap1 == 0U);
#endif /* PRIMAP_LEVEL_2 */
}

Inline uint16_t
primap_extract_bit(uint_t pri, SCHEDCB *p_schedcb)
{
#ifndef PRIMAP_LEVEL_2
	return(p_schedcb->ready_primap & PRIMAP_BIT(pri));
#else /* PRIMAP_LEVEL_2 */
	return(p_schedcb->ready_primap2[pri / TBIT_PRIMAP]
									& PRIMAP_BIT(pri % TBIT_PRIMAP));
#endif /* PRIMAP_LEVEL_2 */
}

static ER
bit_schedcb(SCHEDCB *p_schedcb)
{
	uint_t	pri;
	QUEUE	*p_queue;
	TCB		*p_tcb;

	/*
	 *  p_predtskの整合性検査
	 */
	if (primap_empty(p_schedcb)) {
		if (p_schedcb->p_predtsk != NULL) {
			return(E_SYS_LINENO);
		}
	}
	else {
		if (p_schedcb->p_predtsk != search_predtsk(p_schedcb)) {
			return(E_SYS_LINENO);
		}
	}

#ifdef PRIMAP_LEVEL_2
	/*
	 *  ready_primap1とready_primap2の整合性検査
	 */
	for (pri = 0; pri < TNUM_TPRI; pri += TBIT_PRIMAP) {
		if (p_schedcb->ready_primap2[pri / TBIT_PRIMAP] == 0U) {
			if ((p_schedcb->ready_primap1 & PRIMAP_BIT(pri / TBIT_PRIMAP))
																	!= 0U) {
				return(E_SYS_LINENO);
			}
		}
		else {
			if ((p_schedcb->ready_primap1 & PRIMAP_BIT(pri / TBIT_PRIMAP))
																	== 0U) {
				return(E_SYS_LINENO);
			}
		}
	}
#endif /* PRIMAP_LEVEL_2 */

	/*
	 *  ready_queueとready_primapの整合性検査
	 */
	for (pri = 0; pri < TNUM_TPRI; pri++) {
		p_queue = p_schedcb->ready_queue[pri].p_next;
		if (p_queue == &(p_schedcb->ready_queue[pri])) {
			if (primap_extract_bit(pri, p_schedcb) != 0U) {
				return(E_SYS_LINENO);
			}
		}
		else {
			if (primap_extract_bit(pri, p_schedcb) == 0U) {
				return(E_SYS_LINENO);
			}
		}
		while (p_queue != &(p_schedcb->ready_queue[pri])) {
			p_tcb = (TCB *) p_queue;
			if (!VALID_TCB(p_tcb)) {
				return(E_SYS_LINENO);
			}
			if (!TSTAT_RUNNABLE(p_tcb->tstat)) {
				return(E_SYS_LINENO);
			}
			if (p_tcb->priority != pri) {
				return(E_SYS_LINENO);
			}
			p_queue = p_queue->p_next;
		}
	}
	return(E_OK);
}

/*
 *  タイムイベントヒープの整合性検査
 */
#define LCHILD(p_tmevtn, p_tmevt_heap) \
					((p_tmevt_heap) + (((p_tmevtn) - (p_tmevt_heap)) << 1))
#define RCHILD(p_tmevtn, p_tmevt_heap) \
					((p_tmevt_heap) + (((p_tmevtn) - (p_tmevt_heap)) << 1) + 1)
#define EVTTIM_ADVANCE(t)	((t) - boundary_evttim)
#define EVTTIM_LE(t1, t2)	(EVTTIM_ADVANCE(t1) <= EVTTIM_ADVANCE(t2))

static ER
bit_tmevt_heap(TMEVTN *p_tmevt_heap)
{
	TMEVTN	*p_tmevtn, *p_child;
	TMEVTB	*p_tmevtb;

	/*
	 *  p_last_tmevtn（＝p_tmevt_heap[0].p_last）の検査
	 */
	if (!(p_tmevt_heap <= p_last_tmevtn(p_tmevt_heap))) {
		return(E_SYS_LINENO);
	}
	if ((((char *) p_last_tmevtn(p_tmevt_heap)) - ((char *) p_tmevt_heap))
												% sizeof(TMEVTN) != 0) {
		return(E_SYS_LINENO);
	}
#if 0
	if (!(p_last_tmevtn(p_tmevt_heap)
				< &p_tmevt_heap[1 + tnum_tsk + tnum_cyc + tnum_alm])) {
		return(E_SYS_LINENO);
	}
#endif

	/*
	 *  設定されているタイムイベント数（notmevt）の検査
	 */
#if 0
	if (notmevt != (p_last_tmevtn(p_tmevt_heap) - p_tmevt_heap)) {
		return(E_SYS_LINENO);
	}
#endif

	/*
	 *  タイムイベントノード毎の検査
	 */
	for (p_tmevtn = p_top_tmevtn(p_tmevt_heap);
					p_tmevtn <= p_last_tmevtn(p_tmevt_heap); p_tmevtn++) {
		/*
		 *  対応するタイムイベントブロックを取得
		 */
		p_tmevtb = p_tmevtn->p_tmevtb;

		/*
		 *  p_tmevtnの検査
		 */
		if (p_tmevtn != p_tmevtb->p_tmevtn) {
			return(E_SYS_LINENO);
		}

		/*
		 *  evttimの検査
		 */
		/* 左の子ノードよりイベント発生時刻が早いか同じ */
		if ((p_child = LCHILD(p_tmevtn, p_tmevt_heap))
										<= p_last_tmevtn(p_tmevt_heap)) {
			if (!EVTTIM_LE(p_tmevtb->evttim, p_child->p_tmevtb->evttim)) {
				return(E_SYS_LINENO);
			}
		}
		/* 右の子ノードよりイベント発生時刻が早いか同じ */
		if ((p_child = RCHILD(p_tmevtn, p_tmevt_heap))
										<= p_last_tmevtn(p_tmevt_heap)) {
			if (!EVTTIM_LE(p_tmevtb->evttim, p_child->p_tmevtb->evttim)) {
				return(E_SYS_LINENO);
			}
		}
	}
	return(E_OK);
}

/*
 *  保護ドメイン関連の整合性検査
 */
static ER
bit_domain(void)
{
	ID		domid;
	uint_t	i;
	ER		ercd;

	/*
	 *  p_schedtskの整合性検査
	 */
	if (dspflg) {
		if (schedcb_kernel.p_predtsk != NULL) {
			if (p_twdsched->p_predtsk != NULL
					&& p_twdsched->p_predtsk->priority
								< schedcb_kernel.p_predtsk->priority) {
				if (p_schedtsk != p_twdsched->p_predtsk) {
					return(E_SYS_LINENO);
				}
			}
			else {
				if (p_schedtsk != schedcb_kernel.p_predtsk) {
					return(E_SYS_LINENO);
				}
			}
		}
		else {
			if (p_twdsched->p_predtsk != NULL) {
				if (p_schedtsk != p_twdsched->p_predtsk) {
					return(E_SYS_LINENO);
				}
			}
			else {
				if (p_schedtsk != p_idlesched->p_predtsk) {
					return(E_SYS_LINENO);
				}
			}
		}
	}

	/*
	 *  スケジューリング単位毎の整合性検査
	 */
	ercd = bit_schedcb(&schedcb_kernel);
	if (ercd != E_OK) {
		return(ercd);
	}
	ercd = bit_schedcb(&schedcb_idle);
	if (ercd != E_OK) {
		return(ercd);
	}
	for (i = 0; i < tnum_schedcb; i++) {
		ercd = bit_schedcb(&(schedcb_table[i]));
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  タイムイベントヒープ毎の整合性検査
	 */
	ercd = bit_tmevt_heap(tmevt_heap_kernel);
	if (ercd != E_OK) {
		return(ercd);
	}
	if (system_cyctim != 0U) {
		ercd = bit_tmevt_heap(tmevt_heap_idle);
		if (ercd != E_OK) {
			return(ercd);
		}
	}
	for (domid = TMIN_DOMID; domid <= tmax_domid; domid++) {
		ercd = bit_tmevt_heap(get_dominib(domid)->p_tmevt_heap);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  p_cursom，p_runtwd，p_twdsched，p_idleschedの整合性検査
	 */
	if (system_cyctim == 0U) {
		/*
		 *  時間パーティショニングを使用しない場合の検査
		 */
		if (p_cursom != NULL) {
			return(E_SYS_LINENO);
		}
		if (p_runtwd != NULL) {
			return(E_SYS_LINENO);
		}
		if (p_twdsched != &schedcb_idle) {
			return(E_SYS_LINENO);
		}
		if (p_idlesched != &schedcb_idle) {
			return(E_SYS_LINENO);
		}
	}
	else if (p_cursom == NULL) {
		/*
		 *  システム周期停止モードの場合の検査
		 */
		if (p_runtwd != NULL) {
			return(E_SYS_LINENO);
		}
		if (p_twdsched != &schedcb_kernel) {
			return(E_SYS_LINENO);
		}
		if (p_idlesched != &schedcb_kernel) {
			return(E_SYS_LINENO);
		}
	}
	else {
		/*
		 *  システム周期停止モードでない場合の検査
		 */
		if (!VALID_SOMINIB(p_cursom)) {
			return(E_SYS_LINENO);
		}

		if (p_runtwd == NULL) {
			/*
			 *  アイドルウィンドウの場合の検査
			 */
			if (p_twdsched != &schedcb_idle) {
				return(E_SYS_LINENO);
			}
			if (p_idlesched != &schedcb_idle) {
				return(E_SYS_LINENO);
			}
		}
		else {
			/*
			 *  アイドルウィンドウでない場合の検査
			 */
			if (!(VALID_TWDINIB(p_runtwd) && p_cursom->p_twdinib <= p_runtwd
									&& p_runtwd < (p_cursom + 1)->p_twdinib))) {
				return(E_SYS_LINENO);
			}
			if (p_twdsched != p_runtwd->p_dominib->p_schedcb) {
				return(E_SYS_LINENO);
			}
			if (p_idlesched != &schedcb_idle) {
				return(E_SYS_LINENO);
			}
		}
	}

	return(E_OK);
}

/*
 *  タスク毎の整合性検査
 */
static ER
bit_task(ID tskid)
{
	TCB				*p_tcb;
	const TINIB		*p_tinib;
	const DOMINIB	*p_dominib;
	SCHEDCB			*p_schedcb;
	uint_t			tstat, bpri, pri, raised_pri;
	TMEVTB			*p_tmevtb;
	SEMCB			*p_semcb;
	FLGCB			*p_flgcb;
	DTQCB			*p_dtqcb;
	PDQCB			*p_pdqcb;
	MTXCB			*p_mtxcb;
	const MTXINIB	*p_mtxinib;
	MBFCB			*p_mbfcb;
	MPFCB			*p_mpfcb;
	size_t			winfo_size;

	if (!VALID_TSKID(tskid)) {
		return(E_ID);
	}
	p_tcb = get_tcb(tskid);
	p_tinib = p_tcb->p_tinib;
	p_dominib = p_tcb->p_dominib;
	p_schedcb = p_tcb->p_schedcb;
	tstat = p_tcb->tstat;
	bpri = p_tcb->bpriority;
	pri = p_tcb->priority;

	/*
	 *  タスク初期化ブロックへのポインタの整合性検査
	 */
	if (p_tinib != &(tinib_table[INDEX_TSK(tskid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  保護ドメイン初期化ブロックとスケジューリング単位管理ブロックへ
	 *  のポインタの検査
	 */
	if (p_tinib->domid == TDOM_KERNEL) {
		if (p_dominib != &dominib_kernel) {
			return(E_SYS_LINENO);
		}
		if (p_schedcb != &schedcb_kernel) {
			return(E_SYS_LINENO);
		}
	}
	else {
		if (p_dominib != get_dominib(p_tinib->domid)) {
			return(E_SYS_LINENO);
		}
		if (p_schedcb != p_dominib->p_schedcb) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  tstatの整合性検査
	 */
	if (TSTAT_DORMANT(tstat)) {
		if (tstat != TS_DORMANT) {
			return(E_SYS_LINENO);
		}
	}
	else if (TSTAT_WAITING(tstat)) {
		if ((tstat & ~(TS_WAITING_MASK | TS_SUSPENDED)) != 0U) {
			return(E_SYS_LINENO);
		}
	}
	else if (TSTAT_SUSPENDED(tstat)) {
		if (tstat != TS_SUSPENDED) {
			return(E_SYS_LINENO);
		}
	}
	else {
		if (tstat != TS_RUNNABLE) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  拡張サービスコールのネストレベル（svclevel）の検査
	 */
	if (!sense_context() && p_tcb == p_runtsk
										&& p_tinib->domid != TDOM_KERNEL) {
		if (rundom == TACP_KERNEL) {
			if (p_tcb->svclevel == 0) {
				return(E_SYS_LINENO);
			}
		}
		else {
			if (p_tcb->svclevel > 0) {
				return(E_SYS_LINENO);
			}
		}
	}

	/*
	 *  ベース優先度の検査
	 */
	if (bpri >= TNUM_TPRI) {
		return(E_SYS_LINENO);
	}

	/*
	 *  rasterと他の状態の整合性検査
	 */
	if (p_tcb->raster && (p_tcb->enater || TSTAT_WAITING(tstat))) {
		return(E_SYS_LINENO);
	}

	/*
	 *  休止状態における整合性検査
	 */
	if (TSTAT_DORMANT(tstat)) {
		if (!(bpri == p_tinib->ipriority
			  		&& pri == p_tinib->ipriority
					&& p_tcb->actque == false
					&& p_tcb->wupque == false
					&& p_tcb->raster == false
					&& p_tcb->enater == true
					&& p_tcb->p_lastmtx == NULL)) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  実行できる状態における整合性検査
	 */
	if (TSTAT_RUNNABLE(tstat)) {
		if (!in_queue(&(p_schedcb->ready_queue[pri]), &(p_tcb->task_queue))) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  待ち状態における整合性検査
	 */
	if (TSTAT_WAITING(tstat)) {
		p_tmevtb = p_tcb->p_winfo->p_tmevtb;
		if (p_tmevtb != NULL) {
			if (!on_sstack(p_tmevtb, sizeof(TMEVTB), p_tinib)) {
				return(E_SYS_LINENO);
			}
			if (!valid_tmevtb(p_tmevtb, p_tcb->p_dominib->p_tmevt_heap)) {
				return(E_SYS_LINENO);
			}
			if ((tstat & TS_WAITING_MASK) != TS_WAITING_DLY) {
				if (p_tmevtb->callback != (CBACK) wait_tmout) {
					return(E_SYS_LINENO);
				}
			}
			else {
				if (p_tmevtb->callback != (CBACK) wait_tmout_ok) {
					return(E_SYS_LINENO);
				}
			}
			if (p_tmevtb->arg != (void *) p_tcb) {
				return(E_SYS_LINENO);
			}
		}

		switch (tstat & TS_WAITING_MASK) {
		case TS_WAITING_SLP:
			if (p_tcb->wupque == true) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO);
			break;

		case TS_WAITING_DLY:
			if (p_tmevtb == NULL) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO);
			break;

		case TS_WAITING_SEM:
			p_semcb = ((WINFO_SEM *)(p_tcb->p_winfo))->p_semcb;
			if (!VALID_SEMCB(p_semcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_semcb->wait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_SEM);
			break;

		case TS_WAITING_FLG:
			p_flgcb = ((WINFO_FLG *)(p_tcb->p_winfo))->p_flgcb;
			if (!VALID_FLGCB(p_flgcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_flgcb->wait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_FLG);
			break;

		case TS_WAITING_SDTQ:
			p_dtqcb = ((WINFO_SDTQ *)(p_tcb->p_winfo))->p_dtqcb;
			if (!VALID_DTQCB(p_dtqcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_dtqcb->swait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_SDTQ);
			break;

		case TS_WAITING_RDTQ:
			p_dtqcb = ((WINFO_RDTQ *)(p_tcb->p_winfo))->p_dtqcb;
			if (!VALID_DTQCB(p_dtqcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_dtqcb->rwait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_RDTQ);
			break;

		case TS_WAITING_SPDQ:
			p_pdqcb = ((WINFO_SPDQ *)(p_tcb->p_winfo))->p_pdqcb;
			if (!VALID_PDQCB(p_pdqcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_pdqcb->swait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_SPDQ);
			break;

		case TS_WAITING_RPDQ:
			p_pdqcb = ((WINFO_RPDQ *)(p_tcb->p_winfo))->p_pdqcb;
			if (!VALID_PDQCB(p_pdqcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_pdqcb->rwait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_RPDQ);
			break;

		case TS_WAITING_MTX:
			p_mtxcb = ((WINFO_MTX *)(p_tcb->p_winfo))->p_mtxcb;
			if (!VALID_MTXCB(p_mtxcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_mtxcb->wait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_MTX);
			break;

		case TS_WAITING_SMBF:
			p_mbfcb = ((WINFO_SMBF *)(p_tcb->p_winfo))->p_mbfcb;
			if (!VALID_MBFCB(p_mbfcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_mbfcb->swait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_SMBF);
			break;

		case TS_WAITING_RMBF:
			p_mbfcb = ((WINFO_RMBF *)(p_tcb->p_winfo))->p_mbfcb;
			if (!VALID_MBFCB(p_mbfcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_mbfcb->rwait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_RMBF);
			break;

		case TS_WAITING_MPF:
			p_mpfcb = ((WINFO_MPF *)(p_tcb->p_winfo))->p_mpfcb;
			if (!VALID_MPFCB(p_mpfcb)) {
				return(E_SYS_LINENO);
			}
			if (!in_queue(&(p_mpfcb->wait_queue), &(p_tcb->task_queue))) {
				return(E_SYS_LINENO);
			}
			winfo_size = sizeof(WINFO_MPF);
			break;

		default:
			return(E_SYS_LINENO);
			break;
		}

		if (!on_sstack(p_tcb->p_winfo, winfo_size, p_tinib)) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  p_lastmtxの検査
	 */
	p_mtxcb = p_tcb->p_lastmtx;
	raised_pri = p_tcb->bpriority;
	while (p_mtxcb != NULL) {
		if (!VALID_MTXCB(p_mtxcb)) {
			return(E_SYS_LINENO);
		}
		p_mtxinib = p_mtxcb->p_mtxinib;

		/*
		 *  ミューテックスをロックしているタスクのチェック
		 */
		if (p_mtxcb->p_loctsk != p_tcb) {
			return(E_SYS_LINENO);
		}

		/*
		 *  現在優先度の計算
		 */
		if (MTXPROTO(p_mtxinib)) {
			if (p_mtxinib->ceilpri < raised_pri) {
				raised_pri = p_mtxinib->ceilpri;
			}
		}

		/*
		 *  キューの次の要素に進む
		 */
		p_mtxcb = p_mtxcb->p_prevmtx;
	}

	/*
	 *  現在優先度の検査
	 */
	if (pri != raised_pri) {
		return(E_SYS_LINENO);
	}

	/*
	 *  tskctxbの検査
	 */
	if (!TSTAT_DORMANT(tstat) && p_tcb != p_runtsk) {
		/*
		 *  ターゲット依存の検査
		 */
		if (!VALID_TSKCTXB(&(p_tcb->tskctxb), p_tcb)) {
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  セマフォ毎の整合性検査
 */
#define INDEX_SEM(semid)	((uint_t)((semid) - TMIN_SEMID))
#define get_semcb(semid)	(&(semcb_table[INDEX_SEM(semid)]))

static ER
bit_semaphore(ID semid)
{
	SEMCB			*p_semcb;
	const SEMINIB	*p_seminib;
	uint_t			pri;
	QUEUE			*p_queue;
	TCB				*p_tcb;

	if (!VALID_SEMID(semid)) {
		return(E_ID);
	}
	p_semcb = get_semcb(semid);
	p_seminib = p_semcb->p_seminib;

	/*
	 *  セマフォ初期化ブロックへのポインタの検査
	 */
	if (p_seminib != &(seminib_table[INDEX_SEM(semid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  semcntの検査
	 */
	if (p_semcb->semcnt > p_seminib->maxsem) {
		return(E_SYS_LINENO);
	}

	/*
	 *  wait_queueの検査
	 */
	if (p_semcb->semcnt == 0) {
		p_queue = &(p_semcb->wait_queue);
		pri = TMIN_TPRI;
		while ((p_queue = p_queue->p_next) != &(p_semcb->wait_queue)) {
			p_tcb = (TCB *) p_queue;
			if (!VALID_TCB(p_tcb)) {
				return(E_SYS_LINENO);
			}

			/*
			 *  キューがタスク優先度順になっているかの検査
			 */
			if ((p_seminib->sematr & TA_TPRI) != 0U) {
				if (p_tcb->priority < pri) {
					return(E_SYS_LINENO);
				}
			}
			pri = p_tcb->priority;

			/*
			 *  タスク状態の検査
			 */
			if (p_tcb->tstat != TS_WAITING_SEM) {
				return(E_SYS_LINENO);
			}
			if (p_semcb != ((WINFO_SEM *)(p_tcb->p_winfo))->p_semcb) {
				return(E_SYS_LINENO);
			}
		}
	}
	else {
		if (!queue_empty(&(p_semcb->wait_queue))) {
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  イベントフラグ毎の整合性検査
 */
#define INDEX_FLG(flgid)	((uint_t)((flgid) - TMIN_FLGID))
#define get_flgcb(flgid)	(&(flgcb_table[INDEX_FLG(flgid)]))

static ER
bit_eventflag(ID flgid)
{
	FLGCB			*p_flgcb;
	const FLGINIB	*p_flginib;
	uint_t			pri;
	QUEUE			*p_queue;
	TCB				*p_tcb;
	FLGPTN			waiptn;

	if (!VALID_FLGID(flgid)) {
		return(E_ID);
	}
	p_flgcb = get_flgcb(flgid);
	p_flginib = p_flgcb->p_flginib;

	/*
	 *  イベントフラグ初期化ブロックへのポインタの検査
	 */
	if (p_flginib != &(flginib_table[INDEX_FLG(flgid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  wait_queueの検査
	 */
	p_queue = &(p_flgcb->wait_queue);
	pri = TMIN_TPRI;
	while ((p_queue = p_queue->p_next) != &(p_flgcb->wait_queue)) {
		p_tcb = (TCB *) p_queue;
		if (!VALID_TCB(p_tcb)) {
			return(E_SYS_LINENO);
		}

		/*
		 *  キューがタスク優先度順になっているかの検査
		 */
		if ((p_flginib->flgatr & TA_TPRI) != 0U) {
			if (p_tcb->priority < pri) {
				return(E_SYS_LINENO);
			}
		}
		pri = p_tcb->priority;

		/*
		 *  タスク状態の検査
		 */
		if ((p_tcb->tstat & TS_WAITING_MASK) != TS_WAITING_FLG) {
			return(E_SYS_LINENO);
		}
		if (p_flgcb != ((WINFO_FLG *)(p_tcb->p_winfo))->p_flgcb) {
			return(E_SYS_LINENO);
		}

		waiptn = ((WINFO_FLG *)(p_tcb->p_winfo))->waiptn;
		switch (((WINFO_FLG *)(p_tcb->p_winfo))->wfmode) {
		case TWF_ORW:
			if ((p_flgcb->flgptn & waiptn) != 0U) {
				return(E_SYS_LINENO);
			}
			break;

		case TWF_ANDW:
			if ((p_flgcb->flgptn & waiptn) == waiptn) {
				return(E_SYS_LINENO);
			}
			break;

		default:
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  データキュー毎の整合性検査
 */
#define INDEX_DTQ(dtqid)	((uint_t)((dtqid) - TMIN_DTQID))
#define get_dtqcb(dtqid)	(&(dtqcb_table[INDEX_DTQ(dtqid)]))

static ER
bit_dataqueue(ID dtqid)
{
	DTQCB			*p_dtqcb;
	const DTQINIB	*p_dtqinib;
	uint_t			pri;
	QUEUE			*p_queue;
	TCB				*p_tcb;

	if (!VALID_DTQID(dtqid)) {
		return(E_ID);
	}
	p_dtqcb = get_dtqcb(dtqid);
	p_dtqinib = p_dtqcb->p_dtqinib;

	/*
	 *  データキュー初期化ブロックへのポインタの検査
	 */
	if (p_dtqinib != &(dtqinib_table[INDEX_DTQ(dtqid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  countの検査
	 */
	if (p_dtqcb->count > p_dtqinib->dtqcnt) {
		return(E_SYS_LINENO);
	}

	/*
	 *  headの検査
	 */
	if (p_dtqcb->head > p_dtqinib->dtqcnt) {
		return(E_SYS_LINENO);
	}

	/*
	 *  tailの検査
	 */
	if (p_dtqcb->tail
				!= ((p_dtqcb->tail + p_dtqcb->count) % p_dtqinib->dtqcnt)) {
		return(E_SYS_LINENO);
	}

	/*
	 *  swait_queueの検査
	 */
	if (p_dtqcb->count == p_dtqinib->dtqcnt) {
		p_queue = &(p_dtqcb->swait_queue);
		pri = TMIN_TPRI;
		while ((p_queue = p_queue->p_next) != &(p_dtqcb->swait_queue)) {
			p_tcb = (TCB *) p_queue;
			if (!VALID_TCB(p_tcb)) {
				return(E_SYS_LINENO);
			}

			/*
			 *  キューがタスク優先度順になっているかの検査
			 */
			if ((p_dtqinib->dtqatr & TA_TPRI) != 0U) {
				if (p_tcb->priority < pri) {
					return(E_SYS_LINENO);
				}
			}
			pri = p_tcb->priority;

			/*
			 *  タスク状態の検査
			 */
			if ((p_tcb->tstat & TS_WAITING_MASK) != TS_WAITING_SDTQ) {
				return(E_SYS_LINENO);
			}
			if (p_dtqcb != ((WINFO_SDTQ *)(p_tcb->p_winfo))->p_dtqcb) {
				return(E_SYS_LINENO);
			}
		}
	}
	else {
		if (!queue_empty(&(p_dtqcb->swait_queue))) {
			return(E_SYS_LINENO);
		}
	}

	/*
	 *  rwait_queueの検査
	 */
	if (p_dtqcb->count == 0) {
		p_queue = &(p_dtqcb->rwait_queue);
		while ((p_queue = p_queue->p_next) != &(p_dtqcb->rwait_queue)) {
			p_tcb = (TCB *) p_queue;
			if (!VALID_TCB(p_tcb)) {
				return(E_SYS_LINENO);
			}

			/*
			 *  タスク状態の検査
			 */
			if ((p_tcb->tstat & TS_WAITING_MASK) != TS_WAITING_RDTQ) {
				return(E_SYS_LINENO);
			}
			if (p_dtqcb != ((WINFO_RDTQ *)(p_tcb->p_winfo))->p_dtqcb) {
				return(E_SYS_LINENO);
			}
		}
	}
	else {
		if (!queue_empty(&(p_dtqcb->rwait_queue))) {
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  ミューテックス毎の整合性検査
 */
#define INDEX_MTX(mtxid)	((uint_t)((mtxid) - TMIN_MTXID))
#define get_mtxcb(mtxid)	(&(mtxcb_table[INDEX_MTX(mtxid)]))

static ER
bit_mutex(ID mtxid)
{
	MTXCB			*p_mtxcb, *p_locked_mtx;
	const MTXINIB	*p_mtxinib;
	TCB				*p_loctsk, *p_tcb;
	QUEUE			*p_queue;
	uint_t			pri;

	if (!VALID_MTXID(mtxid)) {
		return(E_ID);
	}
	p_mtxcb = get_mtxcb(mtxid);
	p_mtxinib = p_mtxcb->p_mtxinib;
	p_loctsk = p_mtxcb->p_loctsk;

	/*
	 *  ミューテックス初期化ブロックへのポインタの検査
	 */
	if (p_mtxinib != &(mtxinib_table[INDEX_MTX(mtxid)])) {
		return(E_SYS_LINENO);
	}

	if (p_loctsk != NULL) {
		/*
		 *  ミューテックスがロックされている時
		 */

		/*
		 *  wait_queueの検査
		 */
		p_queue = &(p_mtxcb->wait_queue);
		pri = TMIN_TPRI;
		while ((p_queue = p_queue->p_next) != &(p_mtxcb->wait_queue)) {
			p_tcb = (TCB *) p_queue;
			if (!VALID_TCB(p_tcb)) {
				return(E_SYS_LINENO);
			}

			/*
			 *  キューがタスク優先度順になっているかの検査
			 */
			if (MTXPROTO(p_mtxinib) != TA_NULL) {
				if (p_tcb->priority < pri) {
					return(E_SYS_LINENO);
				}
			}
			pri = p_tcb->priority;

			/*
			 *  タスク状態の検査
			 *
			 *  ミューテックス待ち状態のタスクの検査は，タスク毎の検査で行っ
			 *  ているため，ここでは行わない．
			 */
			if (!TSTAT_WAIT_MTX(p_tcb->tstat)) {
				return(E_SYS_LINENO);
			}

			/*
			 *  優先度上限の検査
			 */
			if (MTX_CEILING(p_mtxinib)) {
				if (p_tcb->bpriority < p_mtxinib->ceilpri) {
					return(E_SYS_LINENO);
				}
			}
		}

		/*
		 *  p_loctskの検査
		 *
		 *  ミューテックスをロックしているタスクの検査は，タスク毎の検
		 *  査で行っているため，ここでは行わない．
		 */
		if (!VALID_TCB(p_loctsk)) {
			return(E_SYS_LINENO);
		}
		p_locked_mtx = p_loctsk->p_lastmtx;
		while (p_mtxcb != NULL) {
			if (p_mtxcb == p_locked_mtx) {
				break;
			}
			p_locked_mtx = p_locked_mtx->p_prevmtx;
		}
		if (p_mtxcb == NULL) {
			return(E_SYS_LINENO);
		}

		/*
		 *  優先度上限の検査
		 */
		if (MTX_CEILING(p_mtxinib)) {
			if (p_loctsk->bpriority < p_mtxinib->ceilpri) {
				return(E_SYS_LINENO);
			}
		}
	}
	else {
		/*
		 *  ミューテックスがロックされていない時
		 */
		if (!queue_empty(&(p_mtxcb->wait_queue))) {
			return(E_SYS_LINENO);
		}
	}
	return(E_OK);
}

/*
 *  周期通知毎の整合性検査
 */
#define INDEX_CYC(cycid)	((uint_t)((cycid) - TMIN_CYCID))
#define get_cyccb(cycid)	(&(cyccb_table[INDEX_CYC(cycid)]))

static ER
bit_cyclic(ID cycid)
{
	CYCCB			*p_cyccb;
	const CYCINIB	*p_cycinib;

	if (!VALID_CYCID(cycid)) {
		return(E_ID);
	}
	p_cyccb = get_cyccb(cycid);
	p_cycinib = p_cyccb->p_cycinib;

	/*
	 *  周期通知初期化ブロックへのポインタの検査
	 */
	if (p_cycinib != &(cycinib_table[INDEX_CYC(cycid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  cycstaとtmevtbの検査
	 */
	switch (p_cyccb->cycsta) {
	case false:
		break;

	case true:
		if (!valid_tmevtb(&(p_cyccb->tmevtb),
							p_cyccb->p_cycinib->p_tmevt_heap)) {
			return(E_SYS_LINENO);
		}
		if (p_cyccb->tmevtb.callback != (CBACK) call_cyclic) {
			return(E_SYS_LINENO);
		}
		if (p_cyccb->tmevtb.arg != (void *) p_cyccb) {
			return(E_SYS_LINENO);
		}
		break;

	default:
		return(E_SYS_LINENO);
	}
	return(E_OK);
}

/*
 *  アラーム通知毎の整合性検査
 */
#define INDEX_ALM(almid)	((uint_t)((almid) - TMIN_ALMID))
#define get_almcb(almid)	(&(almcb_table[INDEX_ALM(almid)]))

static ER
bit_alarm(ID almid)
{
	ALMCB			*p_almcb;
	const ALMINIB	*p_alminib;

	if (!VALID_ALMID(almid)) {
		return(E_ID);
	}
	p_almcb = get_almcb(almid);
	p_alminib = p_almcb->p_alminib;

	/*
	 *  周期通知初期化ブロックへのポインタの検査
	 */
	if (p_alminib != &(alminib_table[INDEX_ALM(almid)])) {
		return(E_SYS_LINENO);
	}

	/*
	 *  almstaとtmevtbの検査
	 */
	switch (p_almcb->almsta) {
	case false:
		break;

	case true:
		if (!valid_tmevtb(&(p_almcb->tmevtb),
							p_almcb->p_alminib->p_tmevt_heap)) {
			return(E_SYS_LINENO);
		}
		if (p_almcb->tmevtb.callback != (CBACK) call_alarm) {
			return(E_SYS_LINENO);
		}
		if (p_almcb->tmevtb.arg != (void *) p_almcb) {
			return(E_SYS_LINENO);
		}
		break;

	default:
		return(E_SYS_LINENO);
	}
	return(E_OK);
}

/*
 *  整合性検査関数本体
 */
ER
eBuiltInTest_builtInTest(void)
{
	ID		tskid;
	ID		semid, flgid, dtqid, mtxid;
	ID		cycid, almid;
	ER		ercd;

	/*
	 *  保護ドメイン関連の整合性検査
	 */
	ercd = bit_domain();
	if (ercd != E_OK) {
		return(ercd);
	}

	/*
	 *  タスク毎の整合性検査
	 */
	for (tskid = TMIN_TSKID; tskid <= tmax_tskid; tskid++) {
		ercd = bit_task(tskid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  セマフォ毎の整合性検査
	 */
	for (semid = TMIN_SEMID; semid <= tmax_semid; semid++) {
		ercd = bit_semaphore(semid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  イベントフラグ毎の整合性検査
	 */
	for (flgid = TMIN_FLGID; flgid <= tmax_flgid; flgid++) {
		ercd = bit_eventflag(flgid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  データキュー毎の整合性検査
	 */
	for (dtqid = TMIN_DTQID; dtqid <= tmax_dtqid; dtqid++) {
		ercd = bit_dataqueue(dtqid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	// ★未完成

	/*
	 *  ミューテックス毎の整合性検査
	 */
	for (mtxid = TMIN_MTXID; mtxid <= tmax_mtxid; mtxid++) {
		ercd = bit_mutex(mtxid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	// ★未完成

	/*
	 *  周期通知毎の整合性検査
	 */
	for (cycid = TMIN_CYCID; cycid <= tmax_cycid; cycid++) {
		ercd = bit_cyclic(cycid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}

	/*
	 *  アラーム通知毎の整合性検査
	 */
	for (almid = TMIN_ALMID; almid <= tmax_almid; almid++) {
		ercd = bit_alarm(almid);
		if (ercd != E_OK) {
			return(ercd);
		}
	}
	return(E_OK);
}
