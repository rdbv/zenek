#include "synth.h"

/* Channels lookup */
_Q15 (*channels[MAX_CHANNELS])(_Q15) = {
    _Q15saw,
    _Q15square,
    _Q15sinPI,
    _Q15triangle
};

/* Note frequency lookup */
const _Q15 notes[84] = {
      44,  47,  50,  53,  56,  59,  63,  66,  70,  75,  79,  84,  // A1 A#1 B1 C1 C#1 D1 D#1 E1 F1 F#1 G1 G#1 
      89,  94, 100, 106, 112, 119, 126, 133, 141, 150, 159, 168,  // A2 A#2 B2 C2 C#2 D2 D#2 E2 F2 F#2 G2 G#2 
     178, 189, 200, 212, 225, 238, 252, 267, 283, 300, 318, 337,  // A3 A#3 B3 C3 C#3 D3 D#3 E3 F3 F#3 G3 G#3 
     357, 378, 400, 424, 450, 476, 505, 535, 567, 600, 636, 674,  // A4 A#4 B4 C4 C#4 D4 D#4 E4 F4 F#4 G4 G#4 
     714, 756, 801, 849, 900, 953,1010,1070,1134,1201,1272,1348,  // A5 A#5 B5 C5 C#5 D5 D#5 E5 F5 F#5 G5 G#5 
    1428,1513,1603,1699,1800,1907,2020,2140,2268,2402,2545,2697,  // A6 A#6 B6 C6 C#6 D6 D#6 E6 F6 F#6 G6 G#6 
    2857,3027,3207,3398,3600,3814,4041,4281,4536,4805,5091,5394,  // A7 A#7 B7 C7 C#7 D7 D#7 E7 F7 F#7 G7 G#7 
};

inline uint16_t __attribute__((always_inline)) Map_Func(uint8_t key)
{
    return notes[key - 24];
}

inline uint8_t __attribute__((always_inline)) Get_Active_Notes_Count(Synth_t* synth)
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

inline void Note_On(Synth_t* synth, MIDI_Msg_t* msg)
{    
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        Active_Note_t *note = &synth->active[i];
        
        if(note->step == 0) {
            note->step = Map_Func(msg->data[0]);
            note->n = 0;
            note->channel = msg->channel;
            note->velocity = msg->data[1];
            note->env.attack = 0;
            break;
        }
        
    }
}

inline void Note_Off(Synth_t* synth, MIDI_Msg_t* msg)
{
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        Active_Note_t *active_note = &synth->active[i];
        
        if(active_note->step == Map_Func(msg->data[0]) 
        && active_note->channel == msg->channel) {  
            //active_note->env.release = active_note->env.attack;
            active_note->state = NOTE_COOLDOWN;
        }   
    } 
}

inline void Process_Envelope(Synth_t* synth)
{
    /* Go over all note slots */
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        Active_Note_t *note = &synth->active[i];
        if(note->step > 0) {
            /* Attack will rise a bit almost to 0.99 
               but we don't want to overshoot and end with ~ -0.99 */
            if(note->env.attack < FIXED_PLUS_ONE - synth->env.attack)
                note->env.attack += synth->env.attack;
            
            /* If note was released, subtract from release for note 
               until next subtraction dont effect with ending with 
               negative value - then just set 0 */
            if(note->state == NOTE_COOLDOWN) {
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
    uint8_t note_count = Get_Active_Notes_Count(synth);    
    _Q15 sample = 0;
    
    /* Go over all notes slots */
    for(uint8_t i=0;i<MAX_KEYS_PRESSED;++i) {
        
        _Q15 tmp_sample = 0;
        Active_Note_t *note = &synth->active[i];
        
        /* If step != 0 then note is playing now */
        if(note->step != 0) {
            tmp_sample = (*channels[note->channel])(note->n);
            note->n += note->step;

            tmp_sample = _Q15mpy(tmp_sample, note->velocity * 255 );
            tmp_sample = _Q15mpy(tmp_sample, note->env.attack);
            
            //tmp_sample = _Q15mpy(tmp_sample, note->env.release);
            
            sample += tmp_sample / (note_count + 1);

            if(note->state == NOTE_COOLDOWN) {
                note->step = 0;
                note->state = NOTE_IDLE;  
            }
        }
    }

    return sample;
}

inline _Q15 __attribute__((always_inline)) _Q15mpy(_Q15 a, _Q15 b)
{	
    _Q15 result = 0;
    volatile register int accA __asm("A");
    accA = __builtin_clr();
    
    accA = __builtin_mpy(a, b, 0, 0, 0, 0, 0, 0);
    result = __builtin_sac(accA ,0);

    return result;
}

inline _Q15 __attribute__((always_inline)) _Q15square(_Q15 x)
{
    if(x <= 0)
        return FIXED_MINUS_ONE;
    else
        return FIXED_PLUS_ONE;
}

inline _Q15 __attribute__((always_inline)) _Q15saw(_Q15 x)
{
    return x;
}

inline _Q15 __attribute__((always_inline)) _Q15triangle(_Q15 x)
{
    if(x <= 0)
        return x;
    else
        return 0-x;
}