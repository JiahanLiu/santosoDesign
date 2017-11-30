//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "utils/uartstdio.h"
#include "interpreter.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "adc_task.h"
#include "pwm_task.h"


//*****************************************************************************
//
// The stack size for the task.
//
//*****************************************************************************
#define INTERPRETERTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************
//
// ADC Init Macros
//
//*****************************************************************************
#define INPUTLENGTH           20 //includes null

//*****************************************************************************
//
// The item size and queue size for the message queue.
//
//*****************************************************************************
#define INTERPRETER_ITEM_SIZE           sizeof(uint8_t)
#define INTERPRETER_QUEUE_SIZE          5

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pInterpreterQueue;

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************

static void InterpreterTask(void *pvParameters)
{
		UARTprintf("Interpreter Init\n");
    char uartInput[INPUTLENGTH];
		int dutyCycle;
	
    while(1)
    {  
			UARTprintf("$");
			UARTgets(uartInput, INPUTLENGTH); //note this is blocking, use peek to do non-blocking
			switch(uartInput[0]) {
				case 'A':
					ADC_Print();
					break;
				case 'P': //PWM 50
					dutyCycle = (uartInput[4] - 48) * 10 + uartInput[5] - 48;
					if(0 <= dutyCycle && dutyCycle <= 99) {
						PWM_dutyCycleChange(dutyCycle);
					} else {
						UARTprintf("Duty cycle between 0-99 inclusive.\n");
					}
					break;
				default :
					UARTprintf("Command not found\n");
			}
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
uint32_t InterpreterTaskInit(void)
{
    // Print the current loggling LED and frequency.
    //UARTprintf("PWM Init\n");
	
    // Create a queue for sending messages to the LED task.
    //g_pLEDQueue = xQueueCreate(ADC_QUEUE_SIZE, ADC_ITEM_SIZE);
		
	
    /* Used for more intense signals
    PWMIntEnable(PWM0_BASE, PWM_INT_GEN_0); 
    IntMasterEnable();
    PWMGenIntTrigEnable(PWM0_BASE, PWM_GEN_0, PWM_INT_CNT_LOAD);
    IntEnable(INT_PWM0_0);
     */

    // Create the task.
    if(xTaskCreate(InterpreterTask, (const portCHAR *)"Interpreter", INTERPRETERTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_INTERPRETER_TASK, NULL) != pdTRUE) 
    {
        return(1);
    }
    
    return(0);
}
