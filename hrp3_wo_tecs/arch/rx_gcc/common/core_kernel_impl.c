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
 *		カーネルのコア依存部（RX用）
 */

#include <sil.h>
#include "kernel_impl.h"

/*
 *  割込み発生回数を保存する変数
 */
uint16_t	intnest;

#if TIPM_LOCK != TIPM_LOCK_ALL
bool_t	lock_flag;		/* CPUロックフラグの値を保持する変数 */
uint32_t	saved_ipl;		/* 割込み優先度レベルを保存する変数 */
#endif	/* TIPM_LOCK != TIPM_LOCK_ALL */


/*
 *  コア依存の初期化
 */
void
core_initialize(void)
{
	uint8_t i;

	/*
	 *  MPU有効化
	 */
	for (i = 0; i < shared_mpu_num; i++) {
		sil_wrw_mem((void *)MPU_RSPAGEn_ADDR(7-i), mpu_info_table[i].rspage);
		sil_wrw_mem((void *)MPU_REPAGEn_ADDR(7-i), mpu_info_table[i].repage);
	}
	sil_wrw_mem((void *)MPU_MPEN_ADDR, 0x1);
}

#ifndef OMIT_DEFAULT_INT_HANDLER
/*
 *  未登録の割込みが発生した場合に呼び出される
 */
void
default_int_handler( uint32_t intno )
{
	syslog(LOG_EMERG, "Unregistered interrupt %d occurs.", intno);
}
#endif /* OMIT_DEFAULT_INT_HANDLER */

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 *  未登録の例外が発生した場合に呼び出される
 */
void
default_exc_handler( void *p_excinf )
{
	uint32_t excno = *(uint32_t*)(p_excinf);
	switch(excno) {
	case EXCNO_PRIV_INST:
		syslog(LOG_EMERG, "Privileged instruction exception occurs.");
		break;
	case EXCNO_ACCESS:
		syslog(LOG_EMERG, "Access exception occurs.");
		syslog(LOG_EMERG, "MPDEA:  0x%x", sil_rew_mem((void *)MPU_MPDEA_ADDR));
		syslog(LOG_EMERG, "MPESTS: 0x%x", sil_rew_mem((void *)MPU_MPESTS_ADDR));
		break;
	case EXCNO_UNDEF:
		syslog(LOG_EMERG, "Undefined instruction exception occurs.");
		break;
	case EXCNO_FLOAT:
		syslog(LOG_EMERG, "Floating-point exception occurs.");
		break;
	case EXCNO_NMI:
		syslog(LOG_EMERG, "Non-maskable interrupt occurs.");
		break;
	case EXCNO_SCYCOVR:
		/*
		 *  システム周期オーバラン例外の時は，エラーメッセージだけを出
		 *  力してリターンする．
		 */
		syslog(LOG_ERROR, "System cycle overrun occurs.");
		return;
	default:
		syslog(LOG_EMERG, "Unexpected exception %d occurs.", excno);
	}
	ext_ker();
}
#endif /* OMIT_DEFAULT_EXC_HANDLER */
