#include "usartCW.h"
	
	void Baud_Rate (int Bps)
		{
//			unsigned int Data;
//			Data = (((168000000/4)<<5) / (16 * Bps));					//Baud Rate equation for Standard USART by choosing value of Bps I set the Required Baud Rate 
//			USART3->BRR = (Data>>1) + (Data&0x1);							//The output of the Baud Rate equation (Data) shifting it to the left and masking it then add the 2 values and Set the Usart Baud rate register
				USART3->BRR = 0x445C;															//Setting the Baud Rate register to be able to run at Baud Rate 600
		}	
//-----------------------------------------------------------
	void config_UART3 (void)
		{
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;							// Enable the clock to USART3
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; 							// Enable the clock to GPIOC
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 							// Enable the clock to GPIOB
			CLEAR_BIT(GPIOC->MODER,3UL<<(TX*2));							//Clearing the TX bit PC10
			SET_BIT(GPIOC->MODER,2UL<<(TX*2));								// Set TX bit PC10 to Alternate function (10)
			CLEAR_BIT(GPIOB->MODER,3UL<<(RX*2)); 							//Clearing the RX bit PB11
			SET_BIT(GPIOB->MODER,2UL<<(RX*2)); 								//Set RX bit PB11 to Alternate function (10)
			CLEAR_BIT(GPIOC->OTYPER, 1UL<<TX);								//Set TX bit  PC10 to push-pull mode
			CLEAR_BIT(GPIOB->OTYPER, 1UL<<RX);								// Set RX bit PB11 to push-pull mode
			CLEAR_BIT(GPIOC->OSPEEDR, 3UL<<(TX*2));						//Set TX bit  PC10  speed to 2MHz to save power
			CLEAR_BIT(GPIOB->OSPEEDR, 3UL<<(RX*2));						//Set RX bit PB11 speed to 2MHz to save power
			CLEAR_BIT(GPIOC->PUPDR, 3UL<<(TX*2));							//Clearing the TX bit PC10 (No pullup,pulldown)
			CLEAR_BIT(GPIOB->PUPDR, 3UL<<(RX*2));							//Clearing the RX bit PB11 (No pullup,pulldown)
			GPIOC->AFR[1] |= 7ul<<(2*4);							 				//Set Alternate Function Register for TX to allow only one peripheral Alternate Function to be connected at a time
			GPIOB->AFR[1] |= 0x07<<(3*4);											//Set Alternate Function Register for RX to allow only one peripheral Alternate Function to be connected at a time
			CLEAR_BIT(USART3->CR2, (1<<12|1<<13));						//Clear STOP bits = 1 STOP BIT
			CLEAR_BIT(USART3->CR1, 1<<15);										//16 bit oversampling
			SET_BIT(USART3->CR1, 1<<12);											//1 start bit 9 Data bits and n stop bits
			SET_BIT(USART3->CR1, 1<<10);											//Enable PARITY
			CLEAR_BIT(USART3->CR1, 1<<9);											//EVEN Parity
			SET_BIT(USART3->CR1, 1<<3); 											//Transmitter enable
			SET_BIT(USART3->CR1, 1<<2);												//Reciever enable
			SET_BIT(USART3->CR1, 1<<13);											//USART Enable
			
			//Enabling the  USART3 Interrupt
			NVIC_EnableIRQ(USART3_IRQn);
			SET_BIT(USART3->CR1, 1<<5);												//Enabling the RXNEIE interrupt 
			SET_BIT(USART3->CR1, 1<<6);												//Enabling the TCIE interrupt 
			SET_BIT(USART3->CR1, 1<<8);												//Enabling the Parity interrupt 
			Baud_Rate(9600);																	//Setting the Baud Rate 
		}
	
			
//----------------------------------------------------------------------------//
//The Usart Interrupt Handler Function
unsigned char RX_DATA_US=0;
unsigned char RX_DATA_Comp=0;			
		
	void USART3_IRQHandler (void)									
		{
			if(USART3->SR & USART_SR_RXNE)																				//Checking the RXNE flag to see if data is received and ready to be read (1) or Data is not Received (0)
				{
					USART3->SR &= ~USART_SR_RXNE;																			//Clearing the RXNE bit in the status register 
						if(USART3->SR & USART_SR_PE)																		//Checking if the parity error bit is is equal to one (Which inicates that I am receiving an error)
							{
								GPIOE->ODR ^= (1UL<<LED_Pin_System_Out_Of_Specification);		//Toggle Red LED To indicate that the system is receiving an error
							}		
								GPIOE->ODR ^= (1UL<<LED_Pin_System_Working);								//Toggle the LED which indicates that the system is working correctly
								RX_DATA_US=USART3->DR;																			//Reading the DR to clear the RXNE Flag and Moving the data received from USART3_DR to RX_DATA_US
								USART6->DR = RX_DATA_US;																		//Moving the data in RX_DATA_US TO USART6 DR
				}			
			if(USART3->SR & USART_SR_TC)
				{	
					USART3->SR &= ~USART_SR_TC;																					//Clearing the TC Flag
				}
		}
			
//----------------------------------------------------------------------------
		void USART_SendData(uint16_t Data)
			{
				USART3->DR = (Data & (uint16_t)0x01FF);	  										  	//Sending the data that will be Transmitted to the DR first 9 bits
			}
//----------------------------------------------------------------------------
		unsigned int USART_Status(void)
			{
				unsigned int status;																							//Initlizing a variable 
				status = (USART3->SR & (1<<6));																		//Checking TC in SR to see if the transmission is completed or not
				return status;																				
			}
//----------------------------------------------------------------------------
/* Use no semihosting */
		#if 1
		#pragma import(__use_no_semihosting)
		struct __FILE
			{  
				int handle;
			};
		FILE __stdout;
		void _sys_exit(int x)
			{
				x = x;
			}
#endif
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

/**
  * @brief  Retargets the C library printf function to the USART.
  */
		PUTCHAR_PROTOTYPE
		{
			USART_SendData((uint8_t) ch);											// Function to write a character to the USART 
			while ((USART3->SR & (1<<6)) == 0)								// Loop as long as the transmission is not yet completed (TC=0)
				{}
			LCD_DATA(ch,TXT);																	//Printing to the LCD the charactrs transmitted when the transmission is completed and TC=1
				return ch;
		}
//--------------------------------------------------------------------//
		
	void Baud_Rate_USART6 (int Bps)
		{
//		unsigned int Data;
//		Data = (((168000000/4)<<5) / (16 * Bps));					//Baud Rate equation for Standard USART by choosing value of Bps I set the Required Baud Rate 
//		USART6->BRR = (Data>>1) + (Data&0x1);							//The output of the Baud Rate equation (Data) shifting it to the left and masking it then add the 2 values and Set the Usart Baud rate register
			USART6->BRR = 0x445C;															//Setting the Baud Rate register to be able to run at Baud Rate 600
		}	
//-----------------------------------------------------------//
		void config_UART6 (void)
			{
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;							// Enable the clock to USART6
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; 							// Enable the clock to GPIOC
			CLEAR_BIT(GPIOC->MODER,3UL<<(TX6*2));							//Clearing the TX bit PC6 
			SET_BIT(GPIOC->MODER,2UL<<(TX6*2));								// Set TX bit PC6 to Alternate function (10)
			CLEAR_BIT(GPIOC->MODER,3UL<<(RX6*2)); 						//Clearing the RX bit PC7
			SET_BIT(GPIOC->MODER,2UL<<(RX6*2)); 							//Set RX bit PB11 to Alternate function (10)
			CLEAR_BIT(GPIOC->OTYPER, 1UL<<TX6);								//Set TX bit  PC6 to push-pull mode
			CLEAR_BIT(GPIOC->OTYPER, 1UL<<RX6);								// Set RX bit PC7 to push-pull mode
			CLEAR_BIT(GPIOC->OSPEEDR, 3UL<<(TX6*2));					//Set TX bit  PC6  speed to 2MHz to save power
			CLEAR_BIT(GPIOC->OSPEEDR, 3UL<<(RX6*2));					//Set RX bit PC7 speed to 2MHz to save power
			CLEAR_BIT(GPIOC->PUPDR, 3UL<<(TX6*2));						//Clearing the TX bit PC6 (No pullup,pulldown)
			CLEAR_BIT(GPIOC->PUPDR, 3UL<<(RX6*2));						//Clearing the RX bit PC7 (No pullup,pulldown)
			GPIOC->AFR[0] |= 8ul<<(6*4);							 				//Set Alternate Function Register for TX to allow only one peripheral Alternate Function to be connected at a time
			GPIOC->AFR[0] |= 8ul<<(7*4);											//Set Alternate Function Register for RX to allow only one peripheral Alternate Function to be connected at a time
			CLEAR_BIT(USART6->CR2, (1<<12|1<<13));						//Clear STOP bits = 1 STOP BIT
			CLEAR_BIT(USART6->CR1, 1<<15);										//16 bit oversampling
			SET_BIT(USART6->CR1, 1<<12);											//1 start bit 9 Data bits and n stop bits
			SET_BIT(USART6->CR1, 1<<10);											//Enable PARITY
			CLEAR_BIT(USART6->CR1, 1<<9);											//EVEN Parity
			SET_BIT(USART6->CR1, 1<<3); 											//Transmitter enable
			SET_BIT(USART6->CR1, 1<<2);												//Reciever enable
			SET_BIT(USART6->CR1, 1<<13);											//USART Enable
			
			//Enabling the  USART6 Interrupt
			NVIC_EnableIRQ(USART6_IRQn);
			SET_BIT(USART6->CR1, 1<<5);												//Enabling the RXNEIE interrupt 
			SET_BIT(USART6->CR1, 1<<6);												//Enabling the TCIE interrupt 
			SET_BIT(USART6->CR1, 1<<8);												//Enabling the Parity interrupt 
			Baud_Rate_USART6(9600);														//Setting the Baud Rate 
		}
//----------------------------------------------------------------------------//
	void USART6_IRQHandler (void)									
		{
			if(USART6->SR & USART_SR_RXNE)										//Checking the RXNE flag to see if data is received and ready to be read (1) or Data is not Received (0)
				{
					USART6->SR &= ~USART_SR_RXNE;									//Clearing the USART_SR RXNE bit 
					GPIOE->ODR ^= (1UL<<LED_Pin_System_Working);	//Toggle the LED which indicates that the system is working correctly
					RX_DATA_Comp=USART6->DR;											//Clearing the RXNE flag by a read to the USART3_DR register
					USART3->DR  = RX_DATA_Comp;										//Moving the data received from the computer from USART6 DR to USART 3 DR
				}
			if(USART6->SR & USART_SR_TC)
				{
				USART6->SR &= ~USART_SR_TC;											//Clearing the TC flag
				}
		}
//----------------------------------------------------------------------------
	unsigned int USART6_Status(void)
		{
			unsigned int status_USART6;												//Inilizing a variable
			status_USART6 = (USART6->SR & (1<<6));						//Checking TC in SR to see if the transmission is completed or not
			return status_USART6;
		}
	void USART6_SendData(uint16_t Data)
		{
			USART6->DR = (Data & (uint16_t)0x01FF);	  		  	//Sending the data that will be Transmitted to the DR first 9 bits
			while(USART6_Status==0);
		}
