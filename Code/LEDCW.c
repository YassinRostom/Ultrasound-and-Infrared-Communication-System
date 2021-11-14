#include "STM32F4xx.h"
#include "LEDCW.h"
#include "core_cm4.h"
#define LED_Clock PORTE
#define PORTE 4
#define PORTD 3
/*----------------------------------------------------------------------------
  initialize LED Pins
 *----------------------------------------------------------------------------*/
	void LED_Init (void) 
		{
			RCC->AHB1ENR |= (1UL << LED_Clock);										//Enable Clock of Port E		
			LED_PORT->MODER |= (1<<(5*2))|(1<<(6*2));							//SET PE5 and PE6 as General purpose output mode
			LED_PORT->OTYPER &=~0x0060;										  			//Set PE5 and PE6 as Output push-pull
			LED_PORT->OSPEEDR &=~0x00002800;								  		//Set the speed to 2MHz to save power
			LED_PORT->PUPDR &=~0x00002800;										 		//Set PE5 and PE6 as no pull-up, pull down
		}
/*----------------------------------------------------------------------------
  Function that indicates that the system is operating correctly 
 *----------------------------------------------------------------------------*/
	void LED_Functioning_Correctly (void) 																								//Creating an LED_Functioning_Correctly Function
		{
			GPIOE->BSRR = (1UL<<LED_Pin_System_Working);																			//Setting PE6 will Turn LED ON 
			delay_nms(100);																																		//delay_nms 250 ms
			GPIOE->BSRR = (1UL<<(LED_Pin_System_Working+16));																	//Clearing PE6 will Turn LED OFF 
			delay_nms(100);																																		//delay_nms 250 ms
		}
/*----------------------------------------------------------------------------
  Function that indicates that the system is operating out of specification 
 *----------------------------------------------------------------------------*/
	void LED_Performing_out_of_Specification (void) 																		//Creating an LED_Performing_out_of_Specification
		{
			GPIOE->BSRR = (1UL<<LED_Pin_System_Out_Of_Specification);												//Setting PE5 will Turn LED ON 
			delay_nms(100);																																	//delay_nms 250 ms
			GPIOE->BSRR = (1UL<<(LED_Pin_System_Out_Of_Specification+16));			  					//Clearing PE5 will Turn LED OFF 
			delay_nms(100);																		                        			//delay_nms 250 ms
		}
//----------------------------------------------------------------------------
//Initialize Timmer
//----------------------------------------------------------------------------
	void Timmer_Init (void) 
		{
			RCC->AHB1ENR |= (1UL << LED_Clock);											//Enable Clock of Port E		
			LED_PORT->MODER |= (1<<(5*2));											  	//SET PE5 as General purpose output mode	
			RCC->APB2ENR |= (1<<16);										    				//Enable Timmer 9 CLOCK 
			TIM9->PSC=10;																			 		  //Setting PSC 10000 to make timmer 1 second
			TIM9->ARR=8400;																					//Setting the ARR 8400 to make timmer 1 second
			TIM9->CR1 |= (1<<3);																		//Setting it to one pulse mode
			TIM9->EGR |= (1<<0);																		//Re-initialize the timer
			TIM9->SR &=~ (1<<0);																		//Clearing the update flag
			TIM9->DIER |= (1<<0);																		//Enabling the interrupt on update event
			NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);											//Enabling the IRQ Timmer 9
			TIM9->CR1 |= (1UL<<0);																	//Enabling Timmer 9 counter
		}

	void TIM1_BRK_TIM9_IRQHandler()
		{
			if(TIM9->SR & (1<<0) !=0)																		 	//if updating the Flag is set
			{
				GPIOE->BSRR = (1UL<<LED_Pin_System_Out_Of_Specification);		//Toggle the RED LED  
				GPIOE->BSRR = (1UL<<LED_Pin_System_Working);								//Toggle the Green LED 
			}
				TIM9->SR&=~(1<<0);																					//Clearing the interrupt Flag
		}
