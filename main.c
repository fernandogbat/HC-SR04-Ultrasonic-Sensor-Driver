//Author : Israel N. Gbati


#include "TM4C123.h"                    // Device header

#include<stdint.h>

void delay_Microsecond(uint32_t time);
void Timer0_init(void);
uint32_t measureD(void);

const double _16MHz_1clock = 62.5e-9; /*Value of 1clock cycle in nanoseconds*/
const uint32_t MULTIPLIER  = 5882;  /*Derived from speed of sound*/



#define ECHO (1U<<6) //PB6 
#define TRIGGER		(1U<<4) //PA4(OUTPUT)
#define BLUE_LED (1U<<2)//PF2 onboard Blue LED 
#define RED_LED  (1U<<3)//PF3 
#define IR_PIN   (1U<<5)//PA5 (INPUT)
#define MOTOR1   (1U<<6)//PA6
#define MOTOR2	 (1U<<7)//PA7

uint32_t highEdge,lowEdge;
uint32_t ddistance;  /*Distance in centimeters*/
uint32_t counter =0;


uint32_t measureD(void){
	GPIOA->DATA &=~TRIGGER;
	delay_Microsecond(12);
	GPIOA->DATA |= TRIGGER;
	delay_Microsecond(12);
	GPIOA->DATA &=~TRIGGER;
	/*Capture firstEgde i.e. rising edge*/
	TIMER0->ICR =4;
	while((TIMER0->RIS & 4)==0){}; //Wait till captured
		highEdge =  TIMER0->TAR;
		
		/*Capture secondEdge i.e. falling edge */
		TIMER0->ICR =4; //cleaer timer capture flag
		while((TIMER0->RIS & 4)  ==0){};
			lowEdge = TIMER0->TAR;
			  ddistance = lowEdge -highEdge; 
			  ddistance = _16MHz_1clock *(double) MULTIPLIER *(double)ddistance;
			  
			return ddistance;
			
}
int main(void){
	SYSCTL->RCGCGPIO |=(1U<<0); //Enable clock for PORTA
	SYSCTL->RCGCGPIO |=(1U<<5); //Enable clock for PORTF
	GPIOA->DIR =TRIGGER|MOTOR1|MOTOR2;
	GPIOF->DIR =BLUE_LED|RED_LED; 
	GPIOA->DEN |=(ECHO)|(TRIGGER)|(IR_PIN)|MOTOR1|MOTOR2;
	GPIOF->DEN |= BLUE_LED|RED_LED;
 
	while(1){
		Timer0_init();
		 measureD();
    if(measureD()<15){
		  GPIOF->DATA |=BLUE_LED;
		  GPIOA->DATA |=MOTOR1;
		}
			else{
				GPIOF->DATA &=~BLUE_LED;
			  GPIOA->DATA &=~MOTOR1;
			}
			  delay_Microsecond(100);
			if((GPIOA -> DATA & IR_PIN)==IR_PIN)
			{
				
				GPIOF->DATA |=RED_LED;
				GPIOA->DATA |=MOTOR2;
			}
			else{
				GPIOF->DATA &=~RED_LED;
			  GPIOA->DATA &=~MOTOR2;
			}
  }
}
void delay_Microsecond(uint32_t time)
{
	int i;
	SYSCTL->RCGCTIMER |=(1U<<1);
	TIMER1->CTL=0;
	TIMER1->CFG=0x04;
	TIMER1->TAMR=0x02;
	TIMER1->TAILR= 16-1;
	TIMER1->ICR =0x1;
	TIMER1->CTL |=0x01;
	
	for(i=0;i<time;i++){
	  while((TIMER1->RIS&0x1)==0);
		TIMER1->ICR = 0x1;
	}
	
}
void Timer0_init(void)
{
	SYSCTL->RCGCTIMER |=(1U<<0);
	SYSCTL->RCGCGPIO |=(1U<<1);
	GPIOB->DIR &=~ECHO;
	GPIOB->DEN |=ECHO;
	GPIOB->AFSEL |=ECHO;
	GPIOB->PCTL &=~0x0F000000;
	GPIOB->PCTL |= 0x07000000;
	
	TIMER0->CTL &=~1;
	TIMER0->CFG =4;
	TIMER0->TAMR = 0x17;
	TIMER0->CTL |=0x0C;
	TIMER0->CTL |=1;
}

