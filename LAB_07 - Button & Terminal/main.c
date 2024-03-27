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

//-----  LCM Functions  -----
#define RS	0x0400
#define E	0x0200
#define RW	0x0100
#define CleanSet 0x0000

void CheckBusy(void) //BF ,CHECK BUSY FLAG
{
	unsigned short int i = 0x80;
	while(i&0x80)
	{
		GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
		GPIO_PTE_CFG = 0x0;

		GPIO_PTE_GPIO = (RW + E);
		delay1(500);

		GPIO_PTE_DIR = 0xFFFF; //Initialize GPIO_PTE input
		GPIO_PTE_CFG = 0x00000;

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
	WriteIns(0x38);  // FUNCTION SET
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
void InitialRTC(char year, char month, char week, char day, char hour, char min, char sec)
{
	OUTW(rRTC_CTL, 0x83);

	OUTW(rRTC_SEC, sec);
	OUTW(rRTC_MIN, min);
	OUTW(rRTC_HOUR, hour);
	OUTW(rRTC_DAY, day);
	OUTW(rRTC_WEEK, week);
	OUTW(rRTC_MONTH, month);
	OUTW(rRTC_YEAR, year);
}

#define To_ASCII 0x30

void DisplayTime(void)
{
	unsigned int year = INW(rRTC_YEAR), mon = INW(rRTC_MONTH), day = INW(rRTC_DAY);
	unsigned int hour = INW(rRTC_HOUR), min = INW(rRTC_MIN),   sec = INW(rRTC_SEC);

	unsigned char week_str[7][3] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	unsigned int  week = INW(rRTC_WEEK), i;

	WriteIns(0x80); //STN LCM,�Ĥ@�C
	WriteData('2');
	WriteData('0');
	WriteData(((year >> 4) & 0xF) + To_ASCII);
	WriteData((year & 0xF) + To_ASCII);
	WriteData('/');
	WriteData(((mon / 10) & 0xF) + To_ASCII);
	WriteData((mon % 10) + To_ASCII);
	WriteData('/');
	WriteData(((day >> 4) & 0xF) + To_ASCII);
	WriteData((day & 0xF) + To_ASCII);
	WriteData(' ');
	for (i=0; i<3; i++)
		WriteData(week_str[week][i]);

	WriteIns(0xC0); //STN LCM,�ĤG�C
	WriteData(' ');
	WriteData(' ');
	WriteData(((hour >> 4) & 0xF) + To_ASCII);
	WriteData((hour & 0xF) + To_ASCII);
	WriteData(':');
	WriteData(((min >> 4) & 0xF) + To_ASCII);
	WriteData((min & 0xF) + To_ASCII);
	WriteData(':');
	WriteData(((sec >> 4) & 0xF) + To_ASCII);
	WriteData((sec & 0xF) + To_ASCII);
}

// int setsec = 2;
// int nddigit = 0;
// int final = 0;

// const char year = 0x23;	//BCD
// const char month = 4;   	//Decimal
// const char week = 4;		//Decimal
// const char day = 0x13;  	//BCD
// const char hour = 0x15; 	//BCD
// const char min = 0x30;	//BCD
// const char sec = 0x00;	//BCD

void Input(int);
void ToTerminal(char []);

int inputtimes = 0;
char str[17] = {0};

int main()
{
	OS_PowerOnDriverInitial();

	GPIO_PTA_FS = 0x0000;

	GPIO_PTA_DIR = 0x0FF0;
	GPIO_PTA_CFG = 0x0000;
	GPIO_PTA_PADINSEL = 0x0000;

	GPIO_PTD_DIR = 0x0000;	//Output mode
	GPIO_PTD_CFG = 0x0000;
	GPIO_PTD_PADINSEL = 0x0000;	//Digital R/W

	unsigned int tmp = 0;
	unsigned int col;
	unsigned int key;
	unsigned int keep = 0xFF;
	InitialLCD();	//Initial LCD Module

	while(1){
		key = 0xFF;
		for (col = 0; col < 4; col++){
			GPIO_PTA_BS = 0x000F;
			GPIO_PTA_BR = 0x0000 | (1 << col);
			tmp = ((~GPIO_PTA_PADIN) & 0xFF0) >> 4;
			if (tmp > 0){
				if (tmp & 0x1)
					key = 0*4 + col;
				else if (tmp & 0x2)
					key = 1*4 + col;
				else if (tmp & 0x4)
					key = 2*4 + col;
				else if (tmp & 0x8)
					key = 3*4 + col;
			}
		}

		if(keep != key){
			keep = key;
		}
		else{
			key = 0xFF;
		}
		
		if (key != 0xFF){
			if(inputtimes >= 16) break;
			if(key == 14){		//SW16 - Clear to 00
				if(inputtimes > 0){
					inputtimes--;
					str[inputtimes] = 0;
					WriteIns(0x38);  //FUNCTION SET
					WriteIns(0x0E);  //DISPLAY CONTROL
					WriteIns(0x06);  //SET INPUT MODE
					WriteIns(0x80 + inputtimes);  //1-LINE DD RAM SET Address
					WriteData(' ');
				}
			}
			else if(key == 15){		//SW17 - Confirm and start counting
				str[inputtimes] = '\0';
				break;
			}
			else if((key >= 0) && (key <= 9)){		//SW 2~11 Add
				str[inputtimes] = key + 0x30;
				Input(key);
			}
			else if(key == 10){
				str[inputtimes] = 'A';
				Input('A');	//A
			}
			else if(key == 11){
				str[inputtimes] = 'B';
				Input('B');		//B
			}
			else if(key == 12){
				str[inputtimes] = 'C';
				Input('C');	//C
			}
			else if(key == 13){
				str[inputtimes] = 'D';
				Input('D');	//D
			}
		}
	}

	ToTerminal(str);

}

void Input(int n){
	if(inputtimes < 16){
		WriteIns(0x38);  //FUNCTION SET
		WriteIns(0x0E);  //DISPLAY CONTROL
		WriteIns(0x06);  //SET INPUT MODE
		WriteIns(0x80 + inputtimes);  //1-LINE DD RAM SET Address
		if((n >= 0) && (n <= 9)){
			char c = n + 0x30;
			WriteData(c);
			inputtimes++;
		}
		else{
			WriteData(n);
			inputtimes++;
		}
	}
	else{
		return;
	}
}

void ToTerminal(char str[]){
	int i = 0; 
	for(i = 0; str[i] != '\0'; i++){
		DRV_Printf("%c", str[i]);
	}
	DRV_Printf("%c\n\r", 0);
}
