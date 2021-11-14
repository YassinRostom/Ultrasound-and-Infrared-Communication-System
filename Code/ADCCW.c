#include "ADCCW.h"
#include "STM32F4xx.h"
#include <stdio.h>

/*----------------------------------------------------------------------------
  Intializing the ADC1
 *----------------------------------------------------------------------------*/
		void Init_ADC1(void)
			{
			RCC->AHB1ENR |= (1UL << 1);								//Enabling the clock of GPIOB
			GPIOB->MODER |= 0x0000000C;								//Setting PB1 as Analogue pin
			RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;					//Enable the clock for ADC 1 module
			RCC->CFGR|=RCC_CFGR_PPRE2_DIV2;						//Set the prescaler for the clock
			ADC1->CR2|=ADC_CR2_ADON;									//Turn ON ADC module
			ADC1->SMPR1|=ADC_SMPR1_SMP16;							//480	cycles, better accuracy than 3 cycles
			ADC1->SQR3|=0x00000009;										//Selecting channel 9 as input for PB1
			}
/*----------------------------------------------------------------------------
  Start the ADC conversion and return the reading
 *----------------------------------------------------------------------------*/
		uint16_t Get_ADC1()
			{
			ADC1->CR2|=ADC_CR2_SWSTART;										//Software trigger the conversion
			while(!(ADC1->SR&(1UL<<1))){}									//Wait for the completion of the conversion
			return ADC1->DR;															//Return the reading value	
			}

/*----------------------------------------------------------------------------
  Start the ADC conversion and return the averaged value of reading
 *----------------------------------------------------------------------------*/
		uint16_t Get_Average_ADC1(uint8_t times)
			{
			uint8_t t;												//Initizlizing a variable called t
			uint32_t Average=0;								//Initizlizing a variable called Average
				for(t=0;t<times;t++)
				{
				Average+=Get_ADC1();  				//Getting the Average value of the ADC conversion
				}
				return Average/times;						//Returning the average value of the Reading
			}

/*----------------------------------------------------------------------------
  Intializing the DAC2 as 12-bit right alignment DAC And the GPIOA
 *----------------------------------------------------------------------------*/
		void Init_DAC2(void)
			{
			RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;						//Enable the clock for DAC module
			RCC->APB1ENR|=RCC_APB1ENR_DACEN;							//Enable the clock for GPIO Port A
			GPIOA->MODER|=GPIO_MODER_MODER4;							//Configure the PoRT A pin 4 to be the Output
			GPIOA->PUPDR&=~(GPIO_PUPDR_PUPDR4);						//Clearing PA4 pull up ,pull-down
			DAC->CR|=DAC_CR_EN1;													//Enable the DAC
			DAC->DHR12R1|=0x7ff;													//Write to the DAC channel1 12-bit right-aligned data holding register which I will use it to set my threshold
			DAC->CR|=DAC_CR_TEN1;													//Enabling the DAC channel 1 Trigger
			}
/*----------------------------------------------------------------------------
	Read the ADC value than output the value to DAC
 *----------------------------------------------------------------------------*/
		int ADC_value (void)
			{
			uint16_t Reading;														//Initizlizing a variable called Reading
			Init_DAC2();																//Initizlizing DAC2
			Init_ADC1();																//Initizlizing ADC1
			Reading=Get_ADC1();													//Setting the Reading value equal to the ADC1 value 
			//DAC->DHR12R2=Reading;												//Setting the DAC 12-bit right aligned Data Holding register = reading of the ADC 
			return Reading;															//returning the reading value
			}
