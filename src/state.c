#include "state.h"
#include "config.h"
#include <stdlib.h>
#include <assert.h>

const int NUM_OSCS = 4;
const int NUM_WAVETABLES = 4;
const int NUM_VOICES = 8;

State *State_create(void) {
    State *state = malloc(sizeof(State));
    assert(state);
    state->oscs = calloc(NUM_OSCS * NUM_VOICES, sizeof(Osc));
    assert(state->oscs);
    state->wts = malloc(NUM_WAVETABLES * sizeof(Wavetable));
    assert(state->wts);
    state->active = calloc(NUM_VOICES, sizeof(int));
    assert(state->active);
    // Create shared wavetables for each waveform type.
    state->wts[WAVEFORM_SINE] = *Wavetable_create(WAVEFORM_SINE, TABLE_SIZE);
    state->wts[WAVEFORM_SAW] = *Wavetable_create(WAVEFORM_SAW, TABLE_SIZE);
    state->wts[WAVEFORM_SQUARE] = *Wavetable_create(WAVEFORM_SQUARE, TABLE_SIZE);
    state->wts[WAVEFORM_TRIANGLE] = *Wavetable_create(WAVEFORM_TRIANGLE, TABLE_SIZE);

    // Initialize each oscillator.
    // For each voice, assign its NUM_OSCS oscillators.
    for (int voice = 0; voice < NUM_VOICES; voice++) {
        // Initially, voice is off.
        state->active[voice] = 0;
        for (int i = 0; i < NUM_OSCS; i++) {
            int idx = voice * NUM_OSCS + i;
            state->oscs[idx].phase = 0.0;
            // Default: assign each oscillator a wavetable index corresponding to i.
            state->oscs[idx].wt_index = i % NUM_WAVETABLES;
            state->oscs[idx].phase_inc = 0.0; // note off by default
        }
    }
    return state;
}

void State_destroy(State *state) {
    if (!state) return;
    // Free shared wavetables.
    for (int i = 0; i < NUM_WAVETABLES; i++) {
        Wavetable_destroy(&state->wts[i]);
    }
    free(state->wts);
    free(state->oscs);
    free(state->active);
    free(state);
}

void State_set_note(State *state, int voice, double freq) {
    if (voice < 0 || voice >= NUM_VOICES) return;
    state->active[voice] = 1;
    for (int i = 0; i < NUM_OSCS; i++) {
        int idx = voice * NUM_OSCS + i;
        state->oscs[idx].phase = 0.0; // reset phase on note-on
        state->oscs[idx].phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    }
}

void State_clear_voice(State *state, int voice) {
    if (voice < 0 || voice >= NUM_VOICES) return;
    state->active[voice] = 0;
    for (int i = 0; i < NUM_OSCS; i++) {
        int idx = voice * NUM_OSCS + i;
        state->oscs[idx].phase_inc = 0.0;
    }
}

float State_mix_sample(State *state) {
    float mix = 0.0f;
    int active_count = 0;
    for (int voice = 0; voice < NUM_VOICES; voice++) {
        if (state->active[voice]) {
            float voice_sum = 0.0f;
            for (int i = 0; i < NUM_OSCS; i++) {
                int idx = voice * NUM_OSCS + i;
                Osc *osc = &state->oscs[idx];
                Wavetable *wt = &state->wts[osc->wt_index];
                size_t len = wt->length;
                double pos = osc->phase;
                int index0 = (int) pos;
                int index1 = (index0 + 1) % len;
                double frac = pos - index0;
                float sample = (float)((1.0 - frac) * wt->data[index0] + frac * wt->data[index1]);
                voice_sum += sample;
                osc->phase += osc->phase_inc;
                if (osc->phase >= len)
                    osc->phase -= len;
            }
            voice_sum /= NUM_OSCS;
            mix += voice_sum;
            active_count++;
        }
    }
    if (active_count > 0)
        mix /= active_count;
    return mix;
}
