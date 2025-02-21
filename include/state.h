#pragma once
#include "osc.h"
#include "wavetable.h"

extern const int NUM_OSCS;
extern const int NUM_WAVETABLES;
extern const int NUM_VOICES;

typedef struct {
    Osc *oscs;        // array of oscillators; size = NUM_OSCS * NUM_VOICES
    Wavetable *wts;   // shared array of wavetables; size = NUM_WAVETABLES
    int *active;      // active flag per voice (0 or 1); size = NUM_VOICES
} State;

// Allocate and initialize a new State.
State *State_create(void);
// Free all resources in a State.
void State_destroy(State *state);

// Set a note on a given voice (all oscillators in that voice).
void State_set_note(State *state, int voice, double freq);
// Clear (turn off) a given voice.
void State_clear_voice(State *state, int voice);

// Mix and return one sample by processing all active voices.
float State_mix_sample(State *state);
