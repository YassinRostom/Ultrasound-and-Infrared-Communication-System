#include "delayCW.h"
#define ms_count 168000000/1000
#define us_count 168000000/1000000
#define u32 unsigned int
static __IO uint32_t sysTickCounter;

		void SysTick_Handler(void)										//Function Systick Handler
			{
				TimeTick_Decrement();
			}

		void SysTick_Init(void)												// This method needs to be called in the SysTick_Handler
			{
				while (SysTick_Config(us_count) != 0)			// One SysTick interrupt now equals 1us
				{
				} 
			}

		void TimeTick_Decrement(void)
			{
					if (sysTickCounter != 0x00)
				{
				sysTickCounter--;
				}
			}
		void delay_nus(u32 n) 												//Function that makes a delay of n microseconds
		{
			sysTickCounter = n;
		while (sysTickCounter != 0)
			{
			}
		}
		void delay_1ms(void) 												//Function that makes a delay of 1 miliseconds
		{
				sysTickCounter = 1000;
			while (sysTickCounter != 0)
			{
			}
		}
		void delay_nms(u32 n) 												//Function that makes a delay of n miliseconds
		{
			while (n--)
			{
			delay_1ms();
			}
		}


		void clock_set (void)
		{
	
//******************************************************************************
//*            PLL (clocked by HSE) used as System clock source                *
//******************************************************************************

		RCC->CR |= RCC_CR_HSEON;										//start HSE
		while(!(RCC->CR & RCC_CR_HSERDY));					//wait for HSE RDY bit set
		
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;					//enable power interface clock source
    PWR->CR |= PWR_CR_VOS;

	
		#define PLL_N	168														//SYSTEM CLOCK SPEED (FCY (MHz))
		#define HSE		8000000												//XTAL FREQUENCY
		
		#define PLL_M	(HSE/2000000)									//Fcy = Fxtal x PLL_N/(PLL_P x PLL_M)
		#define PLL_P	2
		#define PLL_Q	7
    // HCLK = SYSCLK / 1
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;						//CORE CLOCK = 168MHZ
    
    // PCLK2 = HCLK / 2
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV4;						//PERIPHERAL CLOCK 2 = 168MHZ/4 = 42MHZ, THIS IS BECAUSE THE SPI MODULES (AND POSSIBLY OTHERS) DO NOT OPERATE PROPERLY WHEN PCLK > 42MHZ
    
    // PCLK1 = HCLK / 4
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;						//PERIPHERAL CLOCK 1 = 168MHZ/4 = 42MHZ, THIS IS BECAUSE THE SPI MODULES (AND POSSIBLY OTHERS) DO NOT OPERATE PROPERLY WHEN PCLK > 42MHZ

    // Configure the main PLL
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) | (PLL_Q << 24) | RCC_PLLCFGR_PLLSRC_HSE;

    // Enable the main PLL
    RCC->CR |= RCC_CR_PLLON;

    // Wait till the main PLL is ready
    while(!(RCC->CR & RCC_CR_PLLRDY));
   
    // Configure Flash prefetch, Instruction cache, Data cache and wait state
    FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

    // Select the main PLL as system clock source
    RCC->CFGR &=~ RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait till the main PLL is used as system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
	
//******************************************************************************
//*            END PLL (CLOCKED BY HSE) SETUP CODE                             *
//******************************************************************************
		}
