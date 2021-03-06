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
 *		エラーチェック用マクロ
 */

#ifndef TOPPERS_CHECK_H
#define TOPPERS_CHECK_H

#include "kernel_impl.h"
#include "memory.h"

/*
 *  オブジェクトIDの範囲の判定
 */
#define VALID_DOMID(domid)	(TMIN_DOMID <= (domid) && (domid) <= tmax_domid)
#define VALID_SOMID(somid)	(TMIN_SOMID <= (somid) && (somid) <= tmax_somid)
#define VALID_TSKID(tskid)	(TMIN_TSKID <= (tskid) && (tskid) <= tmax_tskid)
#define VALID_SEMID(semid)	(TMIN_SEMID <= (semid) && (semid) <= tmax_semid)
#define VALID_FLGID(flgid)	(TMIN_FLGID <= (flgid) && (flgid) <= tmax_flgid)
#define VALID_DTQID(dtqid)	(TMIN_DTQID <= (dtqid) && (dtqid) <= tmax_dtqid)
#define VALID_PDQID(pdqid)	(TMIN_PDQID <= (pdqid) && (pdqid) <= tmax_pdqid)
#define VALID_MTXID(mtxid)	(TMIN_MTXID <= (mtxid) && (mtxid) <= tmax_mtxid)
#define VALID_MBFID(mbfid)	(TMIN_MBFID <= (mbfid) && (mbfid) <= tmax_mbfid)
#define VALID_MPFID(mpfid)	(TMIN_MPFID <= (mpfid) && (mpfid) <= tmax_mpfid)
#define VALID_CYCID(cycid)	(TMIN_CYCID <= (cycid) && (cycid) <= tmax_cycid)
#define VALID_ALMID(almid)	(TMIN_ALMID <= (almid) && (almid) <= tmax_almid)

/*
 *  優先度の範囲の判定
 */
#define VALID_TPRI(tpri)	(TMIN_TPRI <= (tpri) && (tpri) <= TMAX_TPRI)

/*
 *  相対時間の範囲の判定
 */
#define VALID_RELTIM(reltim)	((reltim) <= TMAX_RELTIM)

/*
 *  タイムアウト指定値の範囲の判定
 */
#define VALID_TMOUT(tmout)	((tmout) <= TMAX_RELTIM || (tmout) == TMO_FEVR)

/*
 *  オブジェクトアクセス権の判定
 */
#define VIOLATE_ACPTN(acptn) \
					(rundom != TACP_KERNEL && (rundom & (acptn)) == 0U)

/*
 *  呼出しコンテキストのチェック（E_CTX）
 */
#define CHECK_TSKCTX() do {									\
	if (sense_context()) {									\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  CPUロック状態のチェック（E_CTX）
 */
#define CHECK_UNL() do {									\
	if (sense_lock()) {										\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  呼出しコンテキストとCPUロック状態のチェック（E_CTX）
 */
#define CHECK_TSKCTX_UNL() do {								\
	if (sense_context() || sense_lock()) {					\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  ディスパッチ保留状態でないかのチェック（E_CTX）
 */
#define CHECK_DISPATCH() do {								\
	if (sense_context() || sense_lock() || !dspflg) {		\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  その他のコンテキストエラーのチェック（E_CTX）
 */
#define CHECK_CTX(exp) do {									\
	if (!(exp)) {											\
		ercd = E_CTX;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  不正ID番号のチェック（E_ID）
 */
#define CHECK_ID(exp) do {									\
	if (!(exp)) {											\
		ercd = E_ID;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  パラメータエラーのチェック（E_PAR）
 */
#define CHECK_PAR(exp) do {									\
	if (!(exp)) {											\
		ercd = E_PAR;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  サービスコール不正使用のチェック（E_ILUSE）
 */
#define CHECK_ILUSE(exp) do {								\
	if (!(exp)) {											\
		ercd = E_ILUSE;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  オブジェクト状態エラーのチェック（E_OBJ）
 */
#define CHECK_OBJ(exp) do {									\
	if (!(exp)) {											\
		ercd = E_OBJ;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  オブジェクトアクセス権のチェック（E_OACV）
 */
#define CHECK_ACPTN(acptn) do {								\
	if (VIOLATE_ACPTN(acptn)) { 							\
		ercd = E_OACV;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  カーネルドメインからの呼出しかのチェック（E_OACV）
 */
#define CHECK_ACPTN_KERNEL() do {							\
	if (rundom != TACP_KERNEL) {							\
		ercd = E_OACV;										\
		goto error_exit;									\
	}														\
} while (false)

/*
 *  メモリアクセス権のチェック（E_MACV）
 */
#define CHECK_MACV_WRITE(p_var, type) do {					\
	if (!KERNEL_PROBE_MEM_WRITE(p_var, type)) {				\
		ercd = E_MACV;										\
		goto error_exit;									\
	}														\
} while (false)

#define CHECK_MACV_READ(p_var, type) do {					\
	if (!KERNEL_PROBE_MEM_READ(p_var, type)) {				\
		ercd = E_MACV;										\
		goto error_exit;									\
	}														\
} while (false)

#define CHECK_MACV_BUF_WRITE(base, size) do {				\
	if (!KERNEL_PROBE_BUF_WRITE(base, size)) {				\
		ercd = E_MACV;										\
		goto error_exit;									\
	}														\
} while (false)

#define CHECK_MACV_BUF_READ(base, size) do {				\
	if (!KERNEL_PROBE_BUF_READ(base, size)) {				\
		ercd = E_MACV;										\
		goto error_exit;									\
	}														\
} while (false)

#endif /* TOPPERS_CHECK_H */
