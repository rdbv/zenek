#ifndef _SYNTH_H
#define _SYNTH_H

#include "midi.h"
#include <libq.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// _Q15 Definitions
#define FIXED_MINUS_ONE -32768
#define FIXED_PLUS_ONE   32767
#define FIXED_FULL_SCALE 65536

#define MAX_KEYS_PRESSED    8
#define MAX_CHANNELS        4
#define LPF_FILTER_SIZE     16

enum {
    NOTE_IDLE,
    NOTE_STARTING,
    NOTE_COOLDOWN
} typedef Note_State_t;

struct {
    uint16_t attack, release;
} typedef Env_t;

struct {
    uint16_t value;
} typedef LFO_t;

struct {
    _Q15 data[LPF_FILTER_SIZE];
    _Q15 last;
    uint16_t n;
} typedef LP_Filter_t;

struct {
    Note_State_t state;
    Env_t env;
    
    uint16_t step, n;
    uint8_t channel, velocity;
} typedef Active_Note_t;

struct {
    Env_t env;
    LFO_t lfo;
    LP_Filter_t lpf;

    Active_Note_t active[MAX_KEYS_PRESSED];
    uint8_t active_count;
} typedef Synth_t;


uint16_t Map_Func(uint8_t);

void Note_On(Synth_t*, MIDI_Msg_t*);
void Note_Off(Synth_t*, MIDI_Msg_t*);

void Process_Envelope(Synth_t*);
void Process_LFO(Synth_t*);

int8_t Get_Active_Note_Index(Synth_t*, uint16_t);
uint8_t Get_Active_Notes_Count(Synth_t*);
uint16_t Synth_Next_Sample(Synth_t*);

_Q15 _Q15mpy(_Q15,_Q15);
_Q15 _Q15square(_Q15);
_Q15 _Q15saw(_Q15);
_Q15 _Q15triangle(_Q15);

#endif

