#include "midi.h"

void MIDI_Recv_Packet(MIDI_Rx_t* rx)
{
    MIDI_Recv_Initial(rx);
    MIDI_Recv_Data(rx);      
}

inline void MIDI_Recv_Initial(MIDI_Rx_t* rx)
{
    uint8_t cmd = U1RXREG & (MIDI_CMD_MASK);
    rx->msg.cmd = cmd;
    
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

inline void MIDI_Recv_Data(MIDI_Rx_t* rx)
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

inline void MIDI_Dump_Packet(MIDI_Msg_t* msg)
{
    printf("C:%02x B0:%02x B1:%02x\r\n", 
            msg->cmd, 
            msg->data[0],
            msg->data[1]);
}
