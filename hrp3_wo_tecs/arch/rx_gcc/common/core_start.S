/*
 * TOPPERS/HRP Kernel
 *     Toyohashi Open Platform for Embedded Real-Time Systems/
 *     High Reliable system Profile Kernel
 *
 * Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                             Toyohashi Univ. of Technology, JAPAN
 * Copyright (C) 2005,2006 by Embedded and Real-Time Systems Laboratory
 *             Graduate School of Information Science, Nagoya Univ., JAPAN
 * Copyright (C) 2008-2010 by Witz Corporation, JAPAN
 * Copyright (C) 2013      by Mitsuhiro Matsuura
 * Copyright (C) 2018-2019 by Embedded and Real-Time Systems Laboratory
 *             Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 * 上記著作権者は，Free Software Foundation によって公表されている 
 * GNU General Public License の Version 2 に記述されている条件か，以
 * 下の(1)～(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 * アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 * 利用と呼ぶ）することを無償で許諾する．
 * (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *     権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *     スコード中に含まれていること．
 * (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *     ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *     に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *     この利用条件および下記の無保証規定を掲載すること．
 * (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *     み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *   (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *       権表示，この利用条件および下記の無保証規定を掲載すること．
 *   (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *       こと．
 * (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *     害からも，上記著作権者を免責すること．
 *
 * 本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 * 本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 * ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 * かなる損害に関しても，その責任を負わない．
 * 
 */

/*
 * RXコア共通の初期化ルーチン
 */
	.text

	.global	_sta_ker
	.global	_software_init_hook
	.global	_hardware_init_hook
	.global	__kernel_istkpt
	.global	__kernel_tnum_datasec
	.global	__kernel_datasecinib_table
	.global	__kernel_tnum_bsssec
	.global	__kernel_bsssecinib_table
	.global	VVECTOR_TOP
	.global	_core_start
_core_start:
	/*
	 * スタックポインタの設定
	 *
	 * ユーザにて非タスクコンテキスト用スタック領域を用意した場合,
	 * ユーザ領域を設定する.
	 * kernel_cfg.cにて"_kernel_istkpt"の値を決定する.
	 */
	mov.l	#__kernel_istkpt, r5
	mov.l	[r5], r0			/* 非タスクコンテキスト用スタック領域を設定 */

	/*
	 * 割込みベクタテーブルの設定
	 */
	mvtc	#VVECTOR_TOP, intb			/* ベクタテーブルの設定 */

	/*
	 * ハードウェア固有の初期化ルーチンの呼出し
	 */
	mov.l	#_hardware_init_hook, r5
	cmp		#0, r5
	beq.b	initialize_datasec
	jsr		r5

	/*
	 * DATAセクションとBSSセクションの初期化
	 */
initialize_datasec:
	mov.l	#__kernel_tnum_datasec, r4
	mov.l	[r4], r4
	mov.l	#__kernel_datasecinib_table, r5
initialize_datasec_loop:
	cmp		#0, r4
	beq		initialize_bsssec
	mov.l	0[r5], r1
	mov.l	8[r5], r2
	mov.l	4[r5], r3
	sub		r1, r3
	smovf
	add		#12, r5
	sub		#1, r4
	bra		initialize_datasec_loop
initialize_bsssec:
	mov.l	#__kernel_tnum_bsssec, r4
	mov.l	[r4], r4
	mov.l	#__kernel_bsssecinib_table, r5
	mov.l	#0, r2
initialize_bsssec_loop:
	cmp		#0, r4
	beq		initialize_bsssec_end
	mov.l	0[r5], r1
	mov.l	4[r5], r3
	sub		r1, r3
	sstr.b
	add		#8, r5
	sub		#1, r4
	bra		initialize_bsssec_loop
initialize_bsssec_end:

	/*
	 * software_init_hook を呼出し（0 でない場合）
	 *
	 * ソフトウェア環境（特にライブラリ）に依存して必要な初期化処理
	 * がある場合は，software_init_hook という関数を用意すればよい.
	 */
	mov.l	#_software_init_hook, r5
	cmp		#0, r5
	beq.b	kernel_link
	jsr		r5

	/*
	 * カーネルスタート
	 */
kernel_link:
	bra.a	_sta_ker					/* --> _sta_ker() */

	.end
