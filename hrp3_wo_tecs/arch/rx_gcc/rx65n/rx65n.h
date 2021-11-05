
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

#ifndef TOPPERS_RX65N_H
#define TOPPERS_RX65N_H

#define __evenaccess

/*
 *  割込み番号の定義(可変ベクタテーブル)
 */
#define INT_SWINT2   26
#define INT_SWINT    27
#define INT_CMWI0    30
#define INT_CMWI1    31
#define INT_RXI1     60
#define INT_TXI1     61
#define INT_IRQ_MIN  64
#define INT_IRQ_MAX  79
#define INT_RXI3     80
#define INT_TXI3     81
#define INT_RXI4     82
#define INT_TXI4     83
#define INT_RXI5     84
#define INT_TXI5     85
#define INT_RXI6     86
#define INT_TXI6     87
#define INT_RXI8     100
#define INT_TXI8     101
#define INT_GROUPBE0 106
#define INT_GROUPBL2 107
#define INT_GROUPBL0 110
#define INT_GROUPBL1 111
#define INT_GROUPAL0 112
#define INT_GROUPAL1 113
#define TMAX_INTNO   255

/*
 *  各モジュールのレジスタ及び設定ビット情報
 */

/*
 *  SYSTEMモジュール
 */
#define SYSTEM_PRCR_ADDR			( ( volatile uint16_t __evenaccess * )0x000803FE )
#define SYSTEM_PRCR_PRC0_BIT		( 0x1UL << 0U )
#define SYSTEM_PRCR_PRC1_BIT		( 0x1UL << 1U )
#define SYSTEM_PRCR_PRC3_BIT		( 0x1UL << 3U )
#define SYSTEM_PRCR_PRKEY_BITS		( 0xA5UL << 8U )
#define SYSTEM_MOFCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C293 )
#define SYSTEM_MOFCR_MOFXIN_BIT		( 1UL << 0U )
#define SYSTEM_MOFCR_MODRV2_SHIFT	( 4U )
#define SYSTEM_MOFCR_MOSEL_BIT		( 1UL << 6U )
#define SYSTEM_MOSCWTCR_ADDR		( ( volatile uint8_t __evenaccess * )0x000800A2 )
#define SYSTEM_MOSCCR_ADDR			( ( volatile uint8_t __evenaccess * )0x00080032 )
#define SYSTEM_MOSCCR_MOSTP_BIT		( 1UL << 0U )
#define SYSTEM_PLLCR_ADDR			( ( volatile uint16_t __evenaccess * )0x00080028 )
#define SYSTEM_PLLCR_PLIDIV_SHIFT	( 0U )
#define SYSTEM_PLLCR_STC_SHIFT		( 8U )
#define SYSTEM_PLLCR2_ADDR			( ( volatile uint8_t __evenaccess * )0x0008002A )
#define SYSTEM_OSCOVFSR_ADDR		( ( volatile uint8_t __evenaccess * )0x0008003C )
#define SYSTEM_OSCOVFSR_PLOVF_BIT	( 1UL << 2U )
#define SYSTEM_SCKCR_ADDR			( ( volatile uint32_t __evenaccess * )0x00080020 )
#define SYSTEM_SCKCR_PCLKD_SHIFT	( 0U )
#define SYSTEM_SCKCR_PCLKC_SHIFT	( 4U )
#define SYSTEM_SCKCR_PCLKB_SHIFT	( 8U )
#define SYSTEM_SCKCR_PCLKA_SHIFT	( 12U )
#define SYSTEM_SCKCR_BCK_SHIFT		( 16U )
#define SYSTEM_SCKCR_PSTOP0_SHIFT	( 22U )
#define SYSTEM_SCKCR_PSTOP1_SHIFT	( 23U )
#define SYSTEM_SCKCR_ICK_SHIFT		( 24U )
#define SYSTEM_SCKCR_FCK_SHIFT		( 28U )
#define SYSTEM_SCKCR2_ADDR			( ( volatile uint16_t __evenaccess * )0x00080024 )
#define SYSTEM_SCKCR2_UCK_SHIFT		( 4U )
#define SYSTEM_SCKCR3_ADDR			( ( volatile uint16_t __evenaccess * )0x00080026 )
#define SYSTEM_SCKCR3_CKSEL_SHIFT	( 8U )
#define SYSTEM_MSTPCRA_ADDR			( ( volatile uint32_t __evenaccess * )0x00080010 )
#define SYSTEM_MSTPCRA_MSTPA0_BIT	( 1UL << 0U )
#define SYSTEM_MSTPCRA_MSTPA1_BIT	( 1UL << 1U )
#define SYSTEM_MSTPCRB_ADDR			( ( volatile uint32_t __evenaccess * )0x00080014 )
#define SYSTEM_MSTPCRB_MSTPB15_BIT	( 1UL << 15U )
#define SYSTEM_MSTPCRB_MSTPB25_BIT	( 1UL << 25U )
#define SYSTEM_MSTPCRB_MSTPB28_BIT	( 1UL << 28U )
#define SYSTEM_MSTPCRB_MSTPB30_BIT	( 1UL << 30U )
#define SYSTEM_MSTPCRC_ADDR			( ( volatile uint32_t __evenaccess * )0x00080018 )
#define SYSTEM_MSTPCRC_MSTPC27_BIT	( 1UL << 27U )
#define SYSTEM_ROMWT_ADDR			( ( volatile uint8_t __evenaccess * )0x0008101C )

/*
 *  FLASHモジュール
 */
#define FLASH_ROMCE_ADDR			( ( volatile uint16_t __evenaccess * )0x00081000 )
#define FLASH_ROMCIV_ADDR			( ( volatile uint16_t __evenaccess * )0x00081004 )

/*
 *  CMTWモジュール
 */
#define CMTW0_CMWCR_ADDR			( ( volatile uint16_t __evenaccess * )0x00094204 )
#define CMTW1_CMWCR_ADDR			( ( volatile uint16_t __evenaccess * )0x00094284 )
#define CMTWn_CMWCR_CKS_SHIFT		( 0U )
#define CMTWn_CMWSTR_CMWIE_BIT		( 1UL << 3U )
#define CMTWn_CMWCR_CCLR_SHIFT		( 13U )
#define CMTW0_CMWSTR_ADDR			( ( volatile uint16_t __evenaccess * )0x00094200 )
#define CMTW1_CMWSTR_ADDR			( ( volatile uint16_t __evenaccess * )0x00094280 )
#define CMTWn_CMWSTR_STR_BIT		( 1UL << 0U )
#define CMTW0_CMWCNT_ADDR			( ( volatile uint32_t __evenaccess * )0x00094210 )
#define CMTW1_CMWCNT_ADDR			( ( volatile uint32_t __evenaccess * )0x00094290 )
#define CMTW0_CMWCOR_ADDR			( ( volatile uint32_t __evenaccess * )0x00094214 )
#define CMTW1_CMWCOR_ADDR			( ( volatile uint32_t __evenaccess * )0x00094294 )
#define CMTW0_CMWIOR_ADDR			( ( volatile uint16_t __evenaccess * )0x00094208 )
#define CMTW1_CMWIOR_ADDR			( ( volatile uint16_t __evenaccess * )0x00094288 )
#define CMTWn_CMWIOR_CMWE_BIT		( 1UL << 15U )

/*
 *  ICUモジュール
 */
#define ICU_IRn_ADDR(no)		( ( volatile uint8_t __evenaccess * )(0x00087010+(no)-16) )
#define ICU_IERm_ADDR(no)		( ( volatile uint8_t __evenaccess * )(0x00087202+(no)-2) )
#define ICU_SWINTR_ADDR			( ( volatile uint8_t __evenaccess * )0x000872E0 )
#define ICU_SWINT2R_ADDR		( ( volatile uint8_t __evenaccess * )0x000872E1 )
#define ICU_IPRr_ADDR(no)		( ( volatile uint8_t __evenaccess * )(0x00087300+(no)) )
#define ICU_DTCERn_ADDR(no)		( ( volatile uint8_t __evenaccess * )(0x0008711A+(no)-26) )
#define ICU_IRQCRn_ADDR(no)		( ( volatile uint8_t __evenaccess * )(0x00087500+(no)) )
#define ICU_IRQCRn_IRQMD_SHIFT	( 2U )
#define ICU_GRPBL0_ADDR			( ( volatile uint32_t __evenaccess * )(0x00087630) )
#define ICU_GENBL0_ADDR			( ( volatile uint32_t __evenaccess * )(0x00087670) )
#define ICU_GRPBL1_ADDR			( ( volatile uint32_t __evenaccess * )(0x00087634) )
#define ICU_GENBL1_ADDR			( ( volatile uint32_t __evenaccess * )(0x00087674) )
#define ICU_SLIBXR135_ADDR		( ( volatile uint32_t __evenaccess * )(0x00087787) )
#define ICU_GRPAL1_ADDR			( ( volatile uint32_t __evenaccess * )(0x00087834) )
#define ICU_GRPAL1_EDMAC0_EINT0	( 1UL << 4U )
#define ICU_GENAL1_ADDR			( ( volatile uint32_t __evenaccess * )(0x00087874) )
#define ICU_GENAL1_EDMAC0_EINT0	( 1UL << 4U )

/*
 *  SCIモジュール
 */
#define SCI0_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A005 )
#define SCI0_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A003 )
#define SCI0_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A000 )
#define SCI0_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A002 )
#define SCI1_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A025 )
#define SCI1_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A023 )
#define SCI1_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A020 )
#define SCI1_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A022 )
#define SCI2_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A045 )
#define SCI2_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A043 )
#define SCI2_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A040 )
#define SCI2_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A042 )
#define SCI3_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A065 )
#define SCI3_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A063 )
#define SCI3_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A060 )
#define SCI3_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A062 )
#define SCI4_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A085 )
#define SCI4_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A083 )
#define SCI4_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A080 )
#define SCI4_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A082 )
#define SCI5_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A5 )
#define SCI5_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A3 )
#define SCI5_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A0 )
#define SCI5_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A2 )
#define SCI6_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C5 )
#define SCI6_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C3 )
#define SCI6_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C0 )
#define SCI6_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C2 )
#define SCI7_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E5 )
#define SCI7_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E3 )
#define SCI7_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E0 )
#define SCI7_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E2 )
#define SCI8_RDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A105 )
#define SCI8_TDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A103 )
#define SCI8_SMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A100 )
#define SCI8_SCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A102 )
#define SCIn_SCR_TEIE_BIT		( 1UL << 2U )
#define SCIn_SCR_RE_BIT			( 1UL << 4U )
#define SCIn_SCR_TE_BIT			( 1UL << 5U )
#define SCIn_SCR_RIE_BIT		( 1UL << 6U )
#define SCIn_SCR_TIE_BIT		( 1UL << 7U )
#define SCI0_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A004 )
#define SCI1_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A024 )
#define SCI2_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A044 )
#define SCI3_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A064 )
#define SCI4_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A084 )
#define SCI5_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A4 )
#define SCI6_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C4 )
#define SCI7_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E4 )
#define SCI8_SSR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A104 )
#define SCIn_SSR_TEND_BIT		( 1UL << 2U )
#define SCIn_SSR_RDRF_BIT		( 1UL << 6U )
#define SCIn_SSR_TDRE_BIT		( 1UL << 7U )
#define SCI0_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A006 )
#define SCI0_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A001 )
#define SCI0_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A007 )
#define SCI1_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A026 )
#define SCI1_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A021 )
#define SCI1_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A027 )
#define SCI2_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A046 )
#define SCI2_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A041 )
#define SCI2_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A047 )
#define SCI3_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A066 )
#define SCI3_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A061 )
#define SCI3_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A067 )
#define SCI4_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A086 )
#define SCI4_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A081 )
#define SCI4_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A087 )
#define SCI5_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A6 )
#define SCI5_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A1 )
#define SCI5_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0A7 )
#define SCI6_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C6 )
#define SCI6_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C1 )
#define SCI6_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0C7 )
#define SCI7_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E6 )
#define SCI7_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E1 )
#define SCI7_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A0E7 )
#define SCI8_SCMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A106 )
#define SCI8_BRR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A101 )
#define SCI8_SEMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008A107 )

/*
 *  MPCモジュール
 */
#define MPC_PFENET_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C10E )
#define MPC_PFENET_PHYMODE0_BIT	( 1UL << 4U )
#define MPC_PWPR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C11F )
#define MPC_PWPR_PFSWE_BIT		( 1UL << 6U )
#define MPC_PWPR_B0WI_BIT		( 1UL << 7U )
#define MPC_P23PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C153 )
#define MPC_P25PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C155 )
#define MPC_P26PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C156 )
#define MPC_P30PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C158 )
#define MPC_PA3PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C193 )
#define MPC_PA4PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C194 )
#define MPC_PA5PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C195 )
#define MPC_PB0PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C198 )
#define MPC_PB1PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C199 )
#define MPC_PB2PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C19A )
#define MPC_PB3PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C19B )
#define MPC_PB4PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C19C )
#define MPC_PB5PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C19D )
#define MPC_PB6PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C19E )
#define MPC_PB7PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C19F )
#define MPC_PF0PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C1B8 )
#define MPC_PF1PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C1B9 )
#define MPC_PF2PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C1BA )
#define MPC_PJ1PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C1D1 )
#define MPC_PJ2PFS_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C1D2 )

/*
 *  PORTモジュール
 */
#define PORT2_PDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C002 )
#define PORT3_PDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C003 )
#define PORTF_PDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C00F )
#define PORTJ_PDR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C012 )
#define PORTn_PDR_B0_BIT		( 1UL << 0U )
#define PORTn_PDR_B1_BIT		( 1UL << 1U )
#define PORTn_PDR_B2_BIT		( 1UL << 2U )
#define PORTn_PDR_B3_BIT		( 1UL << 3U )
#define PORTn_PDR_B4_BIT		( 1UL << 4U )
#define PORTn_PDR_B5_BIT		( 1UL << 5U )
#define PORTn_PDR_B6_BIT		( 1UL << 6U )
#define PORTn_PDR_B7_BIT		( 1UL << 7U )
#define PORT2_PMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C062 )
#define PORT3_PMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C063 )
#define PORTA_PMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C06A )
#define PORTB_PMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C06B )
#define PORTF_PMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C06F )
#define PORTJ_PMR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C072 )

#define PORT2_PODR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C022 )
#define PORT3_PODR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C023 )
#define PORTn_PODR_B0_BIT		( 1UL << 0U )
#define PORTn_PODR_B1_BIT		( 1UL << 1U )
#define PORTn_PODR_B2_BIT		( 1UL << 2U )
#define PORTn_PODR_B3_BIT		( 1UL << 3U )
#define PORTn_PODR_B4_BIT		( 1UL << 4U )
#define PORTn_PODR_B5_BIT		( 1UL << 5U )
#define PORTn_PODR_B6_BIT		( 1UL << 6U )
#define PORTn_PODR_B7_BIT		( 1UL << 7U )

#define PORT2_PCR_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C0C2 )

/*
 * リアルタイムクロック
 */
#define RTC_R64CNT_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C400 )
#define RTC_RSECCNT_ADDR		( ( volatile uint8_t __evenaccess * )0x0008C402 )
#define RTC_RMINCNT_ADDR		( ( volatile uint8_t __evenaccess * )0x0008C404 )
#define RTC_RHRCNT_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C406 )
#define RTC_RWKCNT_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C408 )
#define RTC_RDAYCNT_ADDR		( ( volatile uint8_t __evenaccess * )0x0008C40A )
#define RTC_RMONCNT_ADDR		( ( volatile uint8_t __evenaccess * )0x0008C40C )
#define RTC_RYRCNT_ADDR			( ( volatile uint16_t __evenaccess * )0x0008C40E )

#define RTC_RCR1_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C422 )
#define RTC_RCR1_CIE_BIT		( 1UL << 1U )
#define RTC_RCR2_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C424 )
#define RTC_RCR2_START_BIT		( 1UL << 0U )
#define RTC_RCR2_RESET_BIT		( 1UL << 1U )
#define RTC_RCR2_HR24_BIT		( 1UL << 6U )
#define RTC_RCR2_CNTMD_BIT		( 1UL << 7U )
#define RTC_RCR3_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C426 )
#define RTC_RCR3_RTCEN_BIT		( 1UL << 0U )
#define RTC_RCR4_ADDR			( ( volatile uint8_t __evenaccess * )0x0008C428 )
#define RTC_RCR4_RCKSEL_BIT		( 1UL << 0U )

/*
 *  MPUモジュール
 */
#define MPU_RSPAGEn_ADDR(no)	( ( volatile uint32_t __evenaccess * )(0x00086400+(no)*8) )
#define MPU_REPAGEn_ADDR(no)	( ( volatile uint32_t __evenaccess * )(0x00086404+(no)*8) )
#define MPU_MPEN_ADDR			( ( volatile uint32_t __evenaccess * )0x00086500 )
#define MPU_MPESTS_ADDR			( ( volatile uint32_t __evenaccess * )0x0008650C )
#define MPU_MPDEA_ADDR			( ( volatile uint32_t __evenaccess * )0x00086514 )

#endif	/* TOPPERS_RX65N_H */
