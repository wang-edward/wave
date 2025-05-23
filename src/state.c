#include "state.h"
#include "config.h"
#include "filter.h"
#include <assert.h>
#include <stdlib.h>

const int NUM_OSCS = 4;
const int NUM_WAVETABLES = 4;
const int NUM_VOICES = 12; // increased to cover all keys

State *State_create(void) {
    State *state = malloc(sizeof(State));
    assert(state);
    state->oscs = malloc(NUM_VOICES * NUM_OSCS * sizeof(Osc));
    assert(state->oscs);
    state->wts = malloc(NUM_WAVETABLES * sizeof(Wavetable));
    assert(state->wts);
    state->wt_levels = malloc(NUM_WAVETABLES * sizeof(float));
    assert(state->wt_levels);
    for (int i = 0; i < NUM_VOICES * NUM_OSCS; i++) {
        state->oscs[i] = Osc_create(i % 4, 0); // TODO generalize to n wavetables
    }
    for (int i = 0; i < NUM_WAVETABLES; i++) {
        state->wt_levels[i] = 1.0f;
    }
    state->active = malloc(NUM_VOICES * sizeof(int));
    assert(state->active);
    for (int i = 0; i < NUM_VOICES; i++) {
        state->active[i] = 0;
    }
    // Create shared wavetables.
    state->wts[WAVEFORM_SINE] = *Wavetable_create(WAVEFORM_SINE, TABLE_SIZE);
    state->wts[WAVEFORM_SAW] = *Wavetable_create(WAVEFORM_SAW, TABLE_SIZE);
    state->wts[WAVEFORM_SQUARE] = *Wavetable_create(WAVEFORM_SQUARE, TABLE_SIZE);
    // state->wts[WAVEFORM_TRIANGLE] = *Wavetable_create(WAVEFORM_TRIANGLE, TABLE_SIZE);
    state->wts[WAVEFORM_TRIANGLE] = *Wavetable_create(WAVEFORM_CUSTOM, TABLE_SIZE);
    Wavetable_load(&state->wts[WAVEFORM_TRIANGLE], "Trumpet.bin");

    Lowpass_init(&state->lpf);
    return state;
}

void State_destroy(State *state) {
    if (!state)
        return;
    for (int i = 0; i < NUM_WAVETABLES; i++) {
        Wavetable_destroy(&state->wts[i]);
    }
    free(state->wts);
    free(state->wt_levels);
    free(state->oscs);
    free(state->active);
    free(state);
}

void State_set_note(State *state, int voice, double freq) {
    if (voice < 0 || voice >= NUM_VOICES)
        return;
    state->active[voice] = 1;
    for (int i = 0; i < NUM_OSCS; i++) {
        int idx = voice * NUM_OSCS + i;
        state->oscs[idx].phase = 0.0;
        state->oscs[idx].phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
    }
}

void State_clear_voice(State *state, int voice) {
    if (voice < 0 || voice >= NUM_VOICES)
        return;
    state->active[voice] = 0;
    for (int i = 0; i < NUM_OSCS; i++) {
        int idx = voice * NUM_OSCS + i;
        state->oscs[idx].phase_inc = 0.0;
    }
}

float State_mix_sample(State *state) {
    float mix = 0.0f;
    for (int voice = 0; voice < NUM_VOICES; voice++) {
        if (state->active[voice]) {
            float voice_sum = 0.0f;
            for (int i = 0; i < NUM_OSCS; i++) {
                int idx = voice * NUM_OSCS + i;
                Osc *osc = &state->oscs[idx];
                Wavetable *wt = &state->wts[osc->wt_index];
                size_t len = wt->length;
                double pos = osc->phase;
                int index0 = (int)pos;
                int index1 = (index0 + 1) % len;
                double frac = pos - index0;
                float sample = (float)((1.0 - frac) * wt->data[index0] + frac * wt->data[index1]);
                sample *= state->wt_levels[osc->wt_index];
                voice_sum += sample;
                osc->phase += osc->phase_inc;
                if (osc->phase >= len)
                    osc->phase -= len;
            }
            voice_sum /= NUM_OSCS;
            mix += voice_sum;
        }
    }
    return mix;
}
