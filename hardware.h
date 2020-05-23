#ifndef HARDWARE_H
#define	HARDWARE_H

#define FCY      39936000
//36864000

#define BAUDRATE 38400
#define BRGVAL   ((FCY/BAUDRATE)/16)-1

#define PLL_FBD  63
//40
#define PLL_PRE  3
//2
#define PLL_POST 2
//2

#define DAC_DIV  13


#define T1_PRESCALER 256
#define T1_FRQ(f) (FCY/T1_PRESCALER/f)
#define T1_FRQX 120

// 12

#include <dac.h>
#include <stdint.h>
#include <p33Fxxxx.h>
#include <libpic30.h>

void Init_Osc_RC();
void Init_Osc_XT();
void Init_Timer1();
void Init_DAC();
void Init_UART();
void Init_GPIO();

#endif
 


