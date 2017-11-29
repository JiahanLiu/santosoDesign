// ADCSWTrigger.c
// Runs on LM4F120/TM4C123
// Provide functions that sample ADC inputs PE4, PE5 using SS2
// to be triggered by software and trigger two conversions,
// wait for them to finish, and return the two results.
// Daniel Valvano
// May 2, 2015

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

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"

#include "SysTick.h"
#include "PLL.h"

/////////////////
#define NVIC_EN0_INT17          0x00020000  // Interrupt 17 enable

#define TIMER_CFG_16_BIT        0x00000004  // 16-bit timer configuration,
                                            // function is controlled by bits
                                            // 1:0 of GPTMTAMR and GPTMTBMR
#define TIMER_TAMR_TACDIR       0x00000010  // GPTM Timer A Count Direction
#define TIMER_TAMR_TAMR_PERIOD  0x00000002  // Periodic Timer mode
#define TIMER_CTL_TAOTE         0x00000020  // GPTM TimerA Output Trigger
                                            // Enable
#define TIMER_CTL_TAEN          0x00000001  // GPTM TimerA Enable
#define TIMER_IMR_TATOIM        0x00000001  // GPTM TimerA Time-Out Interrupt
                                            // Mask
#define TIMER_TAILR_TAILRL_M    0x0000FFFF  // GPTM TimerA Interval Load


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

////////////////////////                                            // Register Low

// There are many choices to make when using the ADC, and many
// different combinations of settings will all do basically the
// same thing.  For simplicity, this function makes some choices
// for you.  When calling this function, be sure that it does
// not conflict with any other software that may be running on
// the microcontroller.  Particularly, ADC0 sample sequencer 2
// is used here because it takes up to four samples, and two
// samples are needed.  Sample sequencer 2 generates a raw
// interrupt when the second conversion is complete, but it is
// not promoted to a controller interrupt.  Software triggers
// the ADC0 conversion and waits for the conversion to finish.
// If somewhat precise periodic measurements are required, the
// software trigger can occur in a periodic interrupt.  This
// approach has the advantage of being simple.  However, it does
// not guarantee real-time.
//
// A better approach would be to use a hardware timer to trigger
// the ADC conversion independently from software and generate
// an interrupt when the conversion is finished.  Then, the
// software can transfer the conversion result to memory and
// process it after all measurements are complete.

// Initializes ADC8 and ADC9 sampling
// 125k max sampling
// SS2 triggering event: software trigger, busy-wait sampling
// SS2 interrupts: enabled after 2nd sample but not promoted to controller
void ADC_Init(uint32_t period){ 
  volatile uint32_t delay;                
	
	DisableInterrupts();
	SYSCTL_RCGCADC_R |= 0x00000001; // 1) activate ADC0
	SYSCTL_RCGCTIMER_R |= 0x01;   // activate timer0 
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // 1) activate clock for Port E

  delay = SYSCTL_RCGCGPIO_R;      // 2) allow time for clock to stabilize 
	
	TIMER0_CTL_R = 0x00000000;    // disable timer0A during setup
  TIMER0_CTL_R |= 0x00000020;   // enable timer0A trigger to ADC
  TIMER0_CFG_R = 0;             // configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // prescale value for trigger
  TIMER0_TAILR_R = period-1;    // start value for trigger
  TIMER0_IMR_R = 0x00000000;    // disable all interrupts
  TIMER0_CTL_R |= 0x00000001;   // enable timer0A 32-b, periodic, no interrupts
  
	GPIO_PORTE_DIR_R &= ~0x08;      // 3) make PE3 input 
  GPIO_PORTE_AFSEL_R |= 0x08;     // 4) enable alternate function on PE3
  GPIO_PORTE_DEN_R &= ~0x08;      // 5) disable digital I/O on PE3
  GPIO_PORTE_AMSEL_R |= 0x08;     // 6) enable analog functionality on PE3
	
  ADC0_PC_R |= 0x5;               //   8) configure for 500K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 9) Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x08;        // 10) disable sample sequencer 3
	
  ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFF0FFF)+0x5000; // timer trigger event ///
	ADC0_SSMUX3_R += 0;//0x75194680;         // 12) set channels for SS3 ADC 0
  ADC0_SSCTL3_R = 0x06;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1  !!! write all the 8 bit!!! stop at 4th data

	ADC0_IM_R |= 0x08;              // enable SS3 interrupts
  ADC0_ACTSS_R |= 0x08;          // 15) enable sample sequencer 3

	NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFF00FF)|0x00004000; //priority 2
  NVIC_EN0_R = 1<<17;              // enable interrupt 17 in NVIC
}

//------------ADC0Seq0_Handler------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: two 12-bit result of ADC conversions
// Samples ADC6789
// 125k max sampling
// Timer trigger, busy-wait sampling
// data returned by reference
// data[0] is ADC0 (PE3) 0 to 4095


volatile float data[1];
volatile float V,b,c,d,y, I, P;
volatile float Vold = 0;
volatile float Dold = 6, Dmax = 100, Dmin = 5, deltaD = 0.01;
volatile float dV = 0, D, DC = 0.21, DeltaDC, f=0.1, g=0.05;

volatile float A,B, Duty;

/*
// delay function for testing from sysctl.c
// which delays 3*ulCount cycles
#ifdef __TI_COMPILER_VERSION__
	//Code Composer Studio Code
	void Delay(uint32_t ulCount){
	__asm (	"    subs    r0, #1\n"
			"    bne     Delay\n"
			"    bx      lr\n");
}

#else
	//Keil uVision Code
	__asm void
	Delay(uint32_t ulCount)
	{
    subs    r0, #1
    bne     Delay
    bx      lr
	}
#endif
*/
void ADC0Seq0_Handler(void){
	volatile uint32_t delay;
  while (1) {	
	
	//ADC_Init(8000000);                       // ADC 10 Hz
	
	ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
	data[0] = ADC0_SSFIFO3_R;  			 // 3a) read 1 result
	ADC0_ISC_R = 0x0008;               // 4) acknowledge completion ADC SS3

  V = (data[0]/4095*3.3*0.75)*16;  //Vout
	
	dV = V-Vold;
	DeltaDC = f-g;
		
	if (DeltaDC!=0){
	  if (DeltaDC < 0)
		{
			if (dV < 0)
				{DC = f + deltaD;}
			else
				{DC = f - deltaD;}	
			}
	 else 
		 {
			if (dV < 0)
				{DC = f - deltaD;}
			else
				{DC = f + deltaD;}	
			}
	 }
	
	else
		{DC = f+deltaD;}
		
		
  if (DC >0.8){ DC = 0.8;}
  if (DC <0.05){ DC = 0.05;}
	
	
  g=f;
  f=DC;	
	Vold = V;
	
	
	A = 800;											 // 100 kHz
	B = 800*DC;
  
	PWM0A_Init(A, B);              // initialize PWM0, 100000 Hz, Duty
	
	//ADC0_ISC_R = 0x0008;               // 4) acknowledge completion ADC SS3
	//Delay(1000);
	//delay = SYSCTL_RCGCGPIO_R;
	//delay = SYSCTL_RCGCGPIO_R;
	
	SysTick_Wait10ms(60);      // approximately 10 ms
	
 }
}
