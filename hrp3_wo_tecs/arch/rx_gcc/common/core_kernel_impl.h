/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2016 by Embedded and Real-Time Systems Laboratory
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
 *		kernel_impl.hのコア依存部（RX用）
 *
 *  このヘッダファイルは，target_kernel_impl.h（または，そこからインク
 *  ルードされるファイル）のみからインクルードされる．他のファイルから
 *  直接インクルードしてはならない．
 *
 *  References:
 *  [1] RX Family RXv2 Instruction Set
 *  [2] RX Family User's Manual: Software
 */

#ifndef TOPPERS_CORE_KERNEL_IMPL_H
#define TOPPERS_CORE_KERNEL_IMPL_H

#include "core_insn.h"

/*
 *  ターゲット依存のメモリオブジェクト属性（エラーチェック用）
 */
#define TARGET_MEMATR		(TA_KEEP)

/*
 *  エラーチェック方法の指定
 */
#define CHECK_USTKSZ_ALIGN	16	/* ユーザスタックサイズのアライン単位 */

/*
 *  システム周期オーバラン例外に使用するSVC番号
 */
#define SVC_SCYCOVR		(EXCNO_SCYCOVR)

/*
 *  トレースログに関する設定
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "arch/tracelog/trace_log.h"
#endif /* TOPPERS_ENABLE_TRACE */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  割込み発生回数を保存する変数
 */
extern uint16_t intnest;

/*
 *  CPUロック状態での割込み優先度マスク
 *
 *  TIPM_LOCKは，CPUロック状態での割込み優先度マスク，すなわち，カーネ
 *  ル管理外のものを除くすべての割込み要求をマスクする値に定義する．
 *
 *  TMIN_INTPRI変更することで管理外割込みの有無を決定する．
 *  例えばTMIN_INTPRIを-14に設定すると，レベル15の割込みがカーネル管理外と
 *  なる．TMIN_INTPRIを-15に設定すると，NMI以外にカーネル管理外の割込みを
 *  設けないことになる（この場合には-15に設定することを推奨する）．
 */
#ifndef TIPM_LOCK
#define TIPM_LOCK	TMIN_INTPRI
#endif /* TIPM_LOCK */
#ifndef TIPM_LOCK_ALL
#define TIPM_LOCK_ALL (-15)
#endif

/*
 *  割込み優先度マスクの外部表現と内部表現の変換
 *
 *  RXでは，プロセッサステータスワードレジスタ（PSW）の下から
 *  24～27ビット目の4ビットに割込み優先度レベル（IPL）が置かれている．
 *  カーネル管理外割込みを実装する場合にIPLを使用した制御を行う．
 *  カーネルは割込み優先度マスク（-1から連続した負の値）で管理されて
 *  いるためIPLとの変換が必要となる．
 */
#define IPL_TO_IPM( ipl )	(-(( PRI )(( ipl ) >> 24U )))		/* IPLをIPMに	*/
#define IPM_TO_IPL( ipm )	((( uint32_t )(-( ipm ))) << 24U )	/* IPMをIPLに	*/

/*
 *  CPUロック状態での割込み優先度マスクのIPL
 */
#define IPL_LOCK		( IPM_TO_IPL( TIPM_LOCK ) )

/*
 *  TIPM_ENAALL（割込み優先度マスク全解除）のIPL
 */
#define IPL_ENAALL		( IPM_TO_IPL( TIPM_ENAALL ) )

/*
 *  TOPPERS標準割込み処理モデルの実現
 */

/*
 *  コンテキストの参照
 *
 *  RXでは，割込みの戻り先がタスクかどうかを判断するために intnest
 *  を使用している．これを用いてコンテキストを判断する．
 */
Inline bool_t
sense_context( void )
{
	/*  ネストカウンタ0より大なら非タスクコンテキスト  */
	return ( intnest > 0U );
}

/*
 *  CPUロックフラグ実現のための変数
 *
 *  これらの変数は，CPUロック状態の時のみ書き換えてよいものとする．
 */
#if TIPM_LOCK != TIPM_LOCK_ALL
extern bool_t	lock_flag;		/* CPUロックフラグの値を保持する変数 */
extern uint32_t	saved_ipl;		/* 割込み優先度レベルを保存する変数 */
#endif	/* TIPM_LOCK != TIPM_LOCK_ALL */

/*
 *  CPUロック状態への移行
 *
 *  IPM（ハードウェアの割込み優先度マスク）を，saved_iipmに保存し，カー
 *  ネル管理外のものを除くすべての割込み要求をマスクする値（TIPM_LOCK）
 *  に設定する．また，lock_flagをTRUEにする．
 *
 *  IPMが，最初からTIPM_LOCKと同じかそれより高い場合には，それを
 *  saved_iipmに保存するのみで，TIPM_LOCKには設定しない．これは，モデル
 *  上の割込み優先度マスクが，TIPM_LOCKと同じかそれより高いレベルに設定
 *  されている状態にあたる．
 *
 *  この関数は，CPUロック状態（lock_flagがTRUEの状態）で呼ばれることは
 *  ないものと想定している．
 */
Inline void
lock_cpu( void )
{
#if TIPM_LOCK == TIPM_LOCK_ALL
	disint();
#else	/* TIPM_LOCK == TIPM_LOCK_ALL */
	uint32_t	ipl;

	/*
	 *  current_ipl()の返り値を直接saved_iplに保存せず，一時変数ipl
	 *  を用いているのは，current_ipl()を呼んだ直後に割込みが発生し，
	 *  起動された割込み処理でsaved_iplが変更される可能性があるためで
	 *  ある．
	 */
	ipl = current_ipl();
	if( IPL_LOCK > ipl ){
		set_ipl( IPL_LOCK );
	}

	saved_ipl = ipl;
	lock_flag = true;
#endif	/* TIPM_LOCK == TIPM_LOCK_ALL */
}

/*
 *  CPUロック状態への移行（ディスパッチできる状態）
 */
#define lock_cpu_dsp()		lock_cpu()

/*
 *  CPUロック状態の解除
 *
 *  lock_flagをFALSEにし，IPM（ハードウェアの割込み優先度マスク）を，
 *  saved_iipmに保存した値に戻す．
 *
 *  この関数は，CPUロック状態（lock_flagがtrueの状態）でのみ呼ばれるも
 *  のと想定している．
 */
Inline void
unlock_cpu( void )
{
#if TIPM_LOCK == TIPM_LOCK_ALL
	enaint();
#else	/* TIPM_LOCK == TIPM_LOCK_ALL	*/
	lock_flag = false;
	set_ipl( saved_ipl );
#endif	/* TIPM_LOCK == TIPM_LOCK_ALL	*/
}

/*
 *  CPUロック状態の解除（ディスパッチできる状態）
 */
#define unlock_cpu_dsp()	unlock_cpu()

/*
 *  CPUロック状態の参照
 */
Inline bool_t
sense_lock( void )
{
#if TIPM_LOCK == TIPM_LOCK_ALL
	return (( bool_t )(( current_psw() & PSW_I_MASK) == 0 ));
#else	/* TIPM_LOCK == TIPM_LOCK_ALL	*/
	return lock_flag;
#endif	/* TIPM_LOCK == TIPM_LOCK_ALL	*/
}

/*
 *  割込みを受け付けるための遅延処理
 */
Inline void
delay_for_interrupt(void)
{
}

/*
 *  保護ドメイン初期化コンテキストブロックを使う
 */
#define USE_DOMINICTXB

typedef struct {
	uint32_t rspage;
	uint32_t repage;
} MPU_INFO;

/*
 *  保護ドメイン初期化コンテキストブロックの定義
 */
typedef	struct domain_initialization_context_block {
	uint8_t			use_mpu_num;
	const MPU_INFO	*mpu_area_info;
} DOMINICTXB;

/*
 *  共有MPU領域数（kernel_mem.c）
 */
extern const uint8_t shared_mpu_num;

/*
 *  ドメイン用MPU領域の最大数（kernel_mem.c）
 */
extern const uint8_t max_domain_mpu_num;

/*
 *  MPU情報テーブル（kernel_mem.c）
 */
extern const MPU_INFO mpu_info_table[];

#if defined(TOPPERS_ML_AUTO)
#define USE_LATERPASS_DOMINIB
#endif

#ifndef OMIT_CORE_SET_GET_IPM
/*
 * （モデル上の）割込み優先度マスクの設定
 *
 *  CPUロックフラグがクリアされている時は，ハードウェアの割込み優先度マ
 *  スクを設定する．CPUロックフラグがセットされている時は，saved_iipm
 *  を設定し，さらに，ハードウェアの割込み優先度マスクを，設定しようと
 *  した（モデル上の）割込み優先度マスクとTIPM_LOCKの高い方に設定する．
 */
Inline void
t_set_ipm( PRI intpri )
{
	uint32_t	ipl = IPM_TO_IPL( intpri );
#if TIPM_LOCK == TIPM_LOCK_ALL
	set_ipl(ipl);
#else	/* TIPM_LOCK == TIPM_LOCK_ALL */
	if( !lock_flag ){
		set_ipl(ipl);
	}
	else {
		saved_ipl = ipl;
		set_ipl(ipl > IPL_LOCK ? ipl : IPL_LOCK);
    }
#endif	/* TIPM_LOCK == TIPM_LOCK_ALL */
}

/*
 * （モデル上の）割込み優先度マスクの参照
 *
 *  CPUロックフラグがクリアされている時はハードウェアの割込み優先度マ
 *  スクを，セットされている時はsaved_iipmを参照する．
 */
Inline PRI
t_get_ipm( void )
{
	uint32_t	ipl;
#if TIPM_LOCK == TIPM_LOCK_ALL
	ipl = current_ipl();
#else	/* TIPM_LOCK == TIPM_LOCK_ALL */
	if (!lock_flag) {
		ipl = current_ipl();
	}
	else {
		ipl = saved_ipl;
	}
#endif	/* TIPM_LOCK == TIPM_LOCK_ALL */
	return IPL_TO_IPM( ipl );
}
#endif

/*
 *  割込み要求ライン設定テーブル（kernel_cfg.c）
 *
 *  割込み要求ラインに対して割込み属性が設定されていれば1，設定されてい
 *  なければ0を保持するテーブル．
 */
extern const uint8_t cfg_int_table[];

/*
 *  割込み属性の設定のチェック
 */
Inline bool_t
check_intno_cfg(INTNO intno)
{
	return(cfg_int_table[intno] != 0U);
}

/*
 *  割込み要求がクリアできる状態か？
 */
Inline bool_t
check_intno_clear(INTNO intno)
{
	return(true);
}

/*
 *  割込みが要求できる状態か？
 */
Inline bool_t
check_intno_raise(INTNO intno)
{
	return(true);
}

/*
 *  非タスクコンテキスト用のスタック初期値
 */
#define TOPPERS_ISTKPT(istk, istksz)	((STK_T *)((char *)(istk) + (istksz)))

/*
 *  タスクコンテキストブロックの定義
 */
typedef struct task_context_block {
	void	*sp;		/* スタックポインタ */
	FP		pc;			/* プログラムカウンタ */
} TSKCTXB;

/*
 *  タスクディスパッチャ
 */

/*
 *  最高優先順位タスクへのディスパッチ（core_support.src）
 *
 *  dispatchは，タスクコンテキストから呼び出されたサービスコール処理か
 *  ら呼び出すべきもので，タスクコンテキスト・CPUロック状態・ディスパッ
 *  チ許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さな
 *  ければならない．
 */
extern void dispatch(void);

/*
 *  非タスクコンテキストからのディスパッチ要求
 */
#define request_dispatch_retint()

/*
 *  ディスパッチャの動作開始（core_support.src）
 *
 *  start_dispatchは，カーネル起動時に呼び出すべきもので，すべての割込
 *  みを禁止した状態（割込みロック状態と同等の状態）で呼び出さなければ
 *  ならない．
 */
extern void start_dispatch(void) NoReturn;

/*
 *  現在のコンテキストを捨ててディスパッチ（core_support.src）
 *
 *  exit_and_dispatchは，ext_tskから呼び出すべきもので，タスクコンテキ
 *  スト・CPUロック状態・ディスパッチ許可状態・（モデル上の）割込み優先
 *  度マスク全解除状態で呼び出さなければならない．
 */
extern void exit_and_dispatch(void) NoReturn;

/*
 *  カーネルの終了処理の呼出し（core_support.src）
 *
 *  call_exit_kernelは，カーネルの終了時に呼び出すべきもので，非タスク
 *  コンテキストに切り換えて，カーネルの終了処理（exit_kernel）を呼び出
 *  す．
 */
extern void call_exit_kernel(void) NoReturn;

/*
 *  タスクコンテキストの初期化
 *
 *  タスクが休止状態から実行できる状態に移行する時に呼ばれる．この時点
 *  でスタック領域を使ってはならない．
 *
 *  activate_contextを，インライン関数ではなくマクロ定義としているのは，
 *  この時点ではTCBが定義されていないためである．
 */
extern void start_stask_r(void);
extern void start_utask_r(void);

#define activate_context(p_tcb)											\
{																		\
	(p_tcb)->tskctxb.sp = (void *)((char *)((p_tcb)->p_tinib->sstk)		\
										+ (p_tcb)->p_tinib->sstksz);	\
	if ((p_tcb)->p_dominib->domptn == TACP_KERNEL) {					\
		(p_tcb)->tskctxb.pc = (FP) start_stask_r;						\
	}																	\
	else {																\
		(p_tcb)->tskctxb.pc = (FP) start_utask_r;						\
	}																	\
}


/*
 *  割込みハンドラの設定
 *
 *  RXはROMに割込みベクタを配置するため，本関数は空関数で実装する．
 */
Inline void
define_inh( INHNO inhno, FP inthdr )
{
}

/*
 *  割込み要求ラインの属性の設定
 */
extern void config_int(INTNO intno, ATR intatr, PRI intpri);

/*
 *  CPU例外ハンドラの設定
 *
 *  RXはROMに例外ベクタを配置するため，初期化は不要．
 */

#define OMIT_INITIALIZE_EXCEPTION

Inline void
initialize_exception(void)
{
}

/*
 *  割込みハンドラの入口処理の生成マクロ
 */
#define _INT_ENTRY(inhno, inthdr)				_kernel_##inthdr##_##inhno##_entry
#define INT_ENTRY(inhno, inthdr)				_INT_ENTRY(inhno , inthdr)

#define _INTHDR_ENTRY(inhno, inhno_num ,inthdr)	\
		extern _kernel_##inthdr##_##inhno##_entry(void);
#define INTHDR_ENTRY(inhno, inhno_num ,inthdr)	_INTHDR_ENTRY(inhno, inhno_num ,inthdr)

/*
 *  CPU例外の発生した時のコンテキストの参照
 *
 *  CPU例外の発生した時のコンテキストが，タスクコンテキストの時にFALSE，
 *  そうでない時にtrueを返す．
 */
Inline bool_t
exc_sense_context( void *p_excinf )
{
	/*
	 *  ネストカウンタが1より大なら非タスクコンテキスト
	 */
	return( intnest > 1U );
}

/*
 *	アキュムレータ用レジスタのサイズ
 *	RXv1: ACC-HI, ACC-LO. 4 * 2 = 8 Bytes.
 *	RXv2: ACC0-GU, ACC0-HI, ACC0-LO, ACC1-GU, ACC1-HI, ACC1-LO. 4 * 6 = 24 Bytes.
 */
#if defined(__RXv1__)
#define RX_ACC_REGS_SIZE (8U)
#elif defined(__RXv2__)
#define RX_ACC_REGS_SIZE (24U)
#else
#error "Unsupported ISA"
#endif

/*
 *  CPU例外情報 p_excinf から PSW の値を取得するためのオフセット値
 *  EXCNO + RUNDOM + ACC + FPSW + R1～R15 + PC
 */
#define EXC_GET_PSW_OFFSET	(4+4+RX_ACC_REGS_SIZE+4+60+4)

/*
 *  CPU例外の発生した時のIPLの参照
 */
Inline uint32_t
exc_get_ipl(void *p_excinf)
{
	return((*((uint32_t *)((uintptr_t)p_excinf+EXC_GET_PSW_OFFSET)))
			& PSW_IPL_MASK);
}

Inline bool_t
exc_sense_i(void *p_excinf)
{
	return((*(((uint32_t *)((uintptr_t)p_excinf+EXC_GET_PSW_OFFSET)))
			 & PSW_I_MASK) != 0u);
}

/*
 *  CPU例外の発生した時のコンテキストと割込みのマスク状態の参照
 *
 *  CPU例外の発生した時のシステム状態が，カーネル実行中でなく，タスクコ
 *  ンテキストであり，全割込みロック状態でなく，CPUロック状態でなく，（モ
 *  デル上の）割込み優先度マスク全解除状態である時にtrue，そうでない時
 *  にFALSEを返す（CPU例外がカーネル管理外の割込み処理中で発生した場合
 *  にもFALSEを返す）．
 *
 *  カーネル実行中でない→ (TIPM_LOCK== TIPM_LOCK_ALL) Iフラグ == 1
 *                         (else)           IPL < IPL_LOCK
 *  タスクコンテキストである→intnest == 0
 *  全割込みロックでない→ Iフラグ == 1
 *  割込み優先度マスクが全解除→IPL == 0
 *
 *
 */
Inline bool_t
exc_sense_intmask(void *p_excinf)
{
	return((!exc_sense_context(p_excinf))
			&& exc_sense_i(p_excinf)
			&& (exc_get_ipl(p_excinf) == IPL_ENAALL));
}

/*
 *  コア依存の初期化
 */
extern void core_initialize(void);

/*
 *  システム周期オーバラン例外の要求
 */
Inline void
raise_scycovr_exception(void)
{
	Asm("int %0" :: "n"(SVC_SCYCOVR));
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_KERNEL_IMPL_H */
