#include "synth.h"

const uint16_t notes[24] = {
    22, // C4
    24, // C#4
    25, // D4
    27, // D#4
    28, // E4
    30, // F4
    32, // F#4
    33, // G4
    35, // G#4
    38, // A4
    40, // A#4
    42, // B4
    45, // C5
    47, // C#5
    50, // D5
    53, // D#5
    56, // E5
    60, // F5
    63, // F#5
    67, // G5
    71, // G#5
    75, // A5
    80, // A#5
    84, // B5
};

inline _Q15 Q15mpy(_Q15 a, _Q15 b)
{	
    _Q15 result = 0;
    volatile register int accA __asm("A");
    accA = __builtin_clr();
    
    accA = __builtin_mpy(a, b, 0, 0, 0, 0, 0, 0);
    result = __builtin_sac(accA ,0);

    return result;
}

inline uint16_t Map_Func(uint8_t key)
{
    return notes[key - 60];
}

void Init_Synth(Synth_t* synth)
{
    Change_Osc(synth, synth->osc);
}

inline uint8_t Get_Active_Notes_Count(Synth_t* synth)
{
    uint8_t count = 0;
    
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        if(synth->active[i].step != 0)
            count++;
    }
    
    return count;
}

inline int8_t Get_Active_Note_Index(Synth_t* synth, uint16_t note)
{
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        if(synth->active[i].step == Map_Func(note) )
            return i;
    }
    return -1;
}

inline void Note_On(Synth_t* synth, uint8_t note)
{
    uint8_t n = Get_Active_Notes_Count(synth);
    int8_t exists = Get_Active_Note_Index(synth, note);
    if(n < MAX_KEYS_PRESSED && exists == -1) {
        Active_Note_t active_note = {
            .step = Map_Func(note),
            .n = 0,
            .env = {
                0,0,0,FIXED_PLUS_ONE
            }
        };
        synth->active[n] = active_note;
    }
}

inline void Note_Off(Synth_t* synth, uint8_t note)
{
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        Active_Note_t *active_note = &synth->active[i];
        
        if(active_note->step == Map_Func(note)) {            
            active_note->env.release = active_note->env.attack;
            active_note->cooldown = 1;
        }
    }
}

inline void Change_Osc(Synth_t* synth, Osc_Type_t osc)
{
    switch(osc)
    {
        case OSC_SINE:
            Gen_Data_Sin(synth->buffer);
            break;
        case OSC_SQUARE:
            Gen_Data_Square(synth->buffer);
            break;
        case OSC_SAW:
            Gen_Data_Saw(synth->buffer);
            break;
        case OSC_TRIANGLE:
            Gen_Data_Triangle(synth->buffer);
            break;
        case OSC_NOISE:
            Gen_Data_Noise(synth->buffer);
            break;
    }
}

inline void Process_ADSR(Synth_t* synth)
{
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        Active_Note_t *note = &synth->active[i];
        
        if(note->step > 0) {
            
            if(note->env.attack < FIXED_PLUS_ONE - synth->env.attack)
                note->env.attack += synth->env.attack;
            
            if(note->cooldown) {
                if(note->env.release >= synth->env.release)
                    note->env.release -= synth->env.release;
                else
                    note->env.release = 0;
            }     
        }
    }    
}

inline uint16_t Synth_Next_Sample(Synth_t* synth)
{
    _Q15 sample = 0;
    _Q15 tmp_sample = 0;
    
    /* Add one, because we don't want to divide by 0 */
    uint8_t note_count = Get_Active_Notes_Count(synth);
    
    /* Go over all notes slots */
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        
        Active_Note_t *note = &synth->active[i];
        
        /* If step != 0 then note is playing now */
        if(note->step > 0) {
            
            /* Increase phase acc and get sample value */
            note->n = (note->n + note->step) % SMP_BUF_SIZE;
            tmp_sample = synth->buffer[note->n];
            
            /* Cooldown - note released, so we wait a little bit longer for 
               - zero-crossing point, to avoid knocks 
               - release env finish
             */
            if(note->cooldown && 
               tmp_sample == synth->buffer[0] && 
               note->env.release == 0) {
                note->n = 0;
                note->step = 0;
                note->cooldown = 0;    
            }
            
            /* Multiply sample by attack (rising from start) 
               and release (at start set to 0.99, and falling from 
               key release moment) */
            tmp_sample = Q15mpy(tmp_sample, note->env.attack);
            tmp_sample = Q15mpy(tmp_sample, note->env.release);
            
            sample += tmp_sample / (note_count + 1);
           
        }
    }

    return sample;
}

void Gen_Data_Sin(_Q15 *data)
{
    const _Q15 step = (FIXED_FULL_SCALE / SMP_BUF_SIZE);
    
    for(short i=0, x = FIXED_MINUS_ONE;i<SMP_BUF_SIZE;++i) {
        *(data++) = _Q15sinPI(x);
        x += step;
    }
    
}

void Gen_Data_Square(_Q15 *data)
{
    const _Q15 step = (FIXED_FULL_SCALE / SMP_BUF_SIZE);
    const short half = SMP_BUF_SIZE / 2;
    
    for(short i=0, x = FIXED_MINUS_ONE;i<SMP_BUF_SIZE;++i) {
        if(i <= half)
            *(data++) = FIXED_MINUS_ONE;
        if(i > half)
            *(data++) = FIXED_PLUS_ONE;
        
        x += step;
    }
    
}

void Gen_Data_Saw(_Q15 *data)
{
    const _Q15 step = (FIXED_FULL_SCALE/SMP_BUF_SIZE);
   
    for(short i=0, x = FIXED_MINUS_ONE;i<SMP_BUF_SIZE;i++) {
        *(data++) = x;
        x += step;
    }
}

void Gen_Data_Triangle(_Q15 *data)
{
    const short one_three = SMP_BUF_SIZE / 3;
    const short a = FIXED_PLUS_ONE/one_three;
    short i = 0;
    _Q15 x = 0;
    
    for(i=0;i<one_three;++i) {
        *(data++) = x;
        x += a;
    }
    
    for(i=0;i<one_three;++i) {
        *(data++) = x;
        x -= 2*a;
    }
    
    for(i=0;i<one_three+2;++i) {
        *(data++) = x;
        x += a;
    }
}

void Gen_Data_Noise(_Q15 *data)
{
    const _Q15 step = ( FIXED_FULL_SCALE / SMP_BUF_SIZE );
    
    _Q15 x = FIXED_MINUS_ONE;
    for(short i=0;i<SMP_BUF_SIZE;++i) {
        *(data++) = _Q15random(x);
        x += step;
    }
}