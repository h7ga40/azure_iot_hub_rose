/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 *
 *  Copyright (C) 2008-2010 by Witz Corporation, JAPAN
 *  Copyright (C) 2013      by Mitsuhiro Matsuura
 *
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
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
 * UART用 簡易SIOドライバ
 */

#include <sil.h>
#include <kernel.h>
#include <t_syslog.h>
#include "target_serial.h"
#include "target_syssvc.h"

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
	volatile uint8_t	*ctlreg;		/* シリアルコントロールレジスタ（SCR) */
	volatile uint8_t	*modereg;		/* シリアルモードレジスタ（SMR) */
	volatile uint8_t	*scmr;			/* スマートカードモードレジスタ（SCMR) */
	volatile uint8_t	*extmodereg;	/* シリアル拡張モードレジスタ（SEMR) */
	volatile uint8_t	*tdreg;			/* トランスミットデータレジスタ（TDR)*/
	volatile uint8_t	*rdreg;			/* レシーブデータレジスタ（RDR) */
	volatile uint8_t	*bitratereg;	/* ビットレートレジスタ（BRR) */
	volatile uint32_t	*mstpcrreg;		/* モジュールストップコントロールレジスタ（MSTPCR） */
	volatile uint8_t	*ssrreg;		/* ステータスレジスタ */
	uint8_t				tx_intno;		/* 送信（データエンプティ）割り込み番号 */
	uint8_t				rx_intno;		/* 受信（データフル）割り込み番号 */
	uint32_t			mstpcr_offset;	/* MSTPCRの対応するビットオフセット */
	volatile uint32_t	*erireg;		/* エラー割り込み許可レジスタ */
	uint8_t				eribit;			/* エラー割り込み許可ビット */
} SIOPINIB;

#if defined(SCI0_SIO_ID)
static const SIOPINIB siopinib_sci0 = {
	(volatile uint8_t *)SCI0_SCR_ADDR,
	(volatile uint8_t *)SCI0_SMR_ADDR,
	(volatile uint8_t *)SCI0_SCMR_ADDR,
	(volatile uint8_t *)SCI0_SEMR_ADDR,
	(volatile uint8_t *)SCI0_TDR_ADDR,
	(volatile uint8_t *)SCI0_RDR_ADDR,
	(volatile uint8_t *)SCI0_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI0_SSR_ADDR,
	INT_TXI0,
	INT_RXI0,
	SYSTEM_MSTPCRB_MSTPB31_BIT,
	ICU_GENBL0_ADDR,
	1
};
#endif

#if defined(SCI1_SIO_ID)
static const SIOPINIB siopinib_sci1 = {
	(volatile uint8_t *)SCI1_SCR_ADDR,
	(volatile uint8_t *)SCI1_SMR_ADDR,
	(volatile uint8_t *)SCI1_SCMR_ADDR,
	(volatile uint8_t *)SCI1_SEMR_ADDR,
	(volatile uint8_t *)SCI1_TDR_ADDR,
	(volatile uint8_t *)SCI1_RDR_ADDR,
	(volatile uint8_t *)SCI1_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI1_SSR_ADDR,
	INT_TXI1,
	INT_RXI1,
	SYSTEM_MSTPCRB_MSTPB30_BIT,
	ICU_GENBL0_ADDR,
	3
};
#endif

#if defined(SCI2_SIO_ID)
static const SIOPINIB siopinib_sci2 = {
	(volatile uint8_t *)SCI2_SCR_ADDR,
	(volatile uint8_t *)SCI2_SMR_ADDR,
	(volatile uint8_t *)SCI2_SCMR_ADDR,
	(volatile uint8_t *)SCI2_SEMR_ADDR,
	(volatile uint8_t *)SCI2_TDR_ADDR,
	(volatile uint8_t *)SCI2_RDR_ADDR,
	(volatile uint8_t *)SCI2_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI2_SSR_ADDR,
	INT_TXI2,
	INT_RXI2,
	SYSTEM_MSTPCRB_MSTPB29_BIT,
	ICU_GENBL0_ADDR,
	5
};
#endif

#if defined(SCI3_SIO_ID)
static const SIOPINIB siopinib_sci3 = {
	(volatile uint8_t *)SCI3_SCR_ADDR,
	(volatile uint8_t *)SCI3_SMR_ADDR,
	(volatile uint8_t *)SCI3_SCMR_ADDR,
	(volatile uint8_t *)SCI3_SEMR_ADDR,
	(volatile uint8_t *)SCI3_TDR_ADDR,
	(volatile uint8_t *)SCI3_RDR_ADDR,
	(volatile uint8_t *)SCI3_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI3_SSR_ADDR,
	INT_TXI3,
	INT_RXI3,
	SYSTEM_MSTPCRB_MSTPB28_BIT,
	ICU_GENBL0_ADDR,
	7
};
#endif

#if defined(SCI4_SIO_ID)
static const SIOPINIB siopinib_sci4 = {
	(volatile uint8_t *)SCI4_SCR_ADDR,
	(volatile uint8_t *)SCI4_SMR_ADDR,
	(volatile uint8_t *)SCI4_SCMR_ADDR,
	(volatile uint8_t *)SCI4_SEMR_ADDR,
	(volatile uint8_t *)SCI4_TDR_ADDR,
	(volatile uint8_t *)SCI4_RDR_ADDR,
	(volatile uint8_t *)SCI4_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI4_SSR_ADDR,
	INT_TXI4,
	INT_RXI4,
	SYSTEM_MSTPCRB_MSTPB27_BIT,
	ICU_GENBL0_ADDR,
	9
};
#endif

#if defined(SCI5_SIO_ID)
static const SIOPINIB siopinib_sci5 = {
	(volatile uint8_t *)SCI5_SCR_ADDR,
	(volatile uint8_t *)SCI5_SMR_ADDR,
	(volatile uint8_t *)SCI5_SCMR_ADDR,
	(volatile uint8_t *)SCI5_SEMR_ADDR,
	(volatile uint8_t *)SCI5_TDR_ADDR,
	(volatile uint8_t *)SCI5_RDR_ADDR,
	(volatile uint8_t *)SCI5_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI5_SSR_ADDR,
	INT_TXI5,
	INT_RXI5,
	SYSTEM_MSTPCRB_MSTPB26_BIT,
	ICU_GENBL0_ADDR,
	11
};
#endif

#if defined(SCI6_SIO_ID)
static const SIOPINIB siopinib_sci6 = {
	(volatile uint8_t *)SCI6_SCR_ADDR,
	(volatile uint8_t *)SCI6_SMR_ADDR,
	(volatile uint8_t *)SCI6_SCMR_ADDR,
	(volatile uint8_t *)SCI6_SEMR_ADDR,
	(volatile uint8_t *)SCI6_TDR_ADDR,
	(volatile uint8_t *)SCI6_RDR_ADDR,
	(volatile uint8_t *)SCI6_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI6_SSR_ADDR,
	INT_TXI6,
	INT_RXI6,
	SYSTEM_MSTPCRB_MSTPB25_BIT,
	ICU_GENBL0_ADDR,
	13
};
#endif

#if defined(SCI7_SIO_ID)
static const SIOPINIB siopinib_sci7 = {
	(volatile uint8_t *)SCI7_SCR_ADDR,
	(volatile uint8_t *)SCI7_SMR_ADDR,
	(volatile uint8_t *)SCI7_SCMR_ADDR,
	(volatile uint8_t *)SCI7_SEMR_ADDR,
	(volatile uint8_t *)SCI7_TDR_ADDR,
	(volatile uint8_t *)SCI7_RDR_ADDR,
	(volatile uint8_t *)SCI7_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRB_ADDR,
	(volatile uint8_t *)SCI7_SSR_ADDR,
	INT_TXI7,
	INT_RXI7,
	SYSTEM_MSTPCRB_MSTPB24_BIT,
	ICU_GENBL0_ADDR,
	15
};
#endif

#if defined(SCI8_SIO_ID)
static const SIOPINIB siopinib_sci8 = {
	(volatile uint8_t *)SCI8_SCR_ADDR,
	(volatile uint8_t *)SCI8_SMR_ADDR,
	(volatile uint8_t *)SCI8_SCMR_ADDR,
	(volatile uint8_t *)SCI8_SEMR_ADDR,
	(volatile uint8_t *)SCI8_TDR_ADDR,
	(volatile uint8_t *)SCI8_RDR_ADDR,
	(volatile uint8_t *)SCI8_BRR_ADDR,
	(volatile uint32_t *)SYSTEM_MSTPCRC_ADDR,
	(volatile uint8_t *)SCI8_SSR_ADDR,
	INT_TXI8,
	INT_RXI8,
	SYSTEM_MSTPCRC_MSTPC27_BIT,
	ICU_GENBL1_ADDR,
	25
};
#endif

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
struct sio_port_control_block {
	const SIOPINIB	*p_siopinib; 				/* シリアルI/Oポート初期化ブロック */
	EXINF 	exinf;			 				/* 拡張情報 */
	bool_t		openflag;						/* オープン済みフラグ */
	bool_t		sendflag;						/* 送信割込みイネーブルフラグ */
	bool_t		getready;						/* 文字を受信した状態 */
	bool_t		putready;						/* 文字を送信できる状態 */
	bool_t		is_initialized; 				/* デバイス初期化済みフラグ */
};

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
static SIOPCB	siopcb_table[TNUM_PORT];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)	 ((uint_t)((siopid) - 1))
#define get_siopcb(siopid)	 (&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  SCIドライバの初期化ルーチン
 *
 *  References:
 *  [1] 40.3.6 SCI の初期化（調歩同期式モード） (i.e. UART)
 */
static void
uart_setmode(const SIOPINIB *p_siopinib, uint32_t baud, uint8_t cks)
{
	/*
	 *  モジュールストップ機能の設定
	 */
	sil_wrh_mem((void *)SYSTEM_PRCR_ADDR, SYSTEM_PRCR_PRKEY_BITS | SYSTEM_PRCR_PRC1_BIT);	/* 書込み許可 */
	sil_wrw_mem((void *)p_siopinib->mstpcrreg,
		sil_rew_mem((void *)p_siopinib->mstpcrreg) & ~p_siopinib->mstpcr_offset);
	sil_wrh_mem((void *)SYSTEM_PRCR_ADDR, SYSTEM_PRCR_PRKEY_BITS);	/* 書込み禁止 */

	/* SCR.TIE、RIE、TE、RE、TEIEビットを“0”に設定 */
	sil_wrb_mem((void *)p_siopinib->ctlreg, 0x00U);

	/* I/Oポート機能を設定 */
	/* NOTE: ターゲット依存のため、ターゲット初期化の時で設定 */

	/* SCR.CKE[1:0]ビットを設定 */
	/* NOTE: デフォルトの値（内蔵ボーレートジェネレータ）を使用 */

	/* SIMR1.IICMビットを“0”に設定, SPMR.CKPH、CKPOLビットを“0”に設定 */
	/* 初期値から変更していない場合、本手順は省略できます。 */

	/* SMR、SCMR、SEMRレジスタに送信/受信フォーマットを設定 */
	/* NOTE: フォーマットは8N1、SCMRとSEMRはリセットに */
	sil_wrb_mem((void *)p_siopinib->modereg, cks);
	sil_wrb_mem((void *)p_siopinib->scmr, 0xF2U);
	sil_wrb_mem((void *)p_siopinib->extmodereg, 0x0);

	/* ビットレートを設定 */
	sil_wrb_mem((void *)p_siopinib->bitratereg, ((FREQ_PCLK_BCD / 32) >> (2 * cks)) / baud - 1);

	/*
	 * MDDRレジスタに値を設定
	 * ただし、SEMR.BRMEビットを“0”にした場合および外部クロックを使用する場合には設定の必要はありません。
	 */

	/* SCR.TE、REビットを“1”に設定、SCR.TIE、RIEも許可 */
	sil_wrb_mem((void *)p_siopinib->ctlreg,
			(sil_reb_mem((void *)p_siopinib->ctlreg) | SCIn_SCR_RE_BIT | SCIn_SCR_TE_BIT | SCIn_SCR_TIE_BIT | SCIn_SCR_RIE_BIT));
}


/*
 *  SIOドライバの初期化
 */
void
sio_initialize(EXINF exinf)
{
	SIOPCB	*p_siopcb;
	uint_t	i;
	ER      ercd;

	/*
	 *  シリアルI/Oポート管理ブロックの初期化
	 */
	for (p_siopcb = siopcb_table, i = 0; i < TNUM_PORT; p_siopcb++, i++){
		p_siopcb->openflag = false;
		p_siopcb->sendflag = false;
		switch (i + 1) {
#if defined(SCI0_SIO_ID)
		case SCI0_SIO_ID:
			get_siopcb(SCI0_SIO_ID)->p_siopinib = &siopinib_sci0;
			break;
#endif
#if defined(SCI1_SIO_ID)
		case SCI1_SIO_ID:
			get_siopcb(SCI1_SIO_ID)->p_siopinib = &siopinib_sci1;
			break;
#endif
#if defined(SCI2_SIO_ID)
		case SCI2_SIO_ID:
			get_siopcb(SCI2_SIO_ID)->p_siopinib = &siopinib_sci2;
			break;
#endif
#if defined(SCI3_SIO_ID)
		case SCI3_SIO_ID:
			get_siopcb(SCI3_SIO_ID)->p_siopinib = &siopinib_sci3;
			break;
#endif
#if defined(SCI4_SIO_ID)
		case SCI4_SIO_ID:
			get_siopcb(SCI4_SIO_ID)->p_siopinib = &siopinib_sci4;
			break;
#endif
#if defined(SCI5_SIO_ID)
		case SCI5_SIO_ID:
			get_siopcb(SCI5_SIO_ID)->p_siopinib = &siopinib_sci5;
			break;
#endif
#if defined(SCI6_SIO_ID)
		case SCI6_SIO_ID:
			get_siopcb(SCI6_SIO_ID)->p_siopinib = &siopinib_sci6;
			break;
#endif
#if defined(SCI7_SIO_ID)
		case SCI7_SIO_ID:
			get_siopcb(SCI7_SIO_ID)->p_siopinib = &siopinib_sci7;
			break;
#endif
#if defined(SCI8_SIO_ID)
		case SCI8_SIO_ID:
			get_siopcb(SCI8_SIO_ID)->p_siopinib = &siopinib_sci8;
			break;
#endif
		}
	}

	/*
	 *  カーネル起動時のバナー出力用の初期化
	 */
	p_siopcb = get_siopcb(FPUT_PORTID);
	/*  二重初期化の防止  */
	p_siopcb->is_initialized = true;
	/*  ハードウェアの初期化処理と送信許可  */
	uart_setmode(p_siopcb->p_siopinib, UART_DEFAULT_BAUD, UART_DEFAULT_CKS);
	sil_wrb_mem((void *)p_siopcb->p_siopinib->ctlreg,
					(uint8_t)(sil_reb_mem((void *)p_siopcb->p_siopinib->ctlreg) | SCIn_SCR_TE_BIT));
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
sio_opn_por(ID siopid, EXINF exinf)
{
	SIOPCB          *p_siopcb;
	const SIOPINIB  *p_siopinib;
	ER      		ercd;

	p_siopcb = get_siopcb(siopid);
	p_siopinib = p_siopcb->p_siopinib;

	/*
	 *  シリアルI/O割込みをマスクする．
	 */
	ercd = dis_int(p_siopinib->tx_intno);
	assert(ercd == E_OK);
	ercd = dis_int(p_siopinib->rx_intno);
	assert(ercd == E_OK);

	/*
	 *  ハードウェアの初期化
	 *
	 *  既に初期化している場合は, 二重に初期化しない.
	 */
	if(!(p_siopcb->is_initialized)){
		uart_setmode(p_siopinib, UART_DEFAULT_BAUD, UART_DEFAULT_CKS);
		p_siopcb->is_initialized = true;
	}

	p_siopcb->exinf = exinf;
	p_siopcb->getready = p_siopcb->putready = false;
	p_siopcb->openflag = true;

	/*
	 *  シリアルI/O割込みのマスクは解除しない
	 *  NOTE: ここで解除したら、不要なTXIが発生する。
	 *  必要な場合、serial.cはsio_ena_cbrを使って解除してくれる。
	 */

	/* エラーフラグをクリア */
	sil_wrb_mem((void *)p_siopinib->ssrreg,
		sil_reb_mem((void *)p_siopinib->ssrreg) & 0b11000111);

	/* エラー割り込みマスク解除 */
	sil_wrw_mem((void *)p_siopinib->erireg,
		sil_rew_mem((void *)p_siopinib->erireg) | (1 << p_siopinib->eribit));

	ercd = ena_int(INT_GROUPBL0);
	assert(ercd == E_OK);
	ercd = ena_int(INT_GROUPBL1);
	assert(ercd == E_OK);

    return (p_siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
	ER      		ercd;

	/*
	 *  デバイス依存のクローズ処理: UART停止
	 */
	sil_wrb_mem((void *)p_siopcb->p_siopinib->ctlreg, 0x00U);

	p_siopcb->openflag = false;
	p_siopcb->is_initialized = false;

	/*
	 *  シリアルI/O割込みをマスクする．
	 */
	ercd = dis_int(p_siopcb->p_siopinib->tx_intno);
	assert(ercd == E_OK);
	ercd = dis_int(p_siopcb->p_siopinib->rx_intno);
	assert(ercd == E_OK);

	/* エラー割り込みをマスクする */
	sil_wrw_mem((void *)p_siopcb->p_siopinib->erireg,
		sil_rew_mem((void *)p_siopcb->p_siopinib->erireg) & ~(1 << p_siopcb->p_siopinib->eribit));
}

/*
 *  SIOの割込みハンドラ
 */
void sio_isr_txi(EXINF exinf)
{
	SIOPCB	*p_siopcb = get_siopcb(exinf);

	/*
	 *  TXI発生した後も、sio_snd_chrが呼び出されたことがあるため、チェックが必要。
	 */
	if((sil_reb_mem(
		(void *)p_siopcb->p_siopinib->ssrreg) & SCIn_SSR_TDRE_BIT/*SCIn_SSR_TEND_BIT*/) != 0U){
		/*
		 *  送信可能コールバックルーチンを呼び出す．
		 */
		sio_irdy_snd(p_siopcb->exinf);
	}
}

/*
 *  SIOの割込みハンドラ
 */
void sio_isr_rxi(EXINF exinf)
{
	SIOPCB	*p_siopcb = get_siopcb(exinf);

	if((sil_reb_mem(
		(void *)p_siopcb->p_siopinib->ssrreg) & SCIn_SSR_RDRF_BIT) != 0U){
		/*
		 *  受信通知コールバックルーチンを呼び出す．
		 */
		sio_irdy_rcv(p_siopcb->exinf);
	}
}

/*
 *  シリアルI/Oポートへの文字送信
 */
bool_t
sio_snd_chr(SIOPCB *p_siopcb, char c)
{
	bool_t ercd = false;

	if((sil_reb_mem(
		(void *)p_siopcb->p_siopinib->ssrreg) & SCIn_SSR_TDRE_BIT) != 0){
		sil_wrb_mem((void *)p_siopcb->p_siopinib->tdreg, (uint8_t)c);
		ercd = true;
	}

	return ercd;
}

/*
 *  シリアルI/Oポートからの文字受信
 */
int_t
sio_rcv_chr(SIOPCB *p_siopcb)
{
	int_t c = -1;

	if((sil_reb_mem(
		(void *)p_siopcb->p_siopinib->ssrreg) & SCIn_SSR_RDRF_BIT) != 0U){
		c = (int)(sil_reb_mem((void *)p_siopcb->p_siopinib->rdreg));
	}

	return c;
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	/*
	 * SCRのTIE、RIEではなく、割り込みコントローラで割り込みの禁止 / 許可を制御。
	 * References:
	 * [1] 40.12.2 調歩同期式モード、クロック同期式モードおよび簡易 SPI モードにおける割り込み -> 注 2
	 */
	switch (cbrtn) {
		case SIO_RDY_SND:
			ena_int(p_siopcb->p_siopinib->tx_intno);
			break;
		case SIO_RDY_RCV:
			ena_int(p_siopcb->p_siopinib->rx_intno);
			break;
		default:
			assert(0);
			break;
	}
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
	switch (cbrtn) {
		case SIO_RDY_SND:
			dis_int(p_siopcb->p_siopinib->tx_intno);
			break;
		case SIO_RDY_RCV:
			dis_int(p_siopcb->p_siopinib->rx_intno);
			break;
		default:
			assert(0);
			break;
	}
}


/*
 *  シリアルI/Oポートへのポーリングでの出力
 */
static void
uart_pol_putc(char c, ID siopid)
{
	const SIOPINIB *p_siopinib;

	p_siopinib = get_siopcb(siopid)->p_siopinib;

	/*
	 *  送信レジスタが空になるまで待つ
	 */
	while((sil_reb_mem(
			(void *)p_siopinib->ssrreg) & SCIn_SSR_TDRE_BIT /*SCIn_SSR_TEND_BIT*/) == 0U);

	sil_wrb_mem((void *)p_siopinib->tdreg, (uint8_t)c);
}

/*
 *  システムログの低レベル出力のための文字出力
 */
void
target_fput_log( char c )
{
	if( c == '\n' ){
	   uart_pol_putc( '\r' , FPUT_PORTID );
	}

    uart_pol_putc( c , FPUT_PORTID );
}

/*
 *  SIOの割込みハンドラ
 */
void sio_isr_eri(EXINF exinf)
{
	if (exinf == 0) {
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 1)) != 0){
			sil_wrb_mem((void *)SCI0_SSR_ADDR,
				sil_reb_mem((void *)SCI0_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 3)) != 0){
			sil_wrb_mem((void *)SCI1_SSR_ADDR,
				sil_reb_mem((void *)SCI1_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 5)) != 0){
			sil_wrb_mem((void *)SCI2_SSR_ADDR,
				sil_reb_mem((void *)SCI2_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 7)) != 0){
			sil_wrb_mem((void *)SCI3_SSR_ADDR,
				sil_reb_mem((void *)SCI3_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 9)) != 0){
			sil_wrb_mem((void *)SCI4_SSR_ADDR,
				sil_reb_mem((void *)SCI4_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 11)) != 0){
			sil_wrb_mem((void *)SCI5_SSR_ADDR,
				sil_reb_mem((void *)SCI5_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 13)) != 0){
			sil_wrb_mem((void *)SCI6_SSR_ADDR,
				sil_reb_mem((void *)SCI6_SSR_ADDR) & 0b11000111);
		}
		if((sil_rew_mem((void *)ICU_GRPBL0_ADDR) & (1 << 15)) != 0){
			sil_wrb_mem((void *)SCI7_SSR_ADDR,
				sil_reb_mem((void *)SCI7_SSR_ADDR) & 0b11000111);
		}
	}
	else {
		if((sil_rew_mem((void *)ICU_GRPBL1_ADDR) & (1 << 25)) != 0){
			sil_wrb_mem((void *)SCI8_SSR_ADDR,
				sil_reb_mem((void *)SCI8_SSR_ADDR) & 0b11000111);
		}
	}
}
