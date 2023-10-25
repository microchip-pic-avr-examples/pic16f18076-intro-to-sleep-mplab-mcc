<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Intro to Sleep with MCC Using a PIC16F18076

<!-- This is where the introduction to the example goes, including mentioning the peripherals used -->
Low power modes are essential for any embedded designer looking to minimize their design’s power consumption. This example will cover how to use the low power “Sleep Mode” on the PIC16F18076 and similar microcontrollers. Alongside sleep mode we will also be using the Watchdog Timer, Interrupt on Change (IoC) and the ADCC and EUSART peripherals. 

## Related Documentation

-	[PIC16F18076 Product Page](https://www.microchip.com/en-us/product/PIC16F18076?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08)

## Software Used

- MPLAB® X IDE 6.1.5 or newer [(MPLAB® X IDE 6.1.5)](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08)
- MPLAB® XC8 2.45.0 or newer compiler [(MPLAB® XC8 2.45)](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08)
- MPLAB® Code Configurator (MCC) 5.3.7 or newer [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/en-us/tools-resources/configure/mplab-code-configurator?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08)
- Microchip PIC16F1xxxx_DFP Series Device Support (1.21.368) or newer [(packs.download.microchip.com/)](https://packs.download.microchip.com/)


## Hardware Used

-	PIC16F18076 Curiosity Nano [EV53Z50A](https://www.microchip.com/en-us/development-tool/EV53Z50A?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08) 
-	Curiosity Nano Base for Click boards™ [(AC164162)](https://www.microchip.com/en-us/development-tool/AC164162?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08)
-   Atmel Power Debugger [(atpowerdebugger)](https://www.microchip.com/en-us/development-tool/ATPOWERDEBUGGER?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08) (or any multimeter/ammeter that can measure in nA)
-   1 kOhm potentiometer, breadboard, and some jumper wires

## Setup

<!-- Explain how to connect hardware and set up software. Depending on complexity, step-by-step instructions and/or tables and/or images can be used -->
### Hardware Setup
Plug the PIC16F18076 into the Curiosity Nano Base board and the MikroE Mic Click into mikroBUS 1.

### Initial Setup for all examples

First, we’ll have to start a new project and enter MCC.

![Configuration Bits Setup](images/config_bits.png)

Open “Configuration Bits” and set the “External Oscillator Selection bits” to “Oscillator not Enabled” and “Reset Oscillator Selection bits to “HFINTOSC(32MHz)”. And to optimize low power performance for the purposes of this demo, disable "Brown Out Reset".

![Clock Control](images/clock_control.png)

Next, go in “Clock Control” under “Project Resources” and select “HFINTOSC_32MHz” for “Current Oscillator Select” and set the “HF Internal Clock” to 1Mhz.


When moving from MCC configuration to application code, much of the process of entering “sleep” remains the same. The user calls the command SLEEP() or __asm(“sleep”) and the microcontroller will enter it’s low power state. The interesting part is leaving Sleep mode, since the CPU is no longer running at this point. Genrally speaking, any interrupt or reset that can be triggered while in this state can be used to wake up the device .

### Using Resets with Sleep Mode (Watchdog Timer)

After the initial setup has been handled, the first method covered will be waking up the microcontroller from sleep using the watchdog timer (WDT). The WDT is traditionally used as a reset to protect from infinite loops or freezes, however the WDT can be configured to run while the device is in sleep, and when a timeout event occurs the MCU will leave its sleep mode instead of executing a reset. This can be achieved by enabling the Watchdog timer before sending the sleep command and simply waiting for it to time out.

![Watchdog Timer](images/config_bits_wdt.png)

To start, return to the “Configuration Bits” menu and set “WDT Operating Mode bits” to “WDT enabled/disabled by SEN bit” (the SEN bit is useful for disabling the WDT when not in use).

![Watchdog Timer](images/WDT.png)

Following setting the WDT in your configuration bits, go to "Device Resources", then add "WDT" from under "System" and add the WDT module. Then set “Software Controlled Watchdog Timer Enable” to “ON”, set the Watchdog Timer to either the LFINTOSC or Secondary Oscillator (SOSC) and set the prescaler to a reasonable value. This example uses 1:65536 with the LFINTOSC for a timer of approximately 2 seconds, however a different timer period may be used.
After your code is generated just enable the watchdog timer with the SEN bit before calling the sleep command and disable it after leaving sleep to prevent accidental resets. An example of this is shown below:

```
WDTCONbits.SEN = 1; //Enable Watchdog Timer while in sleep
SLEEP();
NOP();
WDTCONbits.SEN = 0; //Disable Watchdog to prevent full reset
```

NOTE: It’s good practice to place a NOP command after a sleep command. When the sleep command is called, the next assembly line is cached in the CPU. Having a NOP ensures no accidental operations are executed when switching in or out of low-power modes.

To test this, MCC can be used to set pin "RA1" to toggle led on the Curiosity Nano. Toggle "RA1" under GPIO Output in the Pin Grid View menu as shown below:

![Pin Grid View](images/pin_grid_view_wdt.png)

Then, for code readability, go into the "Pins" menu inside of "Device Resources" and change the corresponding Pin name to "LED" as shown:

![Pins](images/pins_wdt.png)

Generate the new changes then add LED toggles as shown below into the `main.c` while loop to make the Curiosity Nano's LED blink while it's awake. For demonstration purposes a delay function has also been added to make the device stay awake for a time before going back to sleep.

```
LED_SetHigh(); //Turn Off LED
        
WDTCONbits.SEN = 1;
SLEEP();
NOP();
WDTCONbits.SEN = 0;
        
LED_SetLow(); 
__delay_ms(250);
```

After which, program the Curiosity Nano and the LED will blink periodically as the device wakes.

### Using Interrupt-on-Change for Wake-Up

This example uses the PIC16F18076's "Interrupt-on-Change" (IoC) to trigger an intterupt to wake the device from sleep when the Switch on the Curiosity Nano is pressed.

To begin, repeat the initial setup earlier in this document and navigate to the "Pin Grid View" in MCC. Then select RA0 as a GPIO input and RA1 as a GPIO output as shown below:

![Interrupt on Change Pin Grid View](images/IOC_PinGridView.png)

Then navigate to "Pins" under project resources and give the pins RA0 and RA1 the custom names "SW0" and "LED" respectively. Also set RA1 with a "Weak Pullup" to ensure the pin isn't floating when the button isn't pressed. Then all the way on the right of the pins menu there is an "Interupt On Change" column. For RA0/SW0 select "negative" for "Interrupt on Change" to trigger an interrupt to wake the controller on a negative edge. Settings are shown below:

![Interrupt on Change Pins Menu](images/IOC_Pins.png)

Following which, generate the code then navigate to the main.c source file and add this code to the while loop:

```
while(1)
    {
        LED_SetHigh(); //Turn Off LED
        
        SLEEP();
        NOP(); //Prevent next instruction from loading while device enters sleep
        
        LED_SetLow(); //Turn on LED to show when Active
        while(!SW0_GetValue()){;} //block while button is held
    }
```

Lastly, enable global interrupts by uncommenting `INTERRUPT_GlobalInterruptEnable();` from the same file then program the device. The LED will light up when SW0 is pressed and the device will be asleep when SW0 is not pressed.

Note: A debounce circuit or function is good practice when using a pushbutton. This demo leaves out hardware debouncing for simplicity, however an example of hardware debouncing that can be done using the CLC and Timer 2 peripherals can been seen [here](https://mplab-discover.microchip.com/v2/item/com.microchip.code.examples/com.microchip.ide.project/com.microchip.subcategories.modules-and-peripherals.system-modules.Others/com.microchip.mcu8.mplabx.project.pic18f16q40-clc-switch-debouncing/2.0.1?view=about&dsl=debounce).

### Using Peripheral Interrupts With Sleep Mode (ADCC)

The PIC16F18076’s sleep mode can also be left using peripheral interrupts. This can be anything from: a Timer peripheral, the CLC, ADCC, and more. This example will be using the threshold interrupt from the ADCC peripheral, so while the specifics for setting up a peripheral will vary, the general process for using any peripheral interrupt as a wake-up will remain similar.

Before setting up the interrupt, the peripheral will need to be configured. For this example the ADCC will be running in Basic Mode, Right-Aligned, and "Auto-conversion Trigger" set to TMR2. Then set the "Postive Input Channel" to "AND0". The ADCC oscillator must be set to ADCRC to enable ADCC operation while the PIC16F18076 is in sleep. For the threshold: set the “Error Calculation Mode” to “Actual Result vs. Setpoint”, the “Threshold Setpoint” to “0”, “Threshold Interrupt Mode” to “ADERR > ADUTH”, and “Upper Threshold” to your desired ADCC reading to trigger a wakeup (this example will use 700. however any value between 4095 and 0 will work for Basic Mode). Last for this menu, switch on “ADTI Interrupt Enable”. 

![ADCCC Settings](images/ADCT_ADCC.png)


Next, add the Timer 2 module from "Device Resources" and change the oscillator from "HFINTOSC" to "LFINTOSC" and change the timer period as desired. This example uses 1ms, but any duration will work.

![Timer 2 Settings](images/ADCT_TMR2.png)


Next, navigate to the “Pin Grid View” at the bottom of the screen and set the ADCC ANx function to your desired analog pin (RD0 in this example). 

![Pin Grid View](images/ADCT_PinGridView.png.png)


Then, open the “Pins” menu under “Project Resources” and check that ADCC input pin is set to “Analog”.

![Pins Settings](images/ADCT_Pins.png)


After configuring your peripheral and enabling interrupts for it it’s time for application code. Generate your MCC code and go into the main.c. Enable interrupts by uncommenting “INTERRUPT_GlobalInterruptEnable();” and “INTERRUPT_PeripheralInterruptEnable();”.

Note: Peripheral Interrupts for the PIC16F18076 may be used to wake the device from sleep by only enabling peripheral interrupts, however they will be unable to execute tasks from the ISR in this fashion. (TODO: !!!Run Test on this to verify!!!)

Lastly include the function call “TMR2_Start();” in the main function, but before the while loop, to start the ADCC. Then add your statements to trigger SLEEP and the LED. Optionally a while statement to freeze the controller awake while the ADCC result is above the threshold result has been included below.

```
int main(void)
{
    SYSTEM_Initialize();
    INTERRUPT_GlobalInterruptEnable(); 
    INTERRUPT_PeripheralInterruptEnable(); 
    
    TMR2_Start();
    
    while(1)
    {
        LED_SetHigh(); //Turn Off LED
        
        SLEEP();
        NOP(); //Prevent next instruction from loading while device enters sleep
        
        LED_SetLow(); //Turn on LED to show when Active                
        
        while(ADCC_GetConversionResult() >= ((ADUTHH << 8) + ADUTHL)); // Block while ADCC is above designated value
    }    
}
```
After this, the microcontroller can be programmed and an analog sensor my be connected to the ADCC input pin (RD0). This example uses a 1 kOhm potentiometer. When the sensor reading passes the defined point the device will wake up and the LED will light up, then go back to sleep after the potentiometer leaves that range.

## Setting up a Test for Each Method

Since low power is one of the most common reasons to use sleep modes it's worth seeing what the power consumption is for this demonstration. To accomplish this, we connected a Power Debugger to our Curiosity Nano, cut the power strip on the top of the board to disconnect the debugger (a two-pin header can be soldered on after the strap has been cut to reconnect the debugger later), and make sure all unused pins were set to "High Impedance" and connected to ground (VSS can also work for this purpose). Then connect an Atmel Power Debugger to the microcontroller side of the new header and ground as shown below.



![Atmel Power Debugger Connections](images/power_debug.png)

and in the case of the ADCC/Peripheral interrupt example, connect the potentiometer to the power suppley (but before the ammeter)  to isolate the microcontroller's power consumption from the potentiometer's.

Note: For more details on how to use power debugger please see the Atmel Power Debugger User Guide on the [product page](https://www.microchip.com/en-us/development-tool/ATPOWERDEBUGGER?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_pic16f18076&utm_content=intro-to-sleep-pic16f18076&utm_bu=MCU08). Alternatively a sufficient multimeter that can measure down to nanoamps and a seperate power supply may be used to measure current at this stage.

## Operation

Once connected, switch on power and the power usage for any of the the above examples can be measured in Data Visualizer. Acheived values are shown below; however, exact values may vary from device to device. For average expected low power currents, please see the PIC16F18076 data sheet's electrical specifications section.

### WDT Power Consumption

![WDT Current](images/WDT_PowerMeasurement.png)
![WDT Active](images/WDT_ActivePower.png)

Shown above, the WDT example uses an average of 491 nA while asleep, and jumps to approximately 1.64 mA while active. 

### Interrupt-on-Change Power Consumption

![IOC Current](images/IOC_PowerMeasurment.png)
![IOC Active](images/IOC_ActivePower.png)

Shown above, the IoC example uses an average of 80nA while asleep, and jumps to approximately 1.77 mA while active. 

### ADCC/Peripheral Interrupt Power Consumption

![ADCC Threshold Active](images/ADCT_PowerMeasurement.png)
![ADCC Threshold Current](images/ADCT_PowerMeasurementConversionAverage.png)
![ADCC Threshold Active](images/ADCT_ActivePower_ConversionPower.png)

The ADCC example differes from previous examples as there's are three effective "states" for the demo: when the board is active, when the board is asleep and waiting for TMR2, and when the board is asleep and the ADCC peripheral is running a conversion. The averages are shown with the described setting from earlier, however adjusting the duration between conversions will effect overall power usage for this demo. Shown above, the IoC example uses an average of 8.71uA while asleep counting conversions, but an average of only 804nA in between each conversion. The power consumption during an ADCC conversion jumped to roughly 82.4uA (however this value does vary), and while active this example runs approximately 1.78mA.

## Summary

These examples demonstrates how to set up and use the PIC16F18076 sleep mode along with methods of waking the device after using MPLAB Melody.
