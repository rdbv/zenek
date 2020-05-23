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

MIDI_Rx_t MIDI = {
    .state = WAITING_CMD
};

Synth_t Synth = {
    .osc = OSC_SINE,
    .env = {
        .attack = 32 * 2,
        .release = 32 * 4
    }, 
    .lfo0 = 32767,
    .lpf = {
        .n = 0
    }
};
        
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt()
{
    IFS0bits.U1RXIF = 0;
    
    MIDI_Recv_Packet(&MIDI);    

    if(MIDI.msg.cmd == MIDI_NOTE_ON)
    {
        Note_On(&Synth, MIDI.msg.data[0]);
    }
    if(MIDI.msg.cmd == MIDI_NOTE_OFF)
    {
        Note_Off(&Synth, MIDI.msg.data[0]);
    }
    if(MIDI.msg.cmd == MIDI_CHG_OSC)
    {
        Change_Osc(&Synth, MIDI.msg.data[0]);
    }
    /*
    printf("C:%02x B0:%04d B1:%02x C:%02d", 
        MIDI.msg.cmd, 
        MIDI.msg.data[0],
        MIDI.msg.data[1],
        Get_Active_Notes_Count(&Synth) );
    
    
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i)
    {
        printf(" %04x ", Synth.active[i].step);
    }
    printf("\r\n");
    */
    if(U1STAbits.OERR == 1)
    {
        U1STAbits.OERR = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _DAC1LInterrupt()
{
    IFS4bits.DAC1LIF = 0;    
    DAC1LDAT = Synth_Next_Sample(&Synth);
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt()
{
    //PORTBbits.RB8 ^= 1;
    
    Process_ADSR(&Synth);    
    IFS0bits.T2IF = 0;
}

extern void test();

int main(void) 
{
    //CORCONbits.IF = 0;
    
    /* CPU/DAC @ 36.84 MIPS / 48 kHz  */
    Init_Osc_XT();    
    Init_DAC();
    Init_Timer1();
    Init_UART();    
    Init_GPIO();
    
    Init_Synth(&Synth);

    
    while(1) 
    {
        /*
        Active_Note_t an;
        
        an.step = 22;
        an.n = 0;
        Synth.active[0] = an;
        __delay_ms(1000);
        
        an.step = 0;
        an.n = 0;
        Synth.active[0] = an;
        __delay_ms(1000);
        
        an.step = 25;
        an.n = 0;
        Synth.active[0] = an;
        __delay_ms(1000);
        */
        
        //printf(".");
        //PORTBbits.RB8 ^= 1;
        
        //step += 1;
        //if(step > 256)
        //    step = 1;  
        
   
        
        //U1TXREG = step;
        
    }

}
