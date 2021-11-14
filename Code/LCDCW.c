#include "LCDCW.h"
#define 	termination '\0'
unsigned char BF;
unsigned char blank[20] = {"                   "};

// Make Bits 12-15 of PORT E inputs //
	void Config_portE_Input(void)
		{
			LCD_PORT->MODER    &= ~	(                       
				(3UL << 2*12) |                       
				(3UL << 2*13) |
				(3UL << 2*14) |
				(3UL << 2*15) );   																							/* PE12,PE13,PE14,PE15 = input (00)             */
		}
/*---------------------------------------------------------------------*/
// Make Bits 12-15 of PORT E outputs //
	void Config_portE_Output(void)
		{   
			LCD_PORT->MODER    |= (                      
			(1UL << 2*12) |                       
			(1UL << 2*13) |
			(1UL << 2*14) |
			(1UL << 2*15) );  																						 /* PE12,PE13,PE14,PE15 = output (01)              */   
		}
/*---------------------------------------------------------------------*/
// Initialise LCD Pins //
	void LCD_Init(void)
		{
			RCC->AHB1ENR  |= ((1UL <<  4) );         													// Enable GPIOE clock 
			LCD_PORT->MODER    &= ~(
				(3UL << 2*9) |
				(3UL << 2*10) |
				(3UL << 2*11) );																								//Setup LCD Control Pins (PE9,PE10,PE11) as inputs (00)
			LCD_PORT->MODER    |=  (
				(1UL << 2*9) |
				(1UL << 2*10) |
				(1UL << 2*11) );   																					  	//Setup LCD Control Pins (PE9,PE10,PE11) as inputs (00)
// Setup LCD Data Bus PE12,PE13,PE14,PE15
	Config_portE_Input();																									//Clearing PE12,PE13,PE14,PE15
	Config_portE_Output();																								//Setting PE12,PE13,PE14,PE15
  	LCD_PORT->OTYPER   &= ~(                
				(1UL << 9) |
				(1UL << 10) |
				(1UL << 11) |                       
				(1UL << 12) |                       
				(1UL << 13) |
				(1UL << 14) |
				(1UL << 15) );   																							/* PE9,PE10,PE11,PE12,PE13,PE14,PE15 as output Push-Pull     */
  	LCD_PORT->OSPEEDR  &= ~(                    
				(3UL << 2*9) |
				(3UL << 2*10) |
				(3UL << 2*11) |                       
				(3UL << 2*12) |                       
				(3UL << 2*13) |                    
				(3UL << 2*14) |
				(3UL << 2*15)	);   																						/* Clearing PE9,PE10,PE11,PE12,PE13,PE14,PE15     */
	LCD_PORT->OSPEEDR  |=  (                 
				(2UL << 2*9) |
				(2UL << 2*10) |
				(2UL << 2*11) |                       
				(2UL << 2*12) | 
				(2UL << 2*13) | 
				(2UL << 2*14) | 
				(2UL << 2*15) ); 																							/* PE9,PE10,PE11,PE12,PE13,PE14,PE15 is 50MHz Fast Speed     */										 
  	LCD_PORT->PUPDR    &= ~(                 
				(3UL << 2*9) |
				(3UL << 2*10) |
				(3UL << 2*11) |                       
				(3UL << 2*12) |                     
				(3UL << 2*13) |
				(3UL << 2*14) |
				(3UL << 2*15) );   																						/* PE9,PE10,PE11,PE12,PE13,PE14,PE15 is Disabled Pull up-down*/
											 
    	BitClr(LCD_E);        														  					//Clearing The Enable Signal
			BitClr(LCD_RW);																								//Clearing the Read/Write Signal to Write mode
    	BitClr(LCD_RS);    														 					      //Clearing the Register Select signal to the Instruction Command 

    	delay_nms(30);         																		  	//Delay 30 miliseconds
    	LCD_DATA(0x28,CMD);    																				//set to 4 bit interface, 2 line and 5*8 font
			delay_nms(30);																								//Delay 30 miliseconds
			LCD_DATA(0x28,CMD);    														 						//set to 8 bit interface, 2 line and 5*8 font		
			delay_nms(30);																								//Delay 30 miliseconds
			LCD_DATA(0x28,CMD); 																				 	//set to 8 bit interface, 2 line and 5*8 font		
			delay_nms(30);																								//Delay 30 miliseconds
			LCD_DATA(0x0f,CMD);    																				//cursor on, cursor position blink
			LCD_DATA(0x10,CMD);
			LCD_CLR;           																						//Clear LCD display
    	LCD_DATA(0x06,CMD);  														    					//move cursor right after write
			LCD_HOME;																											//return Home-Top line Left
}
/*---------------------------------------------------------------------*/
	void WaitLCDBusy(void)
		{
 // 	delay_nms(3);        		 			 																// delay 3 miliseconds
			int busy=1;								      															 //Initilizing a Variable  named busy
			Config_portE_Input();				  																 //Calling Function to Configure Port E pins as Input
			BitSet(LCD_RW);         																			 //Setting the Read/Write signal to Read mode
			BitClr(LCD_RS);																								 //Clearing the Register Select signal to the Instruction Command 
	while(busy)
			{
			delay_nus(20);																								//Delay 10 microseconds
			BitSet(LCD_E);        																		 		//Setting the Enable Signal High
			delay_nus(20);																								//Delay 10 microseconds
			busy=LCD_PORT->IDR;																						//Making the Variable Busy equal to the input of Port E 
			BitClr(LCD_E);																							  //Clearing The Enable Signal
			delay_nus(20);																								//Delay 10 microseconds			
			BitSet(LCD_E);         																				//Setting the Enable Signal High
			delay_nus(20);																								//Delay 10 microseconds
			//busy|=(LCD_PORT->IDR>>4);																		//--------------------
			BitClr(LCD_E);																								//Clearing The Enable Signal
			busy&=0x8000;																									//Maskng the busy Flag
			}			
			delay_nus(20);
			Config_portE_Output();																				//Calling Function to Configure Port E pins as Input
			BitClr(LCD_RW);										  													//Clearing the Read/Write Signal to Write mode
		}
/*---------------------------------------------------------------------*/
	void LCD_DATA(unsigned char data,unsigned char type)
		{
			WaitLCDBusy();          														  	 			//Testing LCD Busy Flag
    	 if(type == CMD)
				{
					BitClr(LCD_RS);     																			//Clearing the Register Select signal to the Instruction Command 
				} 
			else 
				{
        BitSet(LCD_RS);        																		  //Setting the Register Select signal to the Data
				}			
			LCD_PORT->BSRR = (0xF0000000);																// CLEAR DATA BITS
			//LCD_PORT->ODR &= 0x00FF;						
			LCD_PORT->BSRR = ((data&0xf0)<<8);														// SET DATA BITS
			//LCD_PORT->ODR |= data<<8;		
			
			delay_nus(10);        			 																  //Delay 10 microseconds
    	BitSet(LCD_E);         	   																	  //Setting The Enable Signal
    	delay_nus(1);        																				  //Delay 1 microseconds
    	BitClr(LCD_E);         																 			 //DISABLE LCD DATA LINE
	
			LCD_PORT->BSRR = (0xF0000000);
			LCD_PORT->BSRR = ((data&0x0f)<<12);		

			delay_nus(10);        			 															   //Delay 10 microseconds
    	BitSet(LCD_E);         	   																   //Setting The Enable Signal
    	delay_nus(1);            	  													  		  //Delay 1 microseconds
    	BitClr(LCD_E);         														    			 //Clearing The Enable Signal
}
/*---------------------------------------------------------------------*/
//Function to choose the Line ,Position and Text to display on LCD
	void LCD_Display_Line_Position_Text (int line,int pos,char text[])		
		{
		int counter;																										//Inilizing a variable named counter
		if( line == 1 )
			{
				LCD_DATA(0x80+pos,CMD);																			//Displaying on first line and pos depends on the value chosen
			}
		else 
			{
			LCD_DATA(0xC0+pos,CMD);																				//Displaying on Second line and pos depends on the value chosen
			}
		for(counter=0;counter<40;counter++)
			{
				if(text[counter] != termination)
					{
						WaitLCDBusy();																				//Calling the WaitLCDBusy Function
						LCD_DATA(text[counter],TXT);													//Writting to lcd
						//WaitLCDBusy();																			//Calling the WaitLCDBusy Function
					}
				else
				{
					break;																									//Break will exit the function
				}
			}
		}
