#ifndef TINYUSB_H
#define TINYUSB_H

#define	INT_IRQ2			66
#define	INT_IRQ3			67
#define INT_PERIB_INTB185	185

#define INH_USB0_USBI0		INT_PERIB_INTB185
#define INT_USB0_USBI0		INT_PERIB_INTB185
#define INTATR_USB0_USBI0	(TA_NULL)	/* 割込み属性	*/
#define INTPRI_USB0_USBI0	(-1)		/* 割込み優先度	*/

#define INH_ENC_A			INT_IRQ2
#define INT_ENC_A			INT_IRQ2
#define INTATR_ENC_A		(TA_NULL)	/* 割込み属性	*/
#define INTPRI_ENC_A		(-1)		/* 割込み優先度	*/

#define INH_ENC_B			INT_IRQ3
#define INT_ENC_B			INT_IRQ3
#define INTATR_ENC_B		(TA_NULL)	/* 割込み属性	*/
#define INTPRI_ENC_B		(-1)		/* 割込み優先度	*/

#define TINYUSB_PRIORITY	10		/* TinyUSBタスクの優先度 */
#define TINYUSB_STACK_SIZE	1024	/* TinyUSBタスクのスタック領域のサイズ */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  関数のプロトタイプ宣言
 */
extern void tinyusb_task(EXINF exinf);
extern void usb0_handler(EXINF exinf);
extern void irq2_handler(EXINF exinf);
extern void irq6_handler(EXINF exinf);

#endif /* TOPPERS_MACRO_ONLY */
#endif // TINYUSB_H
