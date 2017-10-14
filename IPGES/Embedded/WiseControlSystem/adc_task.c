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
#include "adc_task.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "adc.h" //for adc lib
#include "hw_memmap.h" //for address bases
#include "sysctl.h" //for init ports
#include "gpio.h" //for gpio to be interfaced to adc

//*****************************************************************************
//
// The stack size for the task.
//
//*****************************************************************************
#define ADCTASKSTACKSIZE        128         // Stack size in words

//*****************************************************************************
//
// ADC Init Macros
//
//*****************************************************************************
#define ADC_SEQUENCE3           3

//*****************************************************************************
//
// The item size and queue size for the message queue.
//
//*****************************************************************************
#define ADC_ITEM_SIZE           sizeof(uint8_t)
#define ADC_QUEUE_SIZE          5

//*****************************************************************************
//
// The queue that holds messages sent to the LED task.
//
//*****************************************************************************
xQueueHandle g_pAdcQueue;



extern xSemaphoreHandle g_pUARTSemaphore;
//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
static void
ADCTask(void *pvParameters)
{
    portTickType ui32WakeTime;

    // Get the current tick count.
    ui32WakeTime = xTaskGetTickCount();

		
		int i = 0;
    // Loop forever.
    while(1)
    {  
        uint32_t adcInput;
        ADCProcessorTrigger(ADC0_BASE, ADC_SEQUENCE3);
        while(!ADCIntStatus(ADC0_BASE, ADC_SEQUENCE3, false))
        {
        }
        ADCIntClear(ADC0_BASE, ADC_SEQUENCE3);
        ADCSequenceDataGet(ADC0_BASE, 3, &adcInput);
        /*
        ADCSequenceDataGet(ADC0_BASE, ADC_SEQUENCE0, &adcInput);
        // Guard UART from concurrent access. Print the currently
        // blinking frequency.
        */
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("ADC reading%d: %d\n", i, adcInput);
        xSemaphoreGive(g_pUARTSemaphore);
				i++;
				if(10 == i) {
					i = 0;
				}

        //
        // Wait for the required amount of time.
        //
        vTaskDelayUntil(&ui32WakeTime, 1000 / portTICK_RATE_MS);
    } //forever loop
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
uint32_t
ADCTaskInit(void)
{
    // Print the current loggling LED and frequency.
    UARTprintf("\nADC Init\n");

    // Create a queue for sending messages to the LED task.
    //g_pLEDQueue = xQueueCreate(ADC_QUEUE_SIZE, ADC_ITEM_SIZE);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //activate ADC0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //active GPIO Port D
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3); //choose port D pin 3

    ADCSequenceDisable(ADC0_BASE, ADC_SEQUENCE3);

    ADCSequenceConfigure(ADC0_BASE, ADC_SEQUENCE3, ADC_TRIGGER_PROCESSOR, 0); //adc_base, Sequence Number, Trigger Object, Priority

    ADCSequenceStepConfigure(ADC0_BASE, ADC_SEQUENCE3, 0, ADC_CTL_CH4 | ADC_CTL_END | ADC_CTL_IE); //adc_base, Sequence Number, Step, set flag and end after first

    ADCSequenceEnable(ADC0_BASE, ADC_SEQUENCE3); //adc_base, sequence 
    ADCIntClear(ADC0_BASE, ADC_SEQUENCE3);
    // Create the task.
    if(xTaskCreate(ADCTask, (const portCHAR *)"LED", ADCTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_LED_TASK, NULL) != pdTRUE) 
    {
        return(1);
    }

    // Success.
    return(0);
}
