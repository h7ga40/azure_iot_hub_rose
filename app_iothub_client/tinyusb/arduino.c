
#include <kernel.h>
#include "arduino.h"
#include "iodefine.h"
#include "tusb.h"

#define IRQ_PRIORITY_USBI0    6

#define MPC_PFS_ISEL          (1<<6)

#define IRQ_USB0_USBI0        62
#define SLIBR_USBI0           SLIBR185
#define IR_USB0_USBI0         IR_PERIB_INTB185
#define IPR_USB0_USBI0        IPR_PERIB_INTB185
#define INT_Excep_USB0_USBI0  INT_Excep_PERIB_INTB185

void board_init(void)
{
	/* setup software configurable interrupts */
	ICU.SLIBR_USBI0.BYTE = IRQ_USB0_USBI0;
	ICU.SLIPRCR.BYTE = 1;

	/* Unlock MPC registers */
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;

	/* USB VBUS -> P16 */
	PORT1.PMR.BIT.B6 = 1U;
	MPC.P16PFS.BYTE = MPC_PFS_ISEL | 0b10001;
	/* Lock MPC registers */
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	/* setup USBI0 interrupt. */
	IR(USB0, USBI0) = 0;
	IPR(USB0, USBI0) = IRQ_PRIORITY_USBI0;

	ena_int(185);
}

unsigned long millis(void)
{
	SYSTIM tm;
	get_tim(&tm);
	return (unsigned long)(tm / 1000);
}

unsigned long micros(void)
{
	SYSTIM tm;
	get_tim(&tm);
	return (unsigned long)(tm);
}

void delay(unsigned long ms)
{
	dly_tsk((RELTIM)ms * 1000);
}

void delayMicroseconds(unsigned int us)
{
	dly_tsk(us);
}

void pinMode(int pin, int mode)
{
	switch (pin) {
	case 2:
		if (mode == OUTPUT) {
			PORT1.PODR.BIT.B2 = 0U;
			PORT1.PMR.BIT.B2 = 0U;
			PORT1.PDR.BIT.B2 = 1U;
		}
		else {
			PORT1.PMR.BIT.B2 = 0U;
			PORT1.PDR.BIT.B2 = 0U;
			PORT1.PCR.BIT.B2 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 6:
		if (mode == OUTPUT) {
			PORT3.PODR.BIT.B3 = 0U;
			PORT3.PMR.BIT.B3 = 0U;
			PORT3.PDR.BIT.B3 = 1U;
		}
		else {
			PORT3.PMR.BIT.B3 = 0U;
			PORT3.PDR.BIT.B3 = 0U;
			PORT3.PCR.BIT.B3 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 8:
		if (mode == OUTPUT) {
			PORT3.PODR.BIT.B0 = 0U;
			PORT3.PMR.BIT.B0 = 0U;
			PORT3.PDR.BIT.B0 = 1U;
		}
		else {
			PORT3.PMR.BIT.B0 = 0U;
			PORT3.PDR.BIT.B0 = 0U;
			PORT3.PCR.BIT.B0 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 9:
		if (mode == OUTPUT) {
			PORT2.PODR.BIT.B6 = 0U;
			PORT2.PMR.BIT.B6 = 0U;
			PORT2.PDR.BIT.B6 = 1U;
		}
		else {
			PORT2.PMR.BIT.B6 = 0U;
			PORT2.PDR.BIT.B6 = 0U;
			PORT2.PCR.BIT.B6 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 10:
		if (mode == OUTPUT) {
			PORTE.PODR.BIT.B4 = 0U;
			PORTE.PMR.BIT.B4 = 0U;
			PORTE.PDR.BIT.B4 = 1U;
		}
		else {
			PORTE.PMR.BIT.B4 = 0U;
			PORTE.PDR.BIT.B4 = 0U;
			PORTE.PCR.BIT.B4 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 11:
		if (mode == OUTPUT) {
			PORTE.PODR.BIT.B6 = 0U;
			PORTE.PMR.BIT.B6 = 0U;
			PORTE.PDR.BIT.B6 = 1U;
		}
		else {
			PORTE.PMR.BIT.B6 = 0U;
			PORTE.PDR.BIT.B6 = 0U;
			PORTE.PCR.BIT.B6 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 12:
		if (mode == OUTPUT) {
			PORTE.PODR.BIT.B7 = 0U;
			PORTE.PMR.BIT.B7 = 0U;
			PORTE.PDR.BIT.B7 = 1U;
		}
		else {
			PORTE.PMR.BIT.B7 = 0U;
			PORTE.PDR.BIT.B7 = 0U;
			PORTE.PCR.BIT.B7 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 13:
		if (mode == OUTPUT) {
			PORTE.PODR.BIT.B5 = 0U;
			PORTE.PMR.BIT.B5 = 0U;
			PORTE.PDR.BIT.B5 = 1U;
		}
		else {
			PORTE.PMR.BIT.B5 = 0U;
			PORTE.PDR.BIT.B5 = 0U;
			PORTE.PCR.BIT.B5 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 20:
		if (mode == OUTPUT) {
			PORT5.PODR.BIT.B2 = 0U;
			PORT5.PMR.BIT.B2 = 0U;
			PORT5.PDR.BIT.B2 = 1U;
		}
		else {
			PORT5.PMR.BIT.B2 = 0U;
			PORT5.PDR.BIT.B2 = 0U;
			PORT5.PCR.BIT.B2 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	case 21:
		if (mode == OUTPUT) {
			PORT5.PODR.BIT.B0 = 0U;
			PORT5.PMR.BIT.B0 = 0U;
			PORT5.PDR.BIT.B0 = 1U;
		}
		else {
			PORT5.PMR.BIT.B0 = 0U;
			PORT5.PDR.BIT.B0 = 0U;
			PORT5.PCR.BIT.B0 = (mode == INPUT_PULLUP) ? 1 : 0;
		}
		break;
	}
}

void digitalWrite(int pin, int value)
{
	switch (pin) {
	case 2:
		PORT1.PODR.BIT.B2 = value;
		break;
	case 6:
		PORT3.PODR.BIT.B3 = value;
		break;
	case 8:
		PORT3.PODR.BIT.B0 = value;
		break;
	case 9:
		PORT2.PODR.BIT.B6 = value;
		break;
	case 10:
		PORTE.PODR.BIT.B4 = value;
		break;
	case 11:
		PORTE.PODR.BIT.B6 = value;
		break;
	case 12:
		PORTE.PODR.BIT.B7 = value;
		break;
	case 13:
		PORTE.PODR.BIT.B5 = value;
		break;
	case 20:
		PORT5.PODR.BIT.B2 = value;
		break;
	case 21:
		PORT5.PODR.BIT.B0 = value;
		break;
	}
}

int digitalRead(int pin)
{
	switch (pin) {
	case 2:
		return PORT1.PIDR.BIT.B2;
	case 6:
		return PORT3.PIDR.BIT.B3;
	case 8:
		return PORT3.PIDR.BIT.B0;
	case 9:
		return PORT2.PIDR.BIT.B6;
	case 10:
		return PORTE.PIDR.BIT.B4;
	case 11:
		return PORTE.PIDR.BIT.B6;
	case 12:
		return PORTE.PIDR.BIT.B7;
	case 13:
		return PORTE.PIDR.BIT.B5;
	case 20:
		return PORT5.PIDR.BIT.B2;
	case 21:
		return PORT5.PIDR.BIT.B0;
	}
	return 0;
}

void (*irq2_user_func)(void);
void (*irq3_user_func)(void);

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode)
{
	int irqno = 0;

	switch (interruptNum) {
	case 2:
		irqno = 2;
		break;
	case 6:
		irqno = 3;
		break;
	default:
		return;
	}

	unsigned char irqmd;
	switch (mode)
	{
	case LOW:
		irqmd = 0;
		break;
	case FALLING:
		irqmd = 1;
		break;
	case RISING:
		irqmd = 2;
		break;
	case CHANGE:
		irqmd = 3;
		break;
	default:
		return;
	}

	dis_int(VECT_ICU_IRQ1 + irqno - 1);

	/* Unlock MPC registers */
	MPC.PWPR.BIT.B0WI = 0;
	MPC.PWPR.BIT.PFSWE = 1;
	switch (interruptNum) {
	case 2:
		irq2_user_func = userFunc;
		pinMode(2, INPUT_PULLUP);
		if (userFunc) {
			/* set IRQ2 */
			MPC.P12PFS.BIT.ISEL = 1;
			MPC.P12PFS.BIT.PSEL = 0;
			ICU.IRQCR[irqno].BIT.IRQMD = irqmd;
		}
		else {
			/* set GPIO */
			MPC.P12PFS.BYTE = 0;
		}
		break;
	case 6:
		irq3_user_func = userFunc;
		pinMode(6, INPUT_PULLUP);
		if (userFunc) {
			/* set IRQ3 */
			MPC.P33PFS.BIT.ISEL = 1;
			MPC.P33PFS.BIT.PSEL = 0;
			ICU.IRQCR[irqno].BIT.IRQMD = irqmd;
		}
		else {
			/* set GPIO */
			MPC.P33PFS.BYTE = 0;
		}
		break;
	}
	/* Lock MPC registers */
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	if (userFunc) {
		ena_int(VECT_ICU_IRQ1 + irqno - 1);
	}
}

void usb0_handler(EXINF exinf)
{
	tud_int_handler(0);
}

void irq2_handler(EXINF exinf)
{
	if (irq2_user_func)
		irq2_user_func();
}

void irq3_handler(EXINF exinf)
{
	if (irq3_user_func)
		irq3_user_func();
}
