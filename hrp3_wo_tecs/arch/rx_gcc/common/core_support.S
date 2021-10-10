/*
 *   TOPPERS/HRP Kernel
 *       Toyohashi Open Platform for Embedded Real-Time Systems/
 *       High Reliable system Profile Kernel
 *
 *   Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                               Toyohashi Univ. of Technology, JAPAN
 *   Copyright (C) 2006-2018 by Embedded and Real-Time Systems Laboratory
 *               Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *   上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *       スコード中に含まれていること．
 *   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *       の無保証規定を掲載すること．
 *   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *       と．
 *     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *         作権表示，この利用条件および下記の無保証規定を掲載すること．
 *     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *         報告すること．
 *   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *       免責すること．
 * 
 *   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *   の責任を負わない．
 * 
 */

/*
 * コア依存モジュール アセンブリ言語部（RX用）
 *
 * References:
 * [1] RX Family RXv2 Instruction Set Architecture
 * [2] CC-RX User's Manual
 */

/*
 * 構造体アクセスのオフセット定義
 */
#include "offset.h"
#define sp r0

/*
 * 各種EQU定義(Hファイルの#define定義)
 */
	.global	__kernel_dominib_kernel
	.global	__kernel_exit_kernel
	.global	__kernel_ext_tsk
	.global	__kernel_intnest
	.global	__kernel_istkpt
	.global	__kernel_max_domain_mpu_num
	.global	__kernel_p_ctxdom
	.global	__kernel_p_runtsk
	.global	__kernel_p_schedtsk
	.global	__kernel_rundom
	.global	__kernel_svc_table
	.global	__kernel_svcinib_table
	.global	__kernel_tcb_table
	.global	__kernel_tmax_fncd
	.global	__kernel_twdtimer_enable
	.global	__kernel_twdtimer_start
	.global	__kernel_twdtimer_stop
	.global	__kernel_twdtimer_control
#if	TIPM_LOCK != TIPM_LOCK_ALL
	.global	__kernel_lock_flag
	.global	__kernel_saved_ipl
#endif

	.global	__kernel_dispatch
	.global	__kernel_exit_and_dispatch
	.global	__kernel_start_stask_r
	.global	__kernel_start_utask_r
	.global	_kernel_interrupt
	.global	_kernel_exception
	.global	_kernel_service_call
	.global	__kernel_call_exit_kernel
	.global	__kernel_start_dispatch

#if SUPPORT_ISOTEE == 1
	.global	_isotee_guest_running
	.global	_isotee_guest_tcb
	.global	_isotee_guest_itfp
#endif

/*
 * TCB_sp への書込みマクロ
 */
.macro	st_TCB_sp	src, tcb, tmp
#if (TCB_sp % 4) == 0
	mov.l	\src, TCB_sp[\tcb]
#else
	add		#TCB_sp, \tcb, \tmp
	mov.l	\src, [\tmp]
#endif
.endm

/*
 * TCB_sp からの読出しマクロ
 */
.macro	ld_TCB_sp	dst, tcb, tmp
#if (TCB_sp % 4) == 0
	mov.l	TCB_sp[\tcb], \dst
#else
	add		#TCB_sp, \tcb, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

/*
 * TCB_pc への書込みマクロ
 */
.macro	st_TCB_pc	src, tcb, tmp
#if (TCB_pc % 4) == 0
	mov.l	\src, TCB_pc[\tcb]
#else
	add		#TCB_pc, \tcb, \tmp
	mov.l	\src, [\tmp]
#endif
.endm

/*
 * TCB_pc からの読出しマクロ
 */
.macro	ld_TCB_pc	dst, tcb, tmp
#if (TCB_pc % 4) == 0
	mov.l	TCB_pc[\tcb], \dst
#else
	add		#TCB_pc, \tcb, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

/*
 * TCB_p_tinib からの読出しマクロ
 */
.macro	ld_TCB_p_tinib	dst, tcb, tmp
#if (TCB_p_tinib % 4) == 0
	mov.l	TCB_p_tinib[\tcb], \dst
#else
	add		#TCB_p_tinib, \tcb, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

/*
 * TINIB_exinf からの読出しマクロ
 */
.macro	ld_TINIB_exinf	dst, tinib, tmp
#if (TINIB_exinf % 4) == 0
	mov.l	TINIB_exinf[\tinib], \dst
#else
	add		#TINIB_exinf, \tinib, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

/*
 * TINIB_task からの読出しマクロ
 */
.macro	ld_TINIB_task	dst, tinib, tmp
#if (TINIB_task % 4) == 0
	mov.l	TINIB_task[\tinib], \dst
#else
	add		#TINIB_task, \tinib, \tmp
	mov.l	[\tmp], \dst
#endif
.endm

	.text

/*
 * APIからのタスクディスパッチャ入口
 *
 * 呼び出し条件:
 * ・ディスパッチャモード(ファイルヘッダ参照)
 *
 * ここでは, コンテキストの退避と, 実行再開番地の設定をする.
 */
__kernel_dispatch:
	mov.l	#__kernel_rundom, r4		/* rundomを取得 */
	mov.l	[r4], r4
	mvfc	usp, r5						/* uspを取得 */
	pushm	r4-r13						/* rundom、uspと非スクラッチレジスタ保存 */
	mov.l	#__kernel_p_runtsk, r15
	mov.l	[r15], r1					/* p_runtsk → r1 */
	st_TCB_sp	r0, r1, r5				/* スタックポインタをTCBに保存 */
	st_TCB_pc	#dispatch_r, r1, r5		/* 実行再開番地をTCBに保存 */
#if SUPPORT_ISOTEE == 1
	mov.l	#_isotee_guest_running, r1
	btst	#0, [r1].b
	bz		dispatcher
	mov.l	#_isotee_guest_itfp, r2
	mov.l	#0, [r2]
#endif
	bra		dispatcher

/*
 * APIへのタスクディスパッチャ出口
 *
 *  呼び出し条件:
 * ・ディスパッチャモード(ファイルヘッダ参照)
 *
 * ここでは, タスク例外ハンドラ呼出しと, コンテキストの復帰をする.
 */
dispatch_r:
#if SUPPORT_ISOTEE == 1
	mvtipl	#(ISOTEE_GUEST_MAX_IPL >> 24)
#endif
	popm 	r4-r13							/* rundom、uspと非スクラッチレジスタ復帰 */
	mov.l	#__kernel_rundom, r3
	mov.l	r4, [r3]
	mvtc	r5, usp
	mov.l	#__kernel_twdtimer_enable, r1	/* twdtimer_enableがtrueの場合 */
	mov.l	[r1], r1
	cmp		#0, r1
	beq		dispatch_r_after_twd
	bsr		__kernel_twdtimer_control		/* タイムウィンドウタイマの切換え */
dispatch_r_after_twd:
	rts  	                        		/* dispatch 呼び出し元へ戻る. */

/*
 * ディスパッチャの動作開始
 */
__kernel_start_dispatch:
	/*
	 * このルーチンは，カーネル起動時に，非タスクコンテキストで，NMIを
	 * 除くすべての割込みを禁止した状態（全割込みロック状態と同等）で
	 * 呼び出される．
	 *
	 * dispatcher_0へ分岐する前に，タスクコンテキスト・CPUロック状態・
	 * 割込み優先度マスク全解除状態にし，使用するスタックを，IDが1のタ
	 * スクのスタック領域に切り換えなければならない．
	 */

	/*
	 * タスクコンテキストに切り換える．
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	#0, [r5]						/* 例外ネストカウントを0に */

	/*
	 * IDが1のタスクのシステムスタック領域に切り換える．
	 */
	mov.l	#__kernel_tcb_table, r4			/* tcb_table[0] → r4 */
	mov.l	TCB_p_tinib[r4], r3				/* tcb_table[0].p_tinib → r3 */
	mov.l	TINIB_sstk[r3], r1
	mov.l	TINIB_sstksz[r3], r2
	add		r1, r2, sp

	/*
	 * CPUロック状態にして，ディスパッチャ本体へ分岐する．
	 */
#if	TIPM_LOCK != TIPM_LOCK_ALL
	mvtc	#(IPL_LOCK | 00010000H), psw	/* 全割込みロック解除状態 */
											/* 割込み優先度マスク全解除でない状態 */
#endif
	bra		dispatcher_0

/*
 * 現在のコンテキストを捨ててディスパッチ
 */
__kernel_exit_and_dispatch:
	/*
	 * このルーチンは，タスクコンテキスト・CPUロック状態・割込み優先度
	 * マスク全解除状態・ディスパッチ許可状態で呼び出される．
	 */
#if	LOG_DSP_ENTER == 1
	mov.l	#__kernel_p_runtsk, r1	; p_runtsk → r1
	mov.l	[r1], r1
#endif
	/* ディスパッチャ本体（dispatcher）へ */

/*
 * ディスパッチャ本体
 */
dispatcher:
#if	LOG_DSP_ENTER == 1
	/*
	 *【この時点のレジスタ状態】
	 * r1：p_runtsk（タスク切換え前）
	 */
	bsr		_kernel_log_dsp_enter
#endif

dispatcher_0:
	/*
	 * このルーチンは，タスクコンテキスト・CPUロック状態・割込み優先度
	 * マスク全解除状態・ディスパッチ許可状態で呼び出される．実行再開番
	 * 地へもこの状態のまま分岐する．
	 */
	mov.l	#__kernel_p_schedtsk, r1	/* p_schedtsk → r6 → p_runtsk */
	mov.l	[r1], r6
	mov.l	#__kernel_p_runtsk, r2
	mov.l	r6, [r2]
#if SUPPORT_ISOTEE == 1
	mov.l	#_isotee_guest_tcb, r2
	mov.l	#_isotee_guest_running, r3
	cmp		[r2], r6
	sceq.b	[r3]
#endif
	cmp		#0, r6
	bz		dispatcher_1				/* p_runtskがNULLならdispatcher_1へ */
	ld_TCB_sp	sp, r6, r5				/* タスクスタックを復帰 */

	/*
	 * MMU/MPUの設定切換え
	 *
	 *【この時点のレジスタ状態】
	 * r6：p_runtsk（タスク切換え後）
	 */
	mov.l	TCB_p_dominib[r6], r1			/* p_runtsk->p_dominib → r1 */
	mov.l	#__kernel_dominib_kernel, r2	/* カーネルドメインであれば */
	cmp		r1, r2							/* 			 dispatcher_3へ */
	beq		dispatcher_3
	mov.l	#__kernel_p_ctxdom, r2			/* p_dominibとp_ctxdomが一致して */
	mov.l	[r2], r3						/* 		  いれば，dispatcher_set_task_mpuへ */
	cmp		r3, r1
	beq		dispatcher_set_task_mpu
	mov.l	r1, [r2]						/* p_dominib → p_ctxdom */

	/*
	 * ドメイン用MPUの切換え処理
	 */
	mov.l	#0, r3
	mov.b	DOMINIB_use_mpu_num[r1], r4		/* use_mpu_num → r4 */
	mov.l	DOMINIB_mpu_area_info[r1], r5	/* mpu_area_info → r5 */
	mov.l	#MPU_RSPAGE1_ADDR, r1
	mov.l	#MPU_REPAGE1_ADDR, r2
dispatcher_set_domain_mpu_loop:
	cmp		r3, r4
	beq		dispatcher_disable_unused_mpu
	mov.l	[r5], [r1]
	mov.l	4[r5], [r2]
	add		#8, r1
	add		#8, r2
	add		#1, r3
	add		#8, r5
	bra		dispatcher_set_domain_mpu_loop

dispatcher_disable_unused_mpu:
	/* r2: REPAGE */
	/* r4: use_mpu_num */
	mov.l	#__kernel_max_domain_mpu_num, r3
	mov.b	[r3], r3
	mov.l	#0, r1
dispatcher_disable_unused_mpu_loop:
	cmp		r3, r4
	beq		dispatcher_set_task_mpu
	mov.l	r1, [r2]
	add		#8, r2
	add		#1, r4
	bra		dispatcher_disable_unused_mpu_loop

dispatcher_set_task_mpu:
	/*
	 * ユーザスタック用MPUを設定
	 */
	mov.l	TCB_p_tinib[r6], r1		/* p_runtsk->p_tinib → r1 */
	mov.l	TINIB_ustk[r1], r2		/* p_tinib->ustk → r2 */
	mov.l	#MPU_RSPAGE0_ADDR, r3
	mov.l	r2, [r3]
	mov.l	TINIB_ustksz[r1], r3	/* p_tinib->ustksz → r3 */
	add		r3, r2					/* ユーザスタックの初期値 → r2 */
	sub		#3, r2					/* r2 - 16 + 0xd */
	mov.l	#MPU_REPAGE0_ADDR, r3
	mov.l	r2, [r3]

dispatcher_3:
#if	LOG_DSP_LEAVE == 1
	mov.l	r6, r1					/* p_runtskをパラメータに渡す */
	bsr		_kernel_log_dsp_leave
#endif
	ld_TCB_pc	r1, r6, r5			/* 実行再開番地を復帰 */
	jmp		r1						/* p_runtskをr6に入れた状態で分岐する */

	/*
	 * アイドル処理
	 *
	 * 割込みをすべて許可し，CPUロック解除状態にして割込みを待つ．
	 */
dispatcher_1:
	mov.l	#__kernel_twdtimer_enable, r1	/* twdtimer_enableがtrueの場合 */
	mov.l	[r1], r1
	cmp		#0, r1
	beq		dispatcher_1_after_twd
	bsr		__kernel_twdtimer_start			/* タイムウィンドウタイマの動作開始 */
dispatcher_1_after_twd:

#if	TIPM_LOCK != TIPM_LOCK_ALL
	mov.l	#__kernel_lock_flag, r5	/* CPUロック解除状態へ */
	mov.l	#0, [r5]
	mvtc	#0, psw					/* 優先度0の割込み処理中を偽装 */
#endif

#if SUPPORT_ISOTEE == 1
	mvtipl	#(ISOTEE_GUEST_MAX_IPL >> 24)
#endif

dispatcher_2:
	setpsw	i				/* 全割込み許可 */
	clrpsw	i				/* 全割込み禁止 */
	bra   	dispatcher_2	/* 割込み待ち */

/*
 * カーネルの終了処理の呼出し
 *
 * 割込みロック状態にし，使用するスタックを非タスクコンテキスト用のスタッ
 * ク領域に切り替え，exit_kernelを呼び出す．
 */
__kernel_call_exit_kernel:
#if	TIPM_LOCK != TIPM_LOCK_ALL
	clrpsw	i							/* 全割込み禁止 */
	mov.l	#__kernel_lock_flag, r5		/* CPUロック解除状態へ */
	mov.l	#0, [r5]
#endif
	mov.l	#__kernel_istkpt, r5
	mov.l	[r5], sp					/* 割込み用のスタックへ切替え */
	mov.l	#__kernel_intnest, r5
	mov.w	#1, [r5]					/* 非タスクコンテキストに切換え */
	bsr		__kernel_exit_kernel

/*
 * システムタスクの実行開始時処理
 */
__kernel_start_stask_r:
	/*
	 *【この時点のレジスタ状態】
	 * r6：p_runtsk（タスク切換え後）
	 */
	mov.l	#__kernel_rundom, r1
	mov.l	#0, [r1]						/* TACP_KERNEL → rundom */
	mov.l	#__kernel_twdtimer_enable, r1	/* twdtimer_enableがtrueの場合 */
	mov.l	[r1], r1
	cmp		#0, r1
	beq		start_stask_r_after_twd
	bsr		__kernel_twdtimer_stop			/* タイムウィンドウタイマの停止 */
start_stask_r_after_twd:
#if SUPPORT_ISOTEE == 1
	mvtipl	#(ISOTEE_GUEST_MAX_IPL >> 24)
#endif
#if	TIPM_LOCK == TIPM_LOCK_ALL
	setpsw	i								/* 割込み許可(CPUロック解除状態) */
#else
	mov.l	#__kernel_lock_flag, r4			/* CPUロック解除状態へ                     */
	mov.l	#0, [r4]						/* ここに来るときは必ず saved_ipl の       */
	mvtc	#00010000H, psw					/* 値は 0 のため, 直値を設定する.          */
											/* 全割込みロック解除状態( I = 1 )         */
											/* 割込み優先度マスク全解除状態( IPL = 0 ) */
#endif
	mov.l 	#__kernel_ext_tsk, r5			/* タスク本体からの戻り番地を設定（スタックに積む） */
	push.l	r5
	ld_TCB_p_tinib	r2, r6, r4				/* p_runtsk->p_tinib → r2 */
	ld_TINIB_exinf	r1, r2, r4				/* exinfをパラメータに */
	ld_TINIB_task	r5, r2, r4				/* タスク起動番地にジャンプ */
	jmp		r5

/*
 * ユーザタスクの実行開始時処理
 */
__kernel_start_utask_r:
	/*
	 *【この時点のレジスタ状態】
	 * r6：p_runtsk（タスク切換え後）
	 */
	mov.l	TCB_p_dominib[r6], r1			/* p_runtsk->p_dominib → r1 */
	mov.l	DOMINIB_domptn[r1], r2			/* p_dominib->domptn → r2 */
	mov.l	#__kernel_rundom, r1
	mov.l	r2, [r1]						/* r2 → rundom */
	mov.l	#__kernel_twdtimer_enable, r1	/* twdtimer_enableがtrueの場合 */
	mov.l	[r1], r1
	cmp		#0, r1
	beq		start_utask_r_after_twd
	bsr		__kernel_twdtimer_start			/* タイムウィンドウタイマの動作開始 */
start_utask_r_after_twd:
	mov.l	TCB_p_tinib[r6], r1				/* p_runtsk->p_tinib → r1 */
	mov.l	TINIB_ustk[r1], r2				/* p_tinib->ustk → r2 */
	mov.l	TINIB_ustksz[r1], r3			/* p_tinib->ustksz → r3 */
	add		r3, r2							/* ユーザスタックの初期値 → r2 */
	sub		#4, r2
	mov.l	#call_ext_tsk, [r2]
	mvtc	r2, usp							/* 戻り番地 をユーザスタックに積む */

	mov.l	TINIB_task[r1], r2				/* タスク起動番地 → r2 */
	mov.l	#00130000H, r3					/* PSWの初期値（U、I、PM） → r3 */
#if SUPPORT_ISOTEE == 1
	or		#ISOTEE_GUEST_MAX_IPL, r3
#endif
	pushm	r2-r3							/* r3、r2の順でスタックに積む */

#if	TIPM_LOCK != TIPM_LOCK_ALL
	mov.l	#__kernel_lock_flag, r4
	mov.l	#0, [r4]
#endif

	mov.l	TINIB_exinf[r1], r1				/* exinfをパラメータに */
	rte

/*
 * 割込みの出入口処理(アセンブリ言語記述部分)
 *
 * 呼出し条件:
 * ・割込み発生時のH/W処理により, PSWレジスタのIビット=0, IPLは受付け
 *   た割込みのIPL.
 * ・スタックは多重割り込みなら割込みスタック, そうでなければ
 *   タスクスタック
 * ・割込み発生時のH/W処理により,スタックに割込みからの復帰PCとPSWが
 *   保存されている.
 * ・ベクタテーブルに登録された個別の入り口処理により, スタックに
 *   スクラッチレジスタ(R1-R5)とR6-R7が保存されている.
 *
 * 引数:
 * ・r6:割込み要因番号
 * ・r7:割込みハンドラのアドレス
 *
 * レジスタがスタック上にどのように保存されているかを以下に示す.
 * この図では上が低位, 下が高位のアドレスで, スタックは下から
 * 上方向に向かって積み上げられるものとする.
 *
 *  -------------------------
 *  |       ACC-REGS        |
 *  -------------------------(SP + 0)
 *  |       FPSW(4byte)     |
 *  -------------------------(SP + ACC-SIZE)
 *  |       R14(4byte)      |
 *  -------------------------(SP + ACC-SIZE + 4)
 *  |       R15(4byte)      |
 *  -------------------------(SP + ACC-SIZE + 8)
 *  |       R1(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 12)
 *  |       R2(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 16)
 *  |       R3(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 20)
 *  |       R4(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 24)
 *  |       R5(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 28)
 *  |       R6(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 32)
 *  |       R7(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 36)
 *  |       PC(4byte)       |
 *  -------------------------(SP + ACC-SIZE + 40)
 *  |       PSW(4byte)      |
 *  -------------------------(SP + ACC-SIZE + 44)
 *
 * ここでは, 割込み処理モードに変更してハンドラを実行する.
 *
 * 多重割込みかどうかは割込みネストカウンタの値で判定する.
 * intnest != 0 ならば多重割込みであると判定する.
 *
 */

/*
 * ret_int先頭でスタックに積まれているPSWレジスタへのオフセット
 * ACC-REGS + FPSW + R14--R15 + R1--R7 + PC
 */

	.equ	RET_INT_GET_PSW_OFFSET,	(RX_ACC_REGS_SIZE+4+36+4)

_kernel_interrupt:
	pushm	r14-r15						/* スクラッチレジスタを退避 */
	pushc	fpsw						/* FPUステータスレジスタ退避 */
#if __RXv1__ == 1
	mvfacmi	r5
	shll	#16, r5						/* ACC最下位16bitは0とする */
	mvfachi	r4
	pushm	r4-r5						/* アキュムレータ退避 */
#elif __RXv2__ == 1
	mvfaclo	#0, A1, r5
	mvfachi	#0, A1, r4
	mvfacgu	#0, A1, r3
	pushm	r3-r5						/* ACC1退避 */
	mvfaclo	#0, A0, r5
	mvfachi	#0, A0, r4
	mvfacgu	#0, A0, r3
	pushm	r3-r5						/* ACC0退避 */
#else
	#error "Unsupported ISA"
#endif

	/*
	 * 例外ネストカウントをインクリメントする．割込みが非タスクコンテキ
	 * ストで発生した場合には，irq_handler_1へ分岐する．
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	add		#1, r4						/* ネスト回数をインクリメント */
	mov.w	r4, [r5]
	cmp		#1, r4						/* 割込みが非タスクコンテキストで発生 */
	bnz		_kernel_interrupt_1			/* 			  なら_kernel_interrupt_1に分岐 */

	mov.l	#__kernel_twdtimer_enable, r3	/* twdtimer_enableがtrueの場合 */
	mov.l	[r3], r3
	cmp		#0, r3
	beq		_kernel_interrupt_after_twd
	bsr		__kernel_twdtimer_stop			/* タイムウィンドウタイマの停止 */
_kernel_interrupt_after_twd:

#if SUPPORT_ISOTEE == 1
	mov.l	#_isotee_guest_running, r1
	btst	#0, [r1].b
	bz		interrupt_after_isotee
	mov.l	#_isotee_guest_itfp, r3
	add		#(RET_INT_GET_PSW_OFFSET-4), sp, r4
	mov.l	r4, [r3]
interrupt_after_isotee:
#endif

	/*
	 * rundomをスタックに保存し，TACP_KERNELにする．
	 */
	mov.l	#__kernel_rundom, r3
	mov.l	[r3], r4
	pushm	r3-r4						/* rundomの番地と値をスタックに保存 */
	mov.l	#0, [r3]					/* TACP_KERNEL → rundom */

	/*
	 * 非タスクコンテキスト用のスタックに切り換える．
	 */
	mov.l	sp, r3						/* スタックポインタを取り出し */
	mov.l	#__kernel_istkpt, r5		/* 割込み用のスタックへ切替える */
	mov.l	[r5], sp
	push.l	r3							/* タスクスタックを保持 */

_kernel_interrupt_1:
	setpsw	i							/* CPUロック解除状態にする（割込み許可） */

#if	LOG_INH_ENTER == 1
	/*
	 * ログ出力の呼出し
	 */
	mov.l	r6, r1						/* 割込み番号をパラメータに渡す */
	bsr		_kernel_log_inh_enter
#endif

	jsr  	r7							/* 割込みハンドラの呼出し */

#if	LOG_INH_LEAVE == 1
	/*
	 * ログ出力の呼出し
	 */
	mov.l	r6, r1						/* 割込み番号をパラメータに渡す */
	bsr		_kernel_log_inh_leave
#endif

	/*
	 * カーネル管理の割込みを禁止する．
	 */
	clrpsw	i							/* NOTE: ここで割込み禁止(CPUロック状態)に、カーネル管理外割込みは後で許可する */

	/*
	 * 例外ネストカウントをデクリメントする．
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	sub		#1, r4						/* ネスト回数をデクリメント */
	mov.w	r4, [r5]
	cmp		#0, r4						/* 戻り先が非タスクコンテキストなら */
	bnz		_kernel_interrupt_5			/* 				_kernel_interrupt_5に分岐 */

	/*
	 * タスク用のスタックに戻す．
	 */
	pop		sp

	/*
	 * p_runtskがNULLか判定する．
	 */
	mov.l	#__kernel_p_runtsk, r1	/* p_runtsk → r1 */
	mov.l	[r1], r1
	cmp		#0, r1					/* p_runtskがNULLでなければ */
	bne		_kernel_interrupt_3		/* 		_kernel_interrupt_3に分岐 */

	/*
	 * タスクのスタックに保存したスクラッチレジスタ等を捨てる．
	 */
	add		#(8+RET_INT_GET_PSW_OFFSET), sp	/* rundomの番地と値、スクラッチレジスタ（PSWを除いて）等を捨てる */

#if	TIPM_LOCK == TIPM_LOCK_ALL
	pop		r5							/* 割込み/CPU例外発生前のIPL値取得 */
	and		#PSW_IPL_MASK, r5
	mvtc	r5, psw						/* 全割込みロック(CPUロック)状態 */
										/* 割込み/CPU例外発生前の割込み優先度 */
#else
	mov.l	#__kernel_lock_flag, r5		/* CPUロック状態へ */
	mov.l	#1, [r5]
	pop		r5							/* 割込み/CPU例外発生前のIPL値取得 */
	and		#PSW_IPL_MASK, r5
	mov.l	#__kernel_saved_ipl, r4
	mov.l	r5, [r4]
	mvtc	#(IPL_LOCK | PSW_I_MASK), psw	/* 全割込みロック解除状態 */
										/* 割込み優先度マスク全解除でない状態 */
#endif
	bra		dispatcher_0				/* dispatcher_0に分岐する */

	/*
	 * ディスパッチが必要か判定する．
	 */
_kernel_interrupt_3:
	/*
	 *【この時点のレジスタ状態】
	 * r1：p_runtsk
	 */
	mov.l	#__kernel_p_schedtsk, r4	/* p_schedtsk → r4 */
	mov.l	[r4], r4
	cmp		r1, r4						/* p_runtskとp_schedtskが同じなら */
	beq		_kernel_interrupt_4			/* 				  _kernel_interrupt_4へ */

#if	TIPM_LOCK == TIPM_LOCK_ALL
	mov.l	(8+RET_INT_GET_PSW_OFFSET)[r0], r5	/* 割込み/CPU例外発生前のIPL値取得 */
	and		#PSW_IPL_MASK, r5
	mvtc	r5, psw							/* 全割込みロック(CPUロック)状態 */
											/* 割込み/CPU例外発生前の割込み優先度 */
#else
	mov.l	#__kernel_lock_flag, r5			/* CPUロック状態へ */
	mov.l	#1, [r5]
	mov.l	(8+RET_INT_GET_PSW_OFFSET)[r0], r5	/* 割込み/CPU例外発生前のIPL値取得 */
	and		#PSW_IPL_MASK, r5
	mov.l	#__kernel_saved_ipl, r4
	mov.l	r5, [r4]
	mvtc	#(IPL_LOCK | PSW_I_MASK), psw	/* 全割込みロック解除状態 */
											/* 割込み優先度マスク全解除でない状態 */
#endif

	/*
	 * コンテキストを保存する．
	 */
	mvfc	usp, r7						/* uspを取得 */
	pushm	r7-r13						/* 残りのレジスタの保存（uspとr8-r13） */
	st_TCB_sp	sp, r1, r5				/* スタックポインタを保存 */
	st_TCB_pc	#ret_int_r, r1, r5		/* 実行再開番地を保存 */
	bra		dispatcher					/* r1にはp_runtskが格納されている */

ret_int_r:
	/*
	 * コンテキストを復帰する．
	 *
	 *【この時点のレジスタ状態】
	 *  r6：p_runtsk（タスク切換え後）
	 */
	popm 	r7-r13						/* 残りのレジスタの復帰（uspとr8-r13） */
	mvtc	r7, usp
_kernel_interrupt_4:
	/*
	 * rundomをスタックから復帰する．
	 */
	popm	r3-r4							/* rundomの番地と値をスタックから復帰 */
	mov.l	r4, [r3]

	mov.l	#__kernel_twdtimer_enable, r3	/* twdtimer_enableがtrueの場合 */
	mov.l	[r3], r3
	cmp		#0, r3
	beq		_kernel_interrupt_5
	bsr		__kernel_twdtimer_control		/* タイムウィンドウタイマの切換え */

	/*
	 * 割込み処理からのリターン
	 */
_kernel_interrupt_5:
#if __RXv1__ == 1
	popm	r4-r5						/* アキュムレータ復帰 */
	mvtaclo	r5							/* ACC最下位16bitは0で復帰 */
	mvtachi	r4
#elif __RXv2__ == 1
	popm	r3-r5						/* ACC0復帰 */
	mvtaclo	r5, A0
	mvtachi	r4, A0
	mvtacgu	r3, A0
	popm	r3-r5						/* ACC1復帰 */
	mvtaclo	r5, A1
	mvtachi	r4, A1
	mvtacgu	r3, A1
#else
	#error "Unsupported ISA"
#endif
	popc	fpsw						/* FPUステータスレジスタ復帰 */
	popm	r14-r15						/* レジスタ復帰 */
	popm	r1-r7
	rte									/* 割込み前の処理に戻る */

/*
 * CPU例外の出入口処理(アセンブリ言語記述部分)
 *
 * 呼出し条件:
 * ・CPU例外発生時のH/W処理により, PSWレジスタのIビット=0, IPL=0.
 * ・スタックは多重割り込みなら割込みスタック, そうでなければ
 *   タスクスタック
 * ・CPU例外発生時のH/W処理により,スタックにCPU例外からの復帰PCと
 *   PSWが保存されている.
 * ・ベクタテーブルに登録された個別の入り口処理により, スタックに
 *   スクラッチレジスタ(R1-R5)とR6-R7が保存されている.
 *
 * 引数:
 * ・r6:CPU例外要因番号
 * ・r7:CPU例外ハンドラのアドレス
 *
 * レジスタがスタック上にどのように保存されているかを以下に示す.
 * この図では上が低位, 下が高位のアドレスで, スタックは下から
 * 上方向に向かって積み上げられるものとする.
 * なお, CPU例外要因番号、rundomとR8-R13はCPU例外ハンドラ内で情報を取得
 * する目的で退避しており, 出口処理では保存内容を破棄すればよい.
 *
 *  -------------------------
 *  |    CPU例外要因番号              | <----- p_excinf
 *  ------------------------- (intptr_t)(p_excinf + 0)
 *  |        rundom         |
 *  ------------------------- (intptr_t)(p_excinf + 4)
 *  |       R8(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + 8)
 *  |       R9(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + 12)
 *  |       R10(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + 16)
 *  |       R11(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + 20)
 *  |       R12(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + 24)
 *  |       R13(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + 28)
 *  |       ACC-REGS        |
 *  ------------------------- (intptr_t)(p_excinf + 32)
 *  |       FPSW(4byte)     |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 32)
 *  |       R14(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 36)
 *  |       R15(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 40)
 *  |       R1(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 44)
 *  |       R2(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 48)
 *  |       R3(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 52)
 *  |       R4(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 56)
 *  |       R5(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 60)
 *  |       R6(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 64)
 *  |       R7(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 68)
 *  |       PC(4byte)       |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 72)
 *  |       PSW(4byte)      |
 *  ------------------------- (intptr_t)(p_excinf + ACC-SIZE + 76)
 *
 * ここでは, 割込み処理モードに変更してハンドラを実行する.
 * CPU例外ハンドラに渡すVP型の変数 p_excinf としてISPの値渡す．
 *
 * 多重割込みかどうかは割込みネストカウンタの値で判定する.
 * intnest != 0 ならば多重割込みであると判定する.
 *
 * CPU例外入口処理
 * ここでは, 割込み処理モードに変更してハンドラを実行する.
 */

/*
 * CPU例外ハンドラ呼出し後に不要となるスタック情報のサイズ
 * EXCNO + RUNDOM + R8--R13
 */
	.equ	EXCINF_REG_SIZE,		(4+4+24)
	.equ	EXC_GET_RUNDOM_OFFSET,	(4)

_kernel_exception:
	/*
	 * スクラッチレジスタをスタックに保存する
	 */
	pushm	r14-r15						/* スクラッチレジスタを退避 */
	pushc	fpsw
#if __RXv1__ == 1
	mvfacmi	r5
	shll	#16, r5						/* ACC最下位16bitは0とする */
	mvfachi	r4
	pushm	r4-r5						/* アキュムレータ退避 */
#elif __RXv2__ == 1
	mvfaclo	#0, A1, r5
	mvfachi	#0, A1, r4
	mvfacgu	#0, A1, r3
	pushm	r3-r5						/* ACC1退避 */
	mvfaclo	#0, A0, r5
	mvfachi	#0, A0, r4
	mvfacgu	#0, A0, r3
	pushm	r3-r5						/* ACC0退避 */
#else
	#error "Unsupported ISA"
#endif

	/*
	 * CPU例外が発生した状況を判断するための追加情報をスタックに保存する
	 */
	pushm	r8-r13						/* 非スクラッチレジスタ保存 */
	mov.l	#__kernel_rundom, r2
	push.l	[r2]						/* rundomをスタックに保存 */
	push.l	r6							/* CPU例外要因番号を保持 */
	mov.l	sp, r6						/* 例外情報の先頭番地 → r6 */
	mov.l	#__kernel_intnest, r2		/* intnestの番地 → r2 */
	mov.w	[r2], r3					/* intnestの値 → r3 */

	/*
	 * カーネル管理外のCPU例外か判定する
	 */
	mov.l	EXC_GET_PSW_OFFSET[r6], r5
	and		#PSW_I_MASK, r5
	bz		exception_nonkernel			/* 全割込み禁止(CPUロック)状態なら管理外 */
#if	TIPM_LOCK != TIPM_LOCK_ALL
	mov.l	EXC_GET_PSW_OFFSET[r6], r5
	and		#PSW_IPL_MASK, r5
	cmp		#IPL_LOCK, r5
	bgt		exception_nonkernel			/* IPLがCPUロックレベル以上なら管理外 */
#endif

	/*
	 *【この時点のレジスタ状態】
	 * r2：intnestの番地
	 * r3：intnestの値
	 * r6：CPU例外の情報を記憶している領域の先頭番地
	 * r7：CPU例外ハンドラのアドレス
	 */

	/*
	 * 例外ネストカウントをインクリメントする．
	 */
	add		#1, r3
	mov.w	r3, [r2]
	cmp		#1, r3						/* CPU例外発生前が非タスクコンテキスト */
	bnz		_kernel_exception_2			/* 			   なら_kernel_exception_2に分岐 */

	mov.l	#__kernel_twdtimer_enable, r3	/* twdtimer_enableがtrueの場合 */
	mov.l	[r3], r3
	cmp		#0, r3
	beq		_kernel_exception_after_twd
	bsr		__kernel_twdtimer_stop			/* タイムウィンドウタイマの停止 */
_kernel_exception_after_twd:

	/*
	 * rundomをTACP_KERNELにする．
	 */
	mov.l	#__kernel_rundom, r1
	mov.l	#0, [r1]					/* TACP_KERNEL → rundom */


	/*
	 * 非タスクコンテキスト用のスタックに切り換える．
	 */
	mov.l	#__kernel_istkpt, r5		/* 非タスクコンテキスト用のスタックに */
	mov.l	[r5], sp
	push.l	r6							/* 切換え前のスタックポインタを保存 */

_kernel_exception_2:
	/*
	 *【この時点のレジスタ状態】
	 * r6：CPU例外の情報を記憶している領域の先頭番地
	 * r7：CPU例外ハンドラのアドレス
	 */

	mov.l	EXC_GET_PSW_OFFSET[r6], r5
	and		#PSW_IPL_MASK, r5			/* 割込み優先度マスクを，CPU例外発生時の値に設定する */
	mvtc	r5, psw						/* PSWを元のIPLかつCPUロック状態(I=0、U=0、PM=0)に設定 */
#if	TIPM_LOCK == TIPM_LOCK_ALL
	setpsw	i							/* 割込み許可(CPUロック解除状態) */
#else
	TODO: 「setpsw i」だけで十分みたいが、要検討
#endif

#if	LOG_EXC_ENTER == 1
	mov.l	[r6], r1					/* 引数の割込み要因番号  → r1 */
	bsr		_kernel_log_exc_enter		/* ログトレース関数の呼出し */
#endif

	mov.l	r6, r1						/* 引数のp_excinfを設定 */
	jsr  	r7							/* Cルーチン呼び出し */

#if	LOG_EXC_LEAVE == 1
	mov.l	[r6], r1					/* 引数の割込み要因番号  → r1 */
	bsr		_kernel_log_exc_leave		/* ログトレース関数の呼出し */
#endif

	clrpsw	i							/* ここからは必ず割込み禁止 */

	/*
	 * 例外ネストカウントをデクリメントする．
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	sub		#1, r4						/* ネスト回数をデクリメント */
	mov.w	r4, [r5]
	cmp		#0, r4						/* 戻り先が非タスクコンテキストなら */
	bnz		_kernel_exception_5			/* 				_kernel_exception_5に分岐 */

	/*
	 * タスク用のスタックに戻す．
	 */
	pop		sp

	/*
	 * p_runtskがNULLか判定する．
	 */
	mov.l	#__kernel_p_runtsk, r1	/* p_runtsk → r1 */
	mov.l	[r1], r1
	cmp		#0, r1					/* p_runtskがNULLでなければ */
	bnz		_kernel_exception_3		/* 		_kernel_exception_3に分岐 */

	/*
	 * タスクのスタックに保存したスクラッチレジスタ等を捨てる．
	 */
	add		#EXC_GET_PSW_OFFSET+4, sp
	bra		dispatcher_0

	/*
	 * ディスパッチが必要か判定する．
	 */
_kernel_exception_3:
	/*
	 *【この時点のレジスタ状態】
	 * r1：p_runtsk
	 */
	mov.l	#__kernel_p_schedtsk, r2	/* p_schedtsk → r2 */
	mov.l	[r2], r2
	cmp		r1, r2						/* p_runtskとp_schedtskが同じなら */
	beq		_kernel_exception_4			/* 				  _kernel_exception_4へ */


	/*
	 * コンテキストを保存する．
	 */
	mvfc	usp, r3						/* usp以外（i.e. r8-r13）は既に例外情報に保存してある */
	push.l	r3
	st_TCB_sp sp, r1, r5				/* スタックポインタを保存 */
	st_TCB_pc #ret_exc_r, r1, r5		/* 実行再開番地を保存 */
	bra		dispatcher					/* r1にはp_runtskが格納されている */

ret_exc_r:
	/*
	 * コンテキストを復帰する．
	 *
	 *【この時点のレジスタ状態】
	 * r6：p_runtsk（タスク切換え後）
	 */
	pop		r3
	mvtc	r3, usp

	/*
	 * r8-r13を例外情報から復帰
	 */
	mov.l	sp, r1
	add		#8, sp
	popm	r8-r13
	mov.l	r1, sp

_kernel_exception_4:
	/*
	 * 例外フレームからrundomを復帰する．
	 */
	mov.l	#__kernel_rundom, r1
	mov.l	EXC_GET_RUNDOM_OFFSET[sp], [r1]

	mov.l	#__kernel_twdtimer_enable, r3	/* twdtimer_enableがtrueの場合 */
	mov.l	[r3], r3
	cmp		#0, r3
	beq		_kernel_exception_4_after_twd
	bsr		__kernel_twdtimer_control		/* タイムウィンドウタイマの切換え */
_kernel_exception_4_after_twd:

	/*
	 * CPU例外処理からのリターン
	 */
_kernel_exception_5:
	add		#EXCINF_REG_SIZE, sp	/* スタック上の例外情報を捨てる */
	bra		_kernel_interrupt_5		/* スタック状態が同じであるため、割込み用リターンへ */

/*
 * カーネル管理外のCPU例外の出入口処理
 */
exception_nonkernel:
	/*
	 *【この時点のレジスタ状態】
	 * r2：intnestの番地
	 * r3：intnestの値
	 * r6：CPU例外の情報を記憶している領域の先頭番地
	 * r7：CPU例外ハンドラのアドレス
	 */

	/*
	 * 例外ネストカウントをインクリメントする．
	 */
	add		#1, r3
	mov.w	r3, [r2]
	cmp		#1, r3						/* CPU例外発生前が非タスクコンテキスト */
	bnz		exception_nonkernel_2		/* 			   ならexception_nonkernel_2に分岐 */

	/*
	 * rundomをTACP_KERNELにする．
	 */
	mov.l	#__kernel_rundom, r1
	mov.l	#0, [r1]					/* TACP_KERNEL → rundom */

	/*
	 * 非タスクコンテキスト用のスタックに切り換える．
	 */
	mov.l	#__kernel_istkpt, r5		/* 非タスクコンテキスト用のスタックに */
	mov.l	[r5], sp
	push.l	r6							/* 切換え前のスタックポインタを保存 */

exception_nonkernel_2:
	/*
	 * システム状態（コンテキストは除く）を，CPU例外発生時の状態へ
	 */
	mov.l	EXC_GET_PSW_OFFSET[r6], r5
	mvtc	r5, psw						/* CPU例外発生前の状態に戻す */

	/*
	 * CPU例外ハンドラの呼出し
	 */
	mov.l	r6, r1						/* 引数のp_excinfを設定 */
	jsr  	r7							/* Cルーチン呼び出し */


	clrpsw	i							/* ここからは必ず割込み禁止 */

	/*
	 * 例外ネストカウントをデクリメントする．
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r4
	sub		#1, r4						/* ネスト回数をデクリメント */
	mov.w	r4, [r5]
	cmp		#0, r4						/* 戻り先が非タスクコンテキストなら */
	bnz		_kernel_exception_5			/* 				_kernel_exception_5に分岐 */

	/*
	 * タスク用のスタックに戻す．
	 */
	pop		sp

	bra		_kernel_exception_5

/*
 * サービスコールの出入口処理
 *
 * 呼出し条件:
 * ・割込み発生時のH/W処理により,スタックに割込みからの復帰PCとPSWが
 *   保存されている.
 * ・r5は機能コード
 * ・r14は第5パラメータ
 */
_kernel_service_call:
	/*
	 * 機能コードの下限値チェック
	 */
	cmp		#TMIN_FNCD, r5
	blt		ret_svc_rsfn

	/*
	 * 機能コードが0より大きければ，拡張サービスコール処理へ
	 */
	cmp		#0, r5
	bgt		extsvc_entry

	/*
	 * ソフトウェア割込み前のシステム状態に戻す
	 */
	mov.l	4[sp], r15				/* 元のPSWをr15に */
	tst		#PSW_I_MASK, r15
	beq		service_call_1
	setpsw	i

service_call_1:
	/*
	 * サービスコールを呼び出す
	 */
	push.l	r14						/* 第5パラメータをスタックに積む */
	mov.l	#__kernel_svc_table, r14
	abs		r5
	rotl	#2, r5
	add		r5, r14
	mov.l	[r14], r14				/* サービスコールの番地を算出 */
	jsr		r14						/* サービスコールへ分岐 */

	/*
	 * サービスコールからのリターン処理
	 */
service_call_return:
	add		#4, sp					/* 第5パラメータを捨てる */

	/*
	 * ソフトウェア割込みからのリターン後のシステム状態が，この時点での
	 * システム状態になるように準備する
	 */
	mvfc	psw, r14				; psw → r14
	and		#(PSW_I_MASK | PSW_IPL_MASK), r14
	mov.l	4[sp], r15
	and		#~(PSW_I_MASK | PSW_IPL_MASK), r15
	or		r14, r15
	mov.l	r15, 4[sp]

	/*
	 * サービスコールからのリターン
	 */
ret_svc_1:
	rte

ret_svc_rsfn:
	mov.l	#E_RSFN, r1
	rte

ret_extsvc_rsfn:
	add		#8, sp			/* 第5パラメータ等を捨てる */
#if	LOG_EXTSVC_ENTER + LOG_EXTSVC_LEAVE > 0
	pop.l	r6				/* r6を復帰 */
#endif
	mov.l	#E_RSFN, r1
	rte

ret_extsvc_nomem:
	add		#8, sp			/* 第5パラメータ等を捨てる */
#if	LOG_EXTSVC_ENTER + LOG_EXTSVC_LEAVE > 0
	pop.l	r6				/* r6を復帰 */
#endif
	mov.l	#E_NOMEM, r1
	rte

ret_extsvc_sys:
	add		#8, sp			/* 第5パラメータ等を捨てる */
#if	LOG_EXTSVC_ENTER + LOG_EXTSVC_LEAVE > 0
	pop.l	r6				/* r6を復帰 */
#endif
	mov.l	#E_SYS, r1
	rte

	/*
	 * 拡張サービスコールの出入口処理
	 *
	 * 呼び出し条件：
	 * ・r5は機能コード、r14は第5パラメータ
	 */
extsvc_entry:
	/*
	 * 第5パラメータをスタックに積む
	 *
	 * 第5パラメータを早めに積んで，使用できるレジスタを確保する．r15
	 * をスタックに積むのは，第6パラメータ（cdmid）を置く領域を確保す
	 * るためであり，積んでいる値には意味がない．
	 */
#if	LOG_EXTSVC_ENTER + LOG_EXTSVC_LEAVE > 0
	push.l	r6						/* r6をスタックに保存 */
	mov.l	r5, r6					/* 機能コード → r6 */
#endif
	pushm	r14-r15					/* r15はスペース確保のため */

	/*
	 * 機能コードの上限値チェック
	 */
	mov.l	#__kernel_tmax_fncd, r14
	mov.l	[r14], r14
	cmp		r14, r5
	bgt		ret_extsvc_rsfn

	/*
	 * 拡張サービスコールルーチンの先頭番地を取得．NULLでないかチェック
	 */
	mov.l	#__kernel_svcinib_table-8, r14
	rotl	#3, r5
	add		r5, r14						/* p_svcinibを取得 */
	mov.l	SVCINIB_extsvc[r14], r15	/* p_svcinib->extsvcを取得 */
	cmp		#0, r15						/* p_svcinib->extsvcがNULLか？ */
	beq		ret_extsvc_rsfn

	/*
	 * 非タスクコンテキストからの呼出しなら，extsvc_entry_1へ分岐
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r5
	cmp		#0, r5
	mov.l	#TDOM_KERNEL, r5
	bnz		extsvc_entry_1

	/*
	 * システムスタックの残量チェック［NGKI3197］
	 */
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r5
	mov.l	TCB_p_tinib[r5], r5
	mov.l	TINIB_sstk[r5], r5		/* スタックの先頭番地 → r5 */
	sub		r5, sp, r5				/* スタック残量 → r5 */
	blt		ret_extsvc_nomem		/* スタック残量 < 0 ならエラー */
	mov.l	SVCINIB_stksz[r14], r14	/* p_svcinib->stksz → r14 */
	cmp		r14, r5					/* スタックの残量 < r14 ならエラー */
	blt		ret_extsvc_nomem

	/*
	 * svclevelのチェックとインクリメント［NGKI3198］
	 */
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r5
	mov.b	TCB_svclevel[r5], r14
	cmp		#255, r14				/* svclevel == UINT8_MAX ならエラー */
	beq		ret_extsvc_sys
	add		#1, r14					/* svclevelをインクリメント */
	mov.b	r14, TCB_svclevel[r5]

	/*
	 * cdmidを求め，rundomを更新する
	 */
	mov.l	#__kernel_rundom, r14
	mov.l	[r14], r5					/* rundom → r5 */
	cmp		#0, r5
	mov.l	#TDOM_KERNEL, r5			/* rundomがTACP_KERNELであれば */
	beq		extsvc_entry_1				/*			  extsvc_entry_1へ */
	mov.l	#0, [r14]					/* rundomをTACP_KERNELに設定 */
	mov.l	#__kernel_p_runtsk, r5		/* ドメインID → r5 */
	mov.l	[r5], r5
	mov.l	TCB_p_tinib[r5], r5
	mov.l	TINIB_domid[r5], r5

extsvc_entry_1:
	/*
	 * ソフトウェア割込み前のシステム状態に戻す
	 *
	 * 【この時点のレジスタ状態】
	 * r15:拡張サービスコールルーチンの先頭番地
	 * r5 :cdmid（呼出し元の保護ドメインID）
	 */
	mov.l	12[sp], r14			/* スタックに保存した元のPSWを取得 */
	tst		#PSW_I_MASK, r14
	beq		extsvc_entry_2
	setpsw	i
extsvc_entry_2:
	mov.l	r5, 4[sp]			/* cdmidをスタック上に置く（r15の場所） */

#if	LOG_EXTSVC_ENTER == 1
	mov.l	[sp], r14				/* 第5パラメータを取得 */
	push.l	r15
	pushm	r1-r4
	push.l	r5						/* cdmidをスタックに積む */
	push.l	r14						/* 第5パラメータをスタックに積む */
	push.l	r4						/* 第4パラメータをスタックに積む */
	mov.l	r3, r4					/* 第3パラメータ → r4 */
	mov.l	r2, r3					/* 第2パラメータ → r3 */
	mov.l	r1, r2					/* 第1パラメータ → r2 */
	mov.l	r6, r1					/* 機能コード → r1 */
	bsr		_kernel_log_extsvc_enter
	add		#12, sp					/* 3つのパラメータを捨てる */
	popm	r1-r4
	pop.l	r15
#endif

	/*
	 * 拡張サービスコールを呼び出す
	 */
	jsr		r15


#if	LOG_EXTSVC_LEAVE == 1
	push.l	r1
	mov.l	r1, r2					/* 拡張サービスコールの返り値 → r2 */
	mov.l	r6, r1					/* 機能コード → r1 */
	bsr		_kernel_log_extsvc_leave
	pop.l	r1
#endif

ret_extsvc:
	add		#8, sp				/* 第5〜6パラメータを捨てる */
#if	LOG_EXTSVC_ENTER + LOG_EXTSVC_LEAVE > 0
	pop.l	r6					/* r6を復帰 */
#endif

	/*
	 * ソフトウェア割込みからのリターン後のシステム状態が，この時点での
	 * システム状態になるように準備する
	 */
	mvfc	psw, r14				/* psw → r14 */
	and		#(PSW_I_MASK | PSW_IPL_MASK), r14
	mov.l	4[sp], r15
	and		#~(PSW_I_MASK | PSW_IPL_MASK), r15
	or		r14, r15
	mov.l	r15, 4[sp]


	/*
	 *  カーネル管理の割込みを禁止する．
	 */
	clrpsw	i							/* NOTE: ここで割込み禁止(CPUロック状態)に、カーネル管理外割込みはリターン時許可する */

	/*
	 * 非タスクコンテキストからの呼出しなら，リターン
	 */
	mov.l	#__kernel_intnest, r5
	mov.w	[r5], r5
	cmp		#0, r5
	bnz		ret_svc_1

	/*
	 * svclevelのデクリメントし，0にならなければリターン
	 */
	mov.l	#__kernel_p_runtsk, r5
	mov.l	[r5], r5
	mov.b	TCB_svclevel[r5], r14
	sub		#1, r14					/* svclevelをデクリメント */
	mov.b	r14, TCB_svclevel[r5]
	bne		ret_svc_1

	/*
	 * rundomの設定
	 */
	mov.l	TCB_p_dominib[r5], r14
	mov.l	#__kernel_rundom, r5
	mov.l	DOMINIB_domptn[r14], [r5]
	bra		ret_svc_1

/*
 * タスクの終了処理
 */
	.section	P_SHARED, CODE
call_ext_tsk:
	mov.l	#TFN_EXT_TSK, r5
	int		#SVC_SERVICE_CALL
	mov.l	#TFN_EXT_KER, r5
	int		#SVC_SERVICE_CALL
call_ext_tsk_loop:
	bra		call_ext_tsk_loop

	.end
