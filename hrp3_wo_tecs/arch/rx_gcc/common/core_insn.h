/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2017 by Embedded and Real-Time Systems Laboratory
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
 *		RXコアの特殊命令のインライン関数定義
 */

#ifndef TOPPERS_CORE_INSN_H
#define TOPPERS_CORE_INSN_H

/*
 *  PSW関連のマクロ定義
 */
#define PSW_I_MASK		( 0x00010000UL )
#define PSW_U_MASK		( 0x1UL << 17UL )
#define PSW_PM_MASK		( 0x1UL << 20UL )
#define PSW_IPL_MASK	( 0x0F000000UL )

#ifndef TOPPERS_MACRO_ONLY

/*
 *  プロセッサステータスレジスタ(PSW)の現在値の読出し
 */
static uint32_t
current_psw( void )
{
	uint32_t psw;
	Asm("mvfc	psw, %0" : "=r"(psw));
	return psw;
}

/*
 *  プロセッサステータスレジスタ(PSW)へ設定
 */
static void
set_psw( uint32_t psw )
{
	Asm("mvtc %0, psw" :: "r"(psw));
}

/*
 *  IPLの現在値の読出し
 */
Inline uint32_t
current_ipl( void )
{
	return ( uint32_t )( current_psw() & PSW_IPL_MASK );
}

/*
 *  IPLへ設定
 */
Inline void
set_ipl( uint32_t ipl )
{
	set_psw( ( current_psw() & ( ~PSW_IPL_MASK ) ) | ipl );
}

/*
 *  NMIを除くすべての割込みの禁止
 */
static void
disint( void )
{
	Asm("clrpsw	i");
}

/*
 *  NMIを除くすべての割込みの許可
 */
static void
enaint( void )
{
	Asm("setpsw	i");
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_INSN_H */
