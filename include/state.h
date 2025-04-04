#pragma once
#include "osc.h"
#include "wavetable.h"
#include "filter.h"

// Fixed constants.
extern const int NUM_OSCS;       // oscillators per voice (e.g., 4)
extern const int NUM_WAVETABLES; // number of shared wavetables (e.g., 4)
extern const int NUM_VOICES;     // maximum polyphony (e.g., 8)

typedef struct {
    Osc *oscs;        // array of oscillators; size = NUM_VOICES * NUM_OSCS
    Wavetable *wts;   // shared array of NUM_WAVETABLES wavetables
    float *wt_levels; // per-wavetable level multipliers; array of NUM_WAVETABLES floats
    int *active;      // for each voice (size NUM_VOICES), 1 if active, 0 if not
    LowpassFilter lpf;
} State;

State *State_create(void);
void State_destroy(State *state);

// For a given voice (0-indexed), set the note (all oscillators in that voice).
void State_set_note(State *state, int voice, double freq);
// Clear (turn off) a given voice.
void State_clear_voice(State *state, int voice);

// Mix and return one audio sample.
float State_mix_sample(State *state);
