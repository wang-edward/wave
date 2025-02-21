#include <math.h>
#include <pthread.h>
#include <raylib.h>
#include <soundio/soundio.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "state.h"

// Global mutex for protecting synth state.
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

// Audio write callback.
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min,
                           int frame_count_max) {
    (void)frame_count_min;
    State *state = (State *)outstream->userdata;
    int frames_left = frame_count_max;
    while (frames_left > 0) {
        int frame_count = frames_left;
        struct SoundIoChannelArea *areas;
        int err = soundio_outstream_begin_write(outstream, &areas, &frame_count);
        if (err) {
            fprintf(stderr, "begin write error: %s\n", soundio_strerror(err));
            exit(1);
        }
        if (frame_count == 0)
            break;
        for (int frame = 0; frame < frame_count; frame++) {
            float sample = 0.0f;
            pthread_mutex_lock(&state_mutex);
            sample = State_mix_sample(state);
            pthread_mutex_unlock(&state_mutex);
            for (int ch = 0; ch < outstream->layout.channel_count; ch++) {
                char *ptr = areas[ch].ptr + areas[ch].step * frame;
                *((float *)ptr) = sample;
            }
        }
        err = soundio_outstream_end_write(outstream);
        if (err) {
            fprintf(stderr, "end write error: %s\n", soundio_strerror(err));
            exit(1);
        }
        frames_left -= frame_count;
    }
}

// Note mapping for a full piano octave (including sharps/flats) based on QWERTY keys.
// We'll use a common mapping for one octave starting at C3:
// White keys: Z (C3), X (D3), C (E3), V (F3), B (G3), N (A3), M (B3), Comma (C4)
// Black keys: S (C#3), D (D#3), G (F#3), H (G#3), J (A#3)
typedef struct {
    int key;           // Raylib key code.
    int semitone_offset; // Semitone offset from base note C3.
} NoteMapping;

const int NUM_NOTE_KEYS = 13;
const NoteMapping note_keys[13] = {
    { KEY_Z,  0 },  // C3
    { KEY_S,  1 },  // C#3
    { KEY_X,  2 },  // D3
    { KEY_D,  3 },  // D#3
    { KEY_C,  4 },  // E3
    { KEY_V,  5 },  // F3
    { KEY_G,  6 },  // F#3
    { KEY_B,  7 },  // G3
    { KEY_H,  8 },  // G#3
    { KEY_N,  9 },  // A3
    { KEY_J, 10 },  // A#3
    { KEY_M, 11 },  // B3
    { KEY_COMMA, 12 } // C4
};

// Active voice mapping for each note key. -1 means not active.
int active_voice[NUM_NOTE_KEYS];

int main(void) {
    // Initialize synth state.
    State *state = State_create();
    for (int i = 0; i < NUM_NOTE_KEYS; i++) {
        active_voice[i] = -1;
    }
    // Base frequency for C3.
    const double base_freq = 130.81;
    const double semitone_ratio = pow(2.0, 1.0/12.0);

    // Initialize SoundIo.
    struct SoundIo *soundio = soundio_create();
    if (!soundio) { fprintf(stderr, "Out of memory.\n"); return 1; }
    int err = soundio_connect(soundio);
    if (err) { fprintf(stderr, "Error connecting: %s\n", soundio_strerror(err)); return 1; }
    soundio_flush_events(soundio);
    int default_out_device_index = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0) { fprintf(stderr, "No output device found.\n"); return 1; }
    struct SoundIoDevice *device = soundio_get_output_device(soundio, default_out_device_index);
    if (!device) { fprintf(stderr, "Unable to get output device.\n"); return 1; }
    printf("Using output device: %s\n", device->name);

    // Create and configure output stream.
    struct SoundIoOutStream *outstream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;
    outstream->sample_rate = SAMPLE_RATE;
    if (device->layout_count > 0) {
        int found = 0;
        for (int i = 0; i < device->layout_count; i++) {
            if (device->layouts[i].channel_count == 2) {
                outstream->layout = device->layouts[i];
                found = 1;
                break;
            }
        }
        if (!found)
            outstream->layout = device->layouts[0];
    } else {
        outstream->layout = *soundio_channel_layout_get_default(2);
    }
    outstream->userdata = state;
    outstream->write_callback = write_callback;
    err = soundio_outstream_open(outstream);
    if (err) { fprintf(stderr, "Error opening stream: %s\n", soundio_strerror(err)); return 1; }
    if (outstream->layout_error)
        fprintf(stderr, "Channel layout error: %s\n", soundio_strerror(outstream->layout_error));
    err = soundio_outstream_start(outstream);
    if (err) { fprintf(stderr, "Error starting stream: %s\n", soundio_strerror(err)); return 1; }

    // Initialize Raylib window.
    InitWindow(640, 480, "Polyphonic Synthesizer");
    SetTargetFPS(60);

    // Main loop.
    while (!WindowShouldClose()) {
        pthread_mutex_lock(&state_mutex);
        // For each mapped note key, if the key is down then ensure a voice is playing that note,
        // otherwise clear the voice.
        for (int i = 0; i < NUM_NOTE_KEYS; i++) {
            if (IsKeyDown(note_keys[i].key)) {
                if (active_voice[i] == -1) {
                    double freq = base_freq * pow(semitone_ratio, note_keys[i].semitone_offset);
                    // We simply use the note mapping index as the voice index.
                    // (This assumes NUM_NOTE_KEYS <= NUM_VOICES.)
                    active_voice[i] = i;
                    State_set_note(state, active_voice[i], freq);
                }
            } else {
                if (active_voice[i] != -1) {
                    State_clear_voice(state, active_voice[i]);
                    active_voice[i] = -1;
                }
            }
        }
        // Process wavetable level adjustments.
        // Map keys 1-8: KEY_ONE increases wt[0], KEY_TWO decreases wt[0],
        // KEY_THREE increases wt[1], KEY_FOUR decreases wt[1], etc.
        if (IsKeyPressed(KEY_ONE)) state->wt_levels[0] += 0.1f;
        if (IsKeyPressed(KEY_TWO)) state->wt_levels[0] -= 0.1f;
        if (IsKeyPressed(KEY_THREE)) state->wt_levels[1] += 0.1f;
        if (IsKeyPressed(KEY_FOUR)) state->wt_levels[1] -= 0.1f;
        if (IsKeyPressed(KEY_FIVE)) state->wt_levels[2] += 0.1f;
        if (IsKeyPressed(KEY_SIX)) state->wt_levels[2] -= 0.1f;
        if (IsKeyPressed(KEY_SEVEN)) state->wt_levels[3] += 0.1f;
        if (IsKeyPressed(KEY_EIGHT)) state->wt_levels[3] -= 0.1f;
        pthread_mutex_unlock(&state_mutex);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Piano Keys (C3 to C4): Z, S, X, D, C, V, G, B, H, N, J, M, COMMA", 10, 10, 20, DARKGRAY);
        DrawText("Adjust Levels: 1/2 for wt[0], 3/4 for wt[1], 5/6 for wt[2], 7/8 for wt[3]", 10, 40, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
    State_destroy(state);
    return 0;
}
