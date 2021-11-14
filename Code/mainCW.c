#include <stdio.h>
#include <stm32f4xx.h>
#include "delayCW.c" 
#include "LEDCW.c" 
#include "LCDCW.c"
#include "usartCW.c" 
#include "ADCCW.c"
#include "core_cm4.h"

int PRINT_ADC;											//Initilizing a variable called PRINT_ADC
int input;													//Initilizing a variable called input

	int main (void)
	{	
		//Prescaling the Clock to be able to run at 600 Baud Rate 
		RCC->CFGR |= 7<<13;  					//Prescale the peripgeral bus 2 by 16 = 5.25MHz to be able to run at baud rate=600
		RCC->CFGR |= 3<<11;						//Setting the APB Low speed prescaler
		
		clock_set();									//Initilizing the clock
		SysTick_Init(); 							//Initilizing the SysTick Timer
		LED_Init(); 									//Initilizing the LED
		LCD_Init(); 									//Initilizing the LCD
		Init_DAC2();									//Initilizing the DAC
		Init_ADC1();									//Initilizing the ADC
		config_UART3(); 							//Configuring the UART3
		config_UART6(); 							//Configuring the UART6
		Timmer_Init();								//Initilizing the Timer
		RCC->AHB1ENR |= (1UL << 0);		//Enabling Port A clock

	while(1)
		{
			input=GPIOA->IDR;										//Setting the value of variable input equal to IDR of Port A
			input &=0x01;												//Masking the Variable input
			if(input == 0x00)
			{
				LCD_CLR;																							//Clearing the LCD
				LCD_Display_Line_Position_Text(1,0,"Baud Rate=600");	//Printing to the LCD lINE 1
				LCD_Display_Line_Position_Text(2,0,"DAC=1.5V");				//Printing to the LCD lINE 2
				delay_nms(1000);
			}
			else
			{
				LCD_Display_Line_Position_Text(1,0,"Yassin Rostom");	//Printing to the LCD lINE 1
				LCD_Display_Line_Position_Text(2,0,"ID#10563669");		//Printing to the LCD lINE 2
			}
//		PRINT_ADC=ADC_value();
//		LCD_DATA(LINE1,CMD);
//		printf("%04d\r\n",PRINT_ADC);
//		delay_nms(200);	
		}
	}

