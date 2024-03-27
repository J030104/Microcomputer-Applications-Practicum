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


void Init_IR_Int(void)
{
	OUTW(rINT0_IE1_ENABLE, INW(rINT0_IE1_ENABLE) | 0x0200);

	OUTW(rIR_CTL, 0xD3);
}

#define RS	0x0400
#define E	0x0200
#define RW	0x0100
#define CleanSet 0x0000

void CheckBusy(void) //BF, CHECK BUSY FLAG
{
	unsigned short int i=0x80;
	while(i&0x80)
	{
		GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
		GPIO_PTE_CFG = 0x0000;

		GPIO_PTE_GPIO = (RW + E);
		delay1(500);

		GPIO_PTE_DIR = 0xFFFF; //Initialize GPIO_PTE input
		GPIO_PTE_CFG = 0x0000;

		delay1(100);
		i = GPIO_PTE_PADIN;
		delay1(100);

		GPIO_PTE_GPIO = CleanSet;
		delay1(1000);
	}
}

void WriteData(unsigned char i)
{
	GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
	GPIO_PTE_CFG = 0x0000;

	GPIO_PTE_GPIO = (i + RS + E);
	delay1(100);
	GPIO_PTE_GPIO = CleanSet;
	delay1(100);
	CheckBusy();
}

void WriteIns(unsigned char instruction)
{
	GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
	GPIO_PTE_CFG = 0x0000;

	GPIO_PTE_GPIO = (instruction + E);
	delay1(100);
	GPIO_PTE_GPIO = CleanSet;
	delay1(100);

	CheckBusy();
}

void InitialLCD(void)
{
	WriteIns(0x38);  //FUNCTION SET
	WriteIns(0x38);
	WriteIns(0x38);
	WriteIns(0x38);
	WriteIns(0x08);  // off display
	delay1(300000);
	WriteIns(0x01);  // clear buffer
	delay1(300000);
	WriteIns(0x0E);  // on display
	delay1(300000);
	WriteIns(0x06);  // set input mode
	delay1(300000);
}


/* RTC memory address */
#define RTC_MEM_ADDR_BASE		(0x00200400)
#define rRTC_CTL				(RTC_MEM_ADDR_BASE+0x00)
#define rRTC_SEC				(RTC_MEM_ADDR_BASE+0x04)
#define rRTC_MIN				(RTC_MEM_ADDR_BASE+0x08)
#define rRTC_HOUR				(RTC_MEM_ADDR_BASE+0x0C)
#define rRTC_DAY				(RTC_MEM_ADDR_BASE+0x10)
#define rRTC_WEEK				(RTC_MEM_ADDR_BASE+0x14)
#define rRTC_MONTH				(RTC_MEM_ADDR_BASE+0x18)
#define rRTC_YEAR				(RTC_MEM_ADDR_BASE+0x1C)
#define rRTC_MIN_ALARM			(RTC_MEM_ADDR_BASE+0x24)
#define rRTC_HOUR_ALARM			(RTC_MEM_ADDR_BASE+0x28)
#define rRTC_DAY_ALARM			(RTC_MEM_ADDR_BASE+0x2C)
#define rRTC_WEEK_ALARM			(RTC_MEM_ADDR_BASE+0x30)
#define rRTC_MONTH_ALARM		(RTC_MEM_ADDR_BASE+0x34)

//-----  RTC Functions  -----
#define To_ASCII 0x30
void InitialRTC(char hour, char min, char sec)
{
	OUTW(rRTC_CTL, 0x83);

	OUTW(rRTC_SEC,  sec);
	OUTW(rRTC_MIN,  min);
	OUTW(rRTC_HOUR, hour);
}

int main()
{
	OS_PowerOnDriverInitial();
	Init_IR_Int();
	GPIO_PTB_DIR = 0x0000; 
	GPIO_PTB_CFG = 0xFFFF;
	GPIO_PTB_PADINSEL = 0x0000;
	
	int number = 65530;
	char a = 'a';
	char string[] = "====================================\r\n";
	DRV_Printf(string, 0);
	DRV_Printf("   ADP-WT58F2C9 UART demo program   \r\n", 0);
	DRV_Printf("   Default baud rate: 38400 8-n-1\r\n", 0);
	DRV_Printf("====================================\r\n", 0);

	DRV_Printf("%d\r\n", number);
	DRV_Printf("%c\r\n", a);
	DRV_Printf("%x\r\n", a);

	int hour0, min0, sec0;
	hour0 = 0x09;
	min0 = 0x59;
	sec0 = 0x59;

	DRV_Printf("UART LAB\r\n", 0);
	InitialLCD();	//Initial LCD Module
	InitialRTC(hour0, min0, sec0); //Initial RTC Module

	int keep = 0;
	while(1){
		unsigned int hour = INW(rRTC_HOUR), min = INW(rRTC_MIN), sec = INW(rRTC_SEC);
		unsigned int h1 = ((hour >> 4) & 0xF) + To_ASCII;
		unsigned int h2 = (hour & 0xF) + To_ASCII;
		unsigned int m1 = ((min >> 4) & 0xF) + To_ASCII;
		unsigned int m2 = (min & 0xF) + To_ASCII;
		unsigned int s1 = ((sec >> 4) & 0xF) + To_ASCII;
		unsigned int s2 = (sec & 0xF) + To_ASCII;
		GPIO_PTB_GPIO = 0xFFFF;
		if(keep != s2){
			DRV_Printf("%c", h1);
			DRV_Printf("%c:", h2);
			DRV_Printf("%c", m1);
			DRV_Printf("%c:", m2);
			DRV_Printf("%c", s1);
			DRV_Printf("%c\r\n", s2);
			keep = s2;
		}
	}

	return 0;
}
