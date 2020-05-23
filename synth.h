#ifndef _SYNTH_H
#define _SYNTH_H

#include <libq.h>
#include <stdint.h>
#include <stdbool.h>

// _Q15 Definitions
#define FIXED_MINUS_ONE -32768
#define FIXED_PLUS_ONE   32767
#define FIXED_FULL_SCALE 65536

// Size of synth buffer
#define SMP_BUF_SIZE     (1024 * 4)

// Max keys pressed
#define MAX_KEYS_PRESSED 6

#define LPF_FILTER_SIZE 16

enum {
    OSC_SINE,
    OSC_SQUARE,
    OSC_SAW,
    OSC_TRIANGLE,
    OSC_NOISE
} typedef Osc_Type_t;

struct {
    uint16_t attack, release;
} typedef Env_t;

struct {
    _Q15 data[LPF_FILTER_SIZE];
    _Q15 last;
    uint16_t n;
} typedef LP_Filter_t;

struct {
    bool cooldown;
    Env_t env;
    int16_t step;
    uint16_t n;
} typedef Active_Note_t;

struct {
    Osc_Type_t osc;
    Env_t env;
    LP_Filter_t lpf;
    
    _Q15 buffer[SMP_BUF_SIZE];   
    _Q15 lfo0;
    _Q15 lfo_i0;
    
    Active_Note_t active[MAX_KEYS_PRESSED];
} typedef Synth_t;


void Init_Synth(Synth_t* synth);
void Note_On(Synth_t*, uint8_t);
void Note_Off(Synth_t*, uint8_t);
void Change_Osc(Synth_t*, Osc_Type_t);
void Process_Envelope(Synth_t*);

int8_t Get_Active_Note_Index(Synth_t*, uint16_t);
uint8_t Get_Active_Notes_Count(Synth_t*);
uint16_t Synth_Next_Sample(Synth_t*);

void Gen_Data_Sin(_Q15 *);
void Gen_Data_Square(_Q15 *);
void Gen_Data_Saw(_Q15 *);
void Gen_Data_Triangle(_Q15 *);
void Gen_Data_Noise(_Q15 *);

#endif

