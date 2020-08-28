#ifndef TOOLS_H
#define TOOLS_H

#include "mDot.h"
#include <stdio.h>

/**
 *  make the pin 0 then 1 again rep number of times
 */
void blink(DigitalOut pin, int rep);

/* clear terminal screen */
void clear_screen();

/** 
 * detect and display battery voltage
 * not at all tested
 */
void voltage(void);



#endif