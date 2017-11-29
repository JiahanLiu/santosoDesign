// PWMtest.c
// Runs on TM4C123
// Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs.
// Daniel Valvano
// March 28, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
  Program 6.7, section 6.3.2

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
#include "PLL.h"
#include "PWM.h"

void WaitForInterrupt(void);  // low power mode
float A,B;

int main(void){
  PLL_Init();                      // bus clock at 80 MHz
//  PWM0A_Init(40000, 30000);         // initialize PWM0, 1000 Hz, 75% duty
//  PWM0B_Init(40000, 10000);         // initialize PWM0, 1000 Hz, 25% duty
//  PWM0_Duty(4000);    // 10%
//  PWM0_Duty(10000);   // 25%
//  PWM0_Duty(30000);   // 75%
  
	A = 4000;
	B = 500;
	
  PWM0A_Init(A, B);         // initialize PWM0, 10000 Hz, 12.5% duty
	PWM0B_Init(A, (A-B));  // initialize PWM0, 10000 Hz, 87.5% duty
//  PWM0A_Init(1000, 900);          // initialize PWM0, 40000 Hz, 90% duty
//  PWM0_Init(1000, 100);          // initialize PWM0, 40000 Hz, 10% duty
//  PWM0_Init(40, 20);             // initialize PWM0, 1 MHz, 50% duty
  while(1){
    WaitForInterrupt();
  }
}
