#include "midi.h"

inline void MIDI_Recv_Packet(MIDI_Rx_t* rx)
{
    MIDI_Recv_Initial(rx);
    MIDI_Recv_Data(rx);      
}

inline void __attribute__((always_inline)) MIDI_Recv_Initial(MIDI_Rx_t* rx)
{
    uint8_t byte = U1RXREG;
    
    uint8_t cmd = byte & (MIDI_CMD_MASK);
    uint8_t chn = byte & (MIDI_CHANNEL_MASK);
    
    rx->msg.cmd = cmd;
    rx->msg.channel = chn;
    
    switch(cmd)
    {
        case MIDI_NOTE_ON:
        case MIDI_NOTE_OFF:
        case MIDI_CHG_OSC:
            rx->state = WAITING_2_BYTES;
            break;
        
        default:
            rx->state = WAITING_CMD;
            break;
    }
}

inline void __attribute__((always_inline)) MIDI_Recv_Data(MIDI_Rx_t* rx)
{
    switch(rx->state)
    {
        case WAITING_2_BYTES:
            rx->msg.data[rx->state] = U1RXREG;
            rx->state++;
            
        case WAITING_1_BYTES:
            rx->msg.data[rx->state] = U1RXREG;
            rx->state = WAITING_CMD;
            break;
            
        default:
            break;
    }
}
