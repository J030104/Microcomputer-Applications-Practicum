#include <nds32_intrinsic.h>
#include <nds32_isr.h>
#include "WT58F2C9.h"
#include "gpio.h"

extern void DRV_Printf(char *pFmt, U16 u16Val);

void nmi_handler(int *reg_ptr)
{
}

inline void GIE_ENABLE()
{
	__nds32__setgie_en();
	__nds32__dsb();
}

inline void GIE_DISABLE()
{
	__nds32__setgie_dis();
	__nds32__dsb();
}

 /* this function generates a s/w interrupt */
void gen_swi()
{
	unsigned int int_pend;

	int_pend = __nds32__mfsr(NDS32_SR_INT_PEND);
	int_pend |= 0x10000;
	__nds32__mtsr(int_pend, NDS32_SR_INT_PEND);
	__nds32__dsb();
}

void initIntr_standby()
{
	/* enable HW1 */
	__nds32__mtsr(0x0002, NDS32_SR_INT_MASK);
	//intc_reset();
	/* Init GPIO */

}

void clear_swi()
{
	unsigned int int_pend;

	int_pend = 0x10;
	__nds32__mtsr(int_pend, NDS32_SR_INT_PEND);
	__nds32__dsb();
}

void __cpu_init();
void __c_init();
void __soc_init();
int main();

/*
 * Interrupt vectors
 * vectors: 7
 * HW0 ~ HW5 and SW0 : IVIC version (IVB.IVIC_VER) = 0
 * HW0 ~ HW31        : IVIC version (IVB.IVIC_VER) = 1
 * */
void NDS32ATTR_RESET("vectors=7;nmi_func=nmi_handler;warm_func=nmi_handler") reset_handler(void)
{
	__cpu_init();
	__c_init();
	__soc_init();
	main();
	DRV_Printf("===== reset_handler end =====\r\n", 0);
}

/*
 * system call handler
 * ready_nested for user to enable GIE when they need.
 * */
void NDS32ATTR_EXCEPT("save_all_regs;ready_nested;id=8") syscall_handler(int vid, NDS32_CONTEXT * ptr)
{
	//static int cnt = 0;

	//if (++cnt < 5)
	//	asm("syscall 0x5000\n\t");

	//puts("Syscall handler\n");
	//ptr->ipc = ptr->ipc + 4;
}
U16 count = 0;
/*
 * HW0 interrupt handler
 * not_nested for top priority interrupt 
 * */
void NDS32ATTR_ISR("not_nested;id=0") HW0_ISR(int vid, NDS32_CONTEXT * ptr)
{
	unsigned short int u16IntFlag1;

	GPIO_PTB_GPIO = 0X0000;
	// delay1(6666);

	u16IntFlag1 = (INW(rINT0_IE1_FLAG) & INW(rINT0_IE1_ENABLE));

	if(u16IntFlag1 & 0x0200)
	{
		//-----Clear H/W IR interrupt
		OUTW(rIR_CTL, INW(rIR_CTL) | 0x02);
		count++;
	}

}

/*
 * HW1 interrupt handler
 * ready_nested for user to enable GIE when they need.
 * */
void NDS32ATTR_ISR("ready_nested;id=1") HW1_ISR(int vid, NDS32_CONTEXT * ptr)
{
	unsigned int msk = (1 << vid);

	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	DRV_Printf("===== HW1_ISR =====\r\n", 0);

	/* Unmask HW interrupt vector */
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}
/*
 * SW0 interrupt handler
 * not_nested for software interrupt
 * */
void NDS32ATTR_ISR("not_nested;id=6") SWI_ISR(int vid)
{
	DRV_Printf("===== A software interrupt occurs ... =====\r\n", 0);
	clear_swi();
	return;
}
