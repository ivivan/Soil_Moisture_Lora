#include "tools.h"
#include "mDot.h"
#include <stdio.h>


void blink(DigitalOut pin, int rep) {
    for (int j = 0; j < rep; j++) {
        pin = 0;
        wait(0.3);
        pin = 1;
        wait(0.3);
    } 
}

void clear_screen() {
    puts("\033[2J");
    puts("\033[0;0H");
    
}

/* attempting to use internal reference voltage, not sure if need external
   comes partly from microcontroller datasheet (STM32F411xE data sheet, under
    the section 'Battery charge monitering' and 'Calculating the actual VDDA 
    voltage using the internal reference voltage')
*/
void voltage(void) {
    AnalogIn vref(ADC_VREF);
    // maybe wait for interrupt?
    /*
    VDDA = 3 * VREFINT_CAL / VREFINT_DATA
    VDD = VREFINT_CAL / VREFINT(ADC float)
    */
    double VREF = vref.read();
    double VREF16 = vref.read_u16();

    // Address from VREFINT_CAL_ADDR which is in the library but won't load
    uint16_t vref_cal = *(__IO uint16_t *)((uint32_t) 0x1FFFF7BAU);
   
   // vref cal is actually 1.224 but for some reason it doesnt work in calc
    float vdd = (1.224)/VREF;
    printf("Voltage: %f \n", vdd);
}

