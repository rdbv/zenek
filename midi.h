#ifndef _MIDI_H
#define _MIDI_H

#include <stdio.h>
#include <stdint.h>
#include <p33Fxxxx.h>

#define MIDI_CMD_MASK     0xf0
#define MIDI_CHANNEL_MASK 0x0f

/* Standard MIDI Messages */
#define MIDI_NOTE_ON      0x90
#define MIDI_NOTE_OFF     0x80

/* Custom MIDI Messages */
#define MIDI_CHG_OSC      0x10

#define MIDI_KEY_MIN      0x00
#define MIDI_KEY_MAX      0x7f
#define MIDI_VEL_MIN      0x00
#define MIDI_VEL_MAX      0x7f

#define RECIEVED_PACKET   0
#define WAITING_PACKET    1

enum {
    WAITING_2_BYTES     = 0,
    WAITING_1_BYTES     = 1,
    WAITING_CMD         = 2
} typedef MIDI_State_t;

struct {
    uint8_t cmd;
    uint8_t channel;
    uint8_t data[2];
} typedef MIDI_Msg_t;

struct {
    MIDI_State_t state;
    MIDI_Msg_t   msg;
} typedef MIDI_Rx_t;

void MIDI_Recv_Packet(MIDI_Rx_t*);
void MIDI_Recv_Initial(MIDI_Rx_t*);
void MIDI_Recv_Data(MIDI_Rx_t*);

#endif
