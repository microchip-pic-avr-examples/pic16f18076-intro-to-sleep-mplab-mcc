 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
*/

/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
//#define DEBUG_MODE

/*
    Main application
*/
void ADT_Interrupt(void);
void IoC_Interrupt(void);

static volatile uint8_t whichInterrupt = 0x00; //Manually defined software flags for tracking interrupts

//Bit-masks to track status of example interrupts
#define WDT_TRIGGERED (0x1)
#define ADTI_TRIGGERED (0x1 << 1)
#define IOC_TRIGGERED (0x1 << 2)

int main(void)
{
    SYSTEM_Initialize();
    ADCC_SetADTIInterruptHandler(&ADT_Interrupt);//Include ADT_Interrupt in ISR
    IO_RA0_SetInterruptHandler(&IoC_Interrupt); //Include IoC_Interrupt in ISR
    
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to: 

    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable(); 

    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    
    printf("\nDemo Initialized\r\n");
    //ADCC_StartConversion(ADPCH);
    TMR2_Start();
    while(1)
    {
        printf("\nGoing to Sleep\r\n\n");
        while(!(EUSART2_IsTxDone()));
       
        WDTCONbits.SEN = 1; //Enable Watchdog Timer while in sleep
        SLEEP();
        NOP(); //NOP buffer
        WDTCONbits.SEN = 0; //Disable Watchdog to prevent full reset
        
        while(!(EUSART2_IsTxReady()));
        printf("Device Awake\nTriggered by: \r\n");
        
        whichInterrupt |= ~(STATUSbits.nTO) & 0x01; //Check Watchdog ~TO Flag

#ifdef DEBUG_MODE
        printf("whichInterrupt = %X\nSTATUSbits.nTO = %u\nADCC Value = %u\r\n", whichInterrupt, STATUSbits.nTO, ADCC_GetConversionResult());
#endif
        switch(whichInterrupt)
        {
            case WDT_TRIGGERED:
                printf("Watchdog Timer\r\n");
                break;
            case ADTI_TRIGGERED:
                printf("ADCC Threshold Interrupt. \nBlock until ADCC read lowered below upper threshold value of:\n%u\r\n", ((ADUTHH << 8) + ADUTHL));
                while(ADCC_GetConversionResult() >= ((ADUTHH << 8) + ADUTHL)); // Block statement placed to prevent flooding EUSART whenever the ADCC is above the threshold for multiple readings
                break;
            case IOC_TRIGGERED:
                printf("Interrupt on Change\n\nResetting Demo...\r\n");
                while(!(EUSART2_IsTxDone()));
                RESET();
            default :
                printf("Error: No trigger detected or multiple triggers detected\nwhichInterrupt = %u\r\n", whichInterrupt);
                break;
        }
        while(!(EUSART2_IsTxDone()));
        whichInterrupt = 0x00; //Clear whichInterrupt flags
    }    
}
void ADT_Interrupt(void)
{
    whichInterrupt |= 1 << 1; //Flag triggered by interrupt
}
void IoC_Interrupt(void)
{
    whichInterrupt |= 1 << 2; //Flag triggered by interrupt
    
}