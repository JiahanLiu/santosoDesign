// ADCTestMain.c
// Runs on LM4F120/TM4C123
// This program periodically samples ADC channel 8 and 9 and
// stores theresult to a global variable that can be accessed
// with the JTAG debugger and viewed with the variable watch
// feature.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include <stdint.h>
#include "ADCSWTrigger.h"
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "PWM.h"
#include "SysTick.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void ADC0Seq0_Handler(void);

#define PF2        (*((volatile uint32_t *)0x40025010))

//volatile float A,B, Duty;

int main(){
  	
	PLL_Init(Bus80MHz);                   // 80 MHz
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // activate port F
  ADC_Init(8000000);                       // ADC 10 Hz
  SysTick_Init();                       // initialize SysTick timer
	GPIO_PORTF_DIR_R |= 0x04;             // make PF2 out (built-in LED)
  GPIO_PORTF_AFSEL_R &= ~0x04;          // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;             // enable digital I/O on PF2
                                        // configure PF2 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;               // disable analog functionality on PF 
 
	ADC0Seq0_Handler(); // test
//  PLL_Init();                       // bus clock at 80 MHz
//  PWM0A_Init(40000, 30000);         // initialize PWM0, 1000 Hz, 75% duty
//  PWM0B_Init(40000, 10000);         // initialize PWM0, 1000 Hz, 25% duty
//  PWM0_Duty(4000);                  // 10%
//  PWM0_Duty(10000);     						// 25%
//  PWM0_Duty(30000);   							// 75%
  
	
	/*
	while (1){
		Duty = ADC0Seq0_Handler();
		A = 400;											// 100 kHz
		B = 400*Duty;
  	PWM0A_Init(A, B);              // initialize PWM0, 100000 Hz, Duty
		PWM0B_Init(A, (A-B));          // initialize PWM0, 100000 Hz, 1-Duty
//  PWM0A_Init(1000, 900);         // initialize PWM0, 40000 Hz, 90% duty
//  PWM0_Init(1000, 100);          // initialize PWM0, 40000 Hz, 10% duty
//  PWM0_Init(40, 20);             // initialize PWM0, 1 MHz, 50% duty
		}
	*/
	
	//EnableInterrupts();
	
	//while(1){ ADC0Seq0_Handler();}
	//while(1){ 
	
	//EnableInterrupts();
	
	//Duty = ADC0Seq0_Handler();//}
	
	//WaitForInterrupt();
  	//PF2 = 0x04;
  //WaitForInterrupt();
    //PF2 = 0x00;
		
		//WaitForInterrupt();
		
		//Duty = ADC0Seq0_Handler();
		
		//WaitForInterrupt();
		
		//A = 400;											 // 100 kHz
		//B = 400*Duty;
  	//PWM0A_Init(A, B);              // initialize PWM0, 100000 Hz, Duty
		//PWM0B_Init(A, (A-B));          // initialize PWM0, 100000 Hz, 1-Duty
	
		
  }

