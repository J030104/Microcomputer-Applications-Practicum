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
	unsigned short int i=0x80;
	while(i&0x80)
	{
		GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
		GPIO_PTE_CFG = 0x0;

		GPIO_PTE_GPIO = (RW + E);
		delay1(500);

		GPIO_PTE_DIR = 0xFF;  //Initialize GPIO_PTE input
		GPIO_PTE_CFG = 0x0;

		delay1(100);
		i = GPIO_PTE_PADIN;	  //Read busy flag
		delay1(100);

		GPIO_PTE_GPIO = CleanSet;	//Clean buffer
		delay1(1000);
	}
}

void WriteData(unsigned char i)
{
	GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
	GPIO_PTE_CFG = 0x0;

	GPIO_PTE_GPIO = (i + RS + E);	//Write data
	delay1(100);
	GPIO_PTE_GPIO = CleanSet;		//Clean buffer
	delay1(100);

	CheckBusy();
}

void WriteIns(unsigned char instruction)
{
	GPIO_PTE_DIR = 0x0000; //Initialize GPIO_PTE output
	GPIO_PTE_CFG = 0x0;

	GPIO_PTE_GPIO = (instruction + E); //Write instruction
	delay1(100);
	GPIO_PTE_GPIO = CleanSet;			//Clean buffer
	delay1(100);

	CheckBusy();
}

void InitialLCD(void)
{
	WriteIns(0x38);  // FUNCTION SET
	delay1(300000);
	WriteIns(0x01);  // clear buffer
	delay1(300000);
	WriteIns(0x0E);  // on display
	delay1(300000);
	WriteIns(0x06);  // set input mode
	delay1(300000);
}

void Display_1Line(unsigned int WordValue)
{
  char ABC[] = {'A', 'b', 'C', 'd', 'E', 'f', 'G', 'h'};
  char i = 0;
  WriteIns(0x38);  //FUNCTION SET
  WriteIns(0x0E);  //DISPLAY CONTROL
  WriteIns(0x06);  //SET INPUT MODE
  for(i = 0; i < WordValue; i++)
  {
    WriteData(ABC[i]);
    delay1(300000);
  }
}

void Display_2Line(int WordValue)
{
  char ABCD[] = "NTUST";
  char i;
  WriteIns(0x38);  //FUNCTION SET
  WriteIns(0x0E);  //DISPLAY CONTROL
  WriteIns(0x06);  //SET INPUT MODE

  WriteIns(0x80);  //1-LINE DD RAM SET Address
  for(i = 0; i < WordValue ; i++)
  {
    WriteData(ABCD[i]);
  }

  WriteIns(0xC0);  //2-LINE DD RAM SET Address
  for(i = 0; i < WordValue; i++)
  {
    WriteData(ABCD[i]);
  }
}

char NumAlpha[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int select = 0;
char OK[] = {'O', 'K'};
int OKcount = 0;

int main()
{
	unsigned int i;
	unsigned int tmp = 0;

	OS_PowerOnDriverInitial();

	InitialLCD();    //Initial LCD Module

	// WriteIns(0x80);  //1-LINE DD RAM SET Address
	// for(i = 0; i < 16; i++)
	// 	WriteData(255);	//Display BLOCKS

	// WriteIns(0xC0);  //2-LINE DD RAM SET Address
	// for(i = 0; i < 16; i++)
	// 	WriteData(255);	//Display BLOCKS
	// delay1(1000000); //Wait

	// WriteIns(0x01);  // clear buffer
	// Display_1Line(8);//Display 'A','b','C','d','E','f','G','h'
	// delay1(1000000); //Wait

	WriteIns(0x01);  // clear buffer
	Display_2Line(5);//Display "NTUST" (Two lines)
	delay1(1000000); //Wait

	GPIO_PTA_FS = 0x0000;

	GPIO_PTA_DIR = 0x0FF0;
	GPIO_PTA_CFG = 0x0000;
	GPIO_PTA_PADINSEL = 0x0000;

	unsigned int col;
	unsigned int key;
	unsigned int keep = 0xFF;

	//Show "NTUST" until SW17 is pressed
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
			switch(key){
				case 15:
					WriteIns(0x01);
					i = 1;
					break;
			}
		}
		if(i){
			break;
		}
	}

	//SW17 has been pressed
	//input StudentID
	
	WriteIns(0x38);  //FUNCTION SET
	WriteIns(0x0E);  //DISPLAY CONTROL
  	WriteIns(0x06);  //SET INPUT MODE
	WriteIns(0x80);
	WriteData(NumAlpha[0]);  //Write a 0 first

	unsigned int numberofinputs = 9;
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

		delay1(10000);
		if(keep != key){
			keep = key;
		}
		else{
			key = 0xFF;
		}
		
		if (key != 0xFF){
			switch(key){
				case 15:  //SW17 confirms current char and show next char if this is not the last one
					if(numberofinputs == 1){
						WriteIns(0x38);  //FUNCTION SET
						WriteIns(0x0E);  //DISPLAY CONTROL
  						WriteIns(0x06);  //SET INPUT MODE
						WriteIns(0xC0);  //2-LINE DD RAM SET Address
						for(OKcount = 0; OKcount < 2; OKcount++){
							WriteData(OK[OKcount]);
						}
						break;
					}
					select = 0;
					numberofinputs--;
					WriteIns(0x80 + (9 - numberofinputs));
					WriteData(NumAlpha[0]);  //Write the next 0
					break;

				case 14:  //SW16 deletes last char
					WriteIns(0x80 + (9 - numberofinputs));
					WriteData(0x20);  //Write a space
					WriteIns(0x80 + (9 - numberofinputs - 1));
					WriteData(NumAlpha[0]);  //Write a space
					numberofinputs++;
					select = 0;
					break;

				case 13:  //SW15 selects 0~9 then A~Z
					WriteIns(0x80 + (9 - numberofinputs));
					if(select == 35){
						select = 0;
					}
					else{
						select++;
					}
					WriteData(NumAlpha[select]);
					break;
			}
		}

		if(numberofinputs == 0){
			break;
		}
	}
	
	return 0;
}
