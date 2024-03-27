#include <nds32_intrinsic.h>
#include "WT58F2C9.h"
#include "gpio.h"


void delay1(unsigned int nCount)
{
   volatile unsigned int i;
   for (i=0;i<nCount;i++);
}


extern void DRV_Printf(char *pFmt, U16 u16Val);


inline void GIE_ENABLE();

void DRV_EnableHWInt (void)
{
	/* enable SW0, HW0 and HW1 */
	__nds32__mtsr(0x10003, NDS32_SR_INT_MASK);
	/* Enable SW0 */
	//__nds32__mtsr(0x10000, NDS32_SR_INT_MASK);
	/* Enable global interrupt */
	GIE_ENABLE();
}

void DRV_BlockIntDisable(void)
{
	// Disable all interrupt
	OUTW(rINT0_IE0_ENABLE, 0x0000);
	OUTW(rINT0_IE1_ENABLE, 0x0000);
	OUTW(rINT0_IE2_ENABLE, 0x0000);
	OUTW(rINT1_IE0_ENABLE, 0x0000);
	OUTW(rINT1_IE1_ENABLE, 0x0000);
	OUTW(rINT1_IE2_ENABLE, 0x0000);
}

void DRV_IntInitial(void)
{
	// Disable all interrupt
	DRV_BlockIntDisable();

	// Enable all HW interrupt
	DRV_EnableHWInt();				//Enable global Hardware interrupt, Assembly command

	// Enable default Block interrupt
	//DRV_BlockIntEnable();			//Enable each block device interrupt

}

#define rSYS_OPTION1	(0x00200004)

void DRV_SysXtal(U8 u8XtalMode)
{

	#if(EXTERNAL_XTAL == XTAL_MODE)
		//-----External Crystal
		//-----24MHz
		OUTW(rSYS_OPTION1,((INW(rSYS_OPTION1)&0xFFFFFF00) | 0x0012)); //Use HXTAL and divide 2
		//-----Crystal 12MHz
		//OUTW(rSYS_OPTION1, (INW(rSYS_OPTION1) | 0x000A));

		//OUTW(rSYS_OPTION1,(INW(rSYS_OPTION1) | (XTAL<<2) | (HSE_OSC_ON <<1)));
		//OUTW(rSYS_CLOCK_SELECT,(INW(rSYS_CLOCK_SELECT) | 0x0001));
	#else
		//-----Internal RC
		//-----24MHz
		//OUTW(rSYS_OPTION1,(INW(rSYS_OPTION1) | 0x0012));
		//-----Crystal 12MHz
		//OUTW(rSYS_OPTION1, (INW(rSYS_OPTION1) | 0x000A));

		//OUTW(rSYS_OPTION1,(INW(rSYS_OPTION1) | (XTAL<<2)));
		//OUTW(rSYS_CLOCK_SELECT,(INW(rSYS_CLOCK_SELECT));
	#endif
	//-----MCU Clock Output Test
	//OUTW(rSYS_OPTION3,0x0090);
}

void OS_PowerOnDriverInitial(void)
{
	//========================= << Typedef Initial  >>
	//SYS_TypeDefInitial();
	//========================= << System Clock Initial >>
	//-----External Crystal
	DRV_SysXtal(EXTERNAL_XTAL);
	//========================= << Interrupt Initial >>
	DRV_IntInitial();
	//========================= << GPIO Initial >>
	//DRV_GpioInitial();
	//========================= << UART Initial >>
	//DRV_UartInitial();
	//========================= << Timer Initial >>
	//DRV_TimerInitial(TIMER_0, SIMPLE_TIMER);
	//DRV_TimerInitial(TIMER_1, SIMPLE_TIMER);
	//========================= << PWM Initial >>
	//DRV_PwmInitial();
	//========================= << SPI Initial >>
	//DRV_SpiInitial(SPI_CH2);
	//========================= << Watchdog Initial >>
}


// int checkButtonState(int prev, int curr);

unsigned int index_7seg_num[17] = {Number_0, Number_1, Number_2, Number_3, Number_4, Number_5, Number_6, Number_7, Number_8, Number_9, Number_A, Number_b, Number_C, Number_d, Number_E, Number_F, Number_Dot};
unsigned int index_7seg[8] = {Digit_1, Digit_2, Digit_3, Digit_4, Digit_5, Digit_6, Digit_7, Digit_8};
unsigned int date[8] = {Number_2, Number_0, Number_2, Number_3, Number_0, Number_3, Number_2, Number_3};
unsigned int show_H = 0x7676;

int main()
{
	OS_PowerOnDriverInitial();
	
	//LED control
	GPIO_PTB_DIR = 0x0000;
	GPIO_PTB_CFG = 0x0000;  //0xFFFF for LEDs
	GPIO_PTB_PADINSEL = 0x0000;
	unsigned int LED = 0x8000;  //leftmost
	GPIO_PTB_GPIO = ~LED;

	GPIO_PTA_DIR = 0x0F00;
	GPIO_PTA_CFG = 0x0000;

	GPIO_PTD_DIR = 0x0000;
	GPIO_PTD_CFG = 0x0000;

	while(1){
		unsigned int button_state = (GPIO_PTA_PADIN & 0x00000F00) >> 8;
		delay1(100000);
		//Date 0323
		if(button_state == 0x0000000D){  //SW20 is pressed (1101)
			int i = 0;
			while(1){
				int current_state = (GPIO_PTA_PADIN & 0x00000F00) >> 8;
				delay1(1000);
				if(current_state != 0x0000000F){
					GPIO_PTD_GPIO = 0x0000;  //Show none
					break;
				}
				for(i = 0; i < 8; i++){
					GPIO_PTA_GPIO = index_7seg[7 - i];  //Select digit
					GPIO_PTD_GPIO = date[i];  //Show date
					delay1(2000);
				}
			}
		}
		//HHHHHHHH
		else if(button_state == 0x0000000E){  //SW21 is pressed (1110)
			int i = 0;
			int toshowornot = 0;
			while(1){
				int current_state = (GPIO_PTA_PADIN & 0x00000F00) >> 8;
				delay1(1000);
				if(current_state != 0x0000000F){
					GPIO_PTD_GPIO = 0x0000;  //Show none
					break;
				}
				for(i = 0; i < 4; i++){
					GPIO_PTA_GPIO = index_7seg[7 - i];  //Select digit
					GPIO_PTD_GPIO = show_H;  //Show Hs
					delay1(2000);
				}
				if(toshowornot % 10 == 0){
					for(i = 0; i < 4; i++){
						GPIO_PTA_GPIO = index_7seg[3 - i];  //Select digit
						GPIO_PTD_GPIO = show_H;  //Show Hs
						delay1(2000);
					}
				}
				toshowornot++;
			}
		}
		//LED go right
		else if(button_state == 0x00000007){  //SW18 is pressed (0111) 
			GPIO_PTB_CFG = 0xFFFF;
			if(LED != 0x8000){  //Can move
				LED <<= 1;
				GPIO_PTB_GPIO = ~LED;
			}
			GPIO_PTB_CFG = 0x0000;
		}
		//LED go left
		else if(button_state == 0x0000000B){  //SW19 is pressed (1011) 
			GPIO_PTB_CFG = 0xFFFF;
			if(LED != 0x0001){  //Can move
				LED >>= 1;
				GPIO_PTB_GPIO = ~LED;
			}
			GPIO_PTB_CFG = 0x0000;
		}
	}

	// while(1){
	// 	if(((GPIO_PTA_PADIN & 0x00000F00) >> 8) != 0x0000000F){  //At least one button is pressed
	// 		int i = 0;
	// 		for(i = 0; i < 4; i++){
	// 			GPIO_PTA_GPIO = index_7seg[3 - i];	//Select digit
	// 			GPIO_PTD_GPIO = date[i];	//Show ID digit
	// 			delay1(2000);
	// 		}
	// 	}
	// }

	return 0;
}

// int checkButtonState(int prev, int curr){
// 	if(prev == curr){
// 		return 1;
// 	}
// 	else{
// 		return 0;
// 	}
// }

