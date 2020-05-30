#include <xc.h>
#include <p33Fxxxx.h>
#include <timer.h>
#include <dsp.h>
#include <libq.h>
#include <stdint.h>
#include <stdio.h>

#include "hardware.h"
#include "synth.h"
#include "midi.h"

#define UART_DEBUG_MIDI 0

MIDI_Rx_t MIDI = {
    .state = WAITING_CMD,
};

Synth_t Synth = {

    .env = {
        .attack = 32 * 8,
        .release = 32 * 3
    }, 

    .lpf = {
        .n = 0
    }
};
        
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt()
{    
    MIDI_Recv_Packet(&MIDI);    
    
    if(MIDI.msg.cmd == MIDI_NOTE_ON && MIDI.msg.data[1] == 0x00) {
        Note_Off(&Synth, &MIDI.msg);
    }   
    else if(MIDI.msg.cmd == MIDI_NOTE_ON) {
        Note_On(&Synth, &MIDI.msg);
    } 
    else if(MIDI.msg.cmd == MIDI_NOTE_OFF) {
        Note_Off(&Synth, &MIDI.msg);
    }
    
    
#if UART_DEBUG_MIDI == 1
    
    printf("C:%02x B0:%04d B1:%02x C:%02d\r\n", 
        MIDI.msg.cmd, 
        MIDI.msg.data[0],
        MIDI.msg.data[1],
        Get_Active_Notes_Count(&Synth) );

    
#endif

    //if(U1STAbits.OERR == 1)
    //    U1STAbits.OERR = 0;

    IFS0bits.U1RXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _DAC1LInterrupt()
{
    DAC1LDAT = Synth_Next_Sample(&Synth);
    IFS4bits.DAC1LIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt()
{   
    Process_Envelope(&Synth);    
    IFS0bits.T2IF = 0;
}

extern void test();

int main(void) 
{
    /* 
       CPU          @ 39.93 MIPS 
       DAC          @ 48 kHz  
       T2 Interrupt @ ~1kHz 
     */
    
    Init_Osc_XT();    
    Init_DAC();
    Init_Timer1(); 
    Init_UART();    
    Init_GPIO();

    while(1) 
    {

    }

}
