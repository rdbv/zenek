#include "hardware.h"

#pragma config FNOSC    = FRC
#pragma config IESO     = OFF
#pragma config FCKSM    = CSECMD
#pragma config OSCIOFNC = OFF
#pragma config POSCMD   = XT
#pragma config FWDTEN   = OFF
#pragma config FPWRT    = PWR128

// Not used in this project - for clearity
void Init_Osc_RC()
{
    PLLFBD = 38;
    
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 0;
    
    RCONbits.SWDTEN = 0;
    
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    
    while(OSCCONbits.COSC != 0b001);
    while(OSCCONbits.LOCK != 1);
}

/*
    http://ww1.microchip.com/downloads/en/DeviceDoc/70186E.pdf
    All info on page 18+ 
 */
void Init_Osc_XT()
{
    PLLFBD = PLL_FBD;
    
    CLKDIVbits.PLLPRE = PLL_PRE; 
    CLKDIVbits.PLLPOST = PLL_POST; 
    
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
        
    while(OSCCONbits.COSC != 0b011);
    while(OSCCONbits.LOCK != 1);
}

void Init_Timer1()
{
    T2CONbits.TON = 0;
    T2CONbits.TCS = 0;
    T2CONbits.TGATE = 0;
    T2CONbits.TCKPS = 0b00;

    TMR2 = 0x0000;
    PR2 = TIMER_FREQUENCY(1000, 1);
    
    IPC1bits.T2IP = 1;
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;
    
    T2CONbits.TON = 1;
}

void Init_DAC()
{
    
    ACLKCONbits.SELACLK = 0;            // DACCLK Settings
    ACLKCONbits.AOSCMD = 0;
    ACLKCONbits.APSTSCLR = 0b111;
    ACLKCONbits.ASRCSEL = 1;
    
    DAC1STATbits.ROEN = 0;              // Channels enabled?
    DAC1STATbits.LOEN = 1;
    
    DAC1STATbits.RITYPE = 0;            // Channels interrupts?
    DAC1STATbits.LITYPE = 0;
    
    DAC1CONbits.AMPON = 0;              // Amp, DACCLK Divider, data form 
    DAC1CONbits.DACFDIV = DAC_DIV - 1;
    DAC1CONbits.FORM = 1;
    
    IFS4bits.DAC1LIF = 0;               // Clear DAC Interrupt flags 
    IEC4bits.DAC1LIE = 1;
    
    DAC1DFLT = 0;                       // Default DAC Value 
    
    DAC1CONbits.DACEN = 1;              // Default DAC Value 
}

void Init_UART()
{
    RPINR18bits.U1RXR = 7;      // RP7 as UART Input
    RPOR3bits.RP6R = 3;         // RP6 as UART Output
        
    U1MODEbits.STSEL = 0;       // Main UART Configuration
    U1MODEbits.PDSEL = 0;
    U1MODEbits.ABAUD = 0;
    U1MODEbits.BRGH = 0;
    
    U1BRG = BRGVAL;             // UART Speed configuration

    IFS0bits.U1RXIF = 0;        // Interrupt configuration
    IPC2bits.U1RXIP = 1;
    U1STAbits.URXISEL = 0b10;
    IEC0bits.U1RXIE = 1;
    
    U1MODEbits.UARTEN = 1;      // Enable UART module
    U1STAbits.UTXEN = 1;
}

void Init_GPIO()
{
    TRISBbits.TRISB8 = 0;
    PORTBbits.RB8 = 0;
}

