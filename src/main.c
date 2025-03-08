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

// --- New key mapping for one octave ---
//
// White keys (naturals):
//   A -> C3 (offset 0)
//   S -> D3 (offset 2)
//   D -> E3 (offset 4)
//   F -> F3 (offset 5)
//   G -> G3 (offset 7)
//   H -> A3 (offset 9)
//   J -> B3 (offset 11)
//
// Black keys (accidentals):
//   W -> C♯3 (offset 1)
//   E -> D♯3 (offset 3)
//   T -> F♯3 (offset 6)
//   Y -> G♯3 (offset 8)
//   U -> A♯3 (offset 10)
//
typedef struct {
    int key;             // Raylib key code.
    int semitone_offset; // Semitone offset from base note C3.
} NoteMapping;

#define NUM_WHITE_KEYS 7
#define NUM_BLACK_KEYS 5
#define NUM_NOTE_KEYS (NUM_WHITE_KEYS + NUM_BLACK_KEYS)

const NoteMapping white_keys[NUM_WHITE_KEYS] = {
    { KEY_A, 0 },   // C3
    { KEY_S, 2 },   // D3
    { KEY_D, 4 },   // E3
    { KEY_F, 5 },   // F3
    { KEY_G, 7 },   // G3
    { KEY_H, 9 },   // A3
    { KEY_J, 11 }   // B3
};

const NoteMapping black_keys[NUM_BLACK_KEYS] = {
    { KEY_W, 1 },   // C♯3
    { KEY_E, 3 },   // D♯3
    { KEY_T, 6 },   // F♯3
    { KEY_Y, 8 },   // G♯3
    { KEY_U, 10 }   // A♯3
};

// Active voice mapping for each note key. A value of -1 indicates no active voice.
int active_voice[NUM_NOTE_KEYS];

int main(void) {
    // Initialize synth state.
    State *state = State_create();
    for (int i = 0; i < NUM_NOTE_KEYS; i++) {
        active_voice[i] = -1;
    }
    // Base frequency for C3.
    const double base_freq = 130.81;
    const double semitone_ratio = pow(2.0, 1.0 / 12.0);

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
        // Process white keys.
        for (int i = 0; i < NUM_WHITE_KEYS; i++) {
            if (IsKeyDown(white_keys[i].key)) {
                if (active_voice[i] == -1) {
                    double freq = base_freq * pow(semitone_ratio, white_keys[i].semitone_offset);
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
        // Process black keys.
        for (int i = 0; i < NUM_BLACK_KEYS; i++) {
            int voice_index = i + NUM_WHITE_KEYS;
            if (IsKeyDown(black_keys[i].key)) {
                if (active_voice[voice_index] == -1) {
                    double freq = base_freq * pow(semitone_ratio, black_keys[i].semitone_offset);
                    active_voice[voice_index] = voice_index;
                    State_set_note(state, active_voice[voice_index], freq);
                }
            } else {
                if (active_voice[voice_index] != -1) {
                    State_clear_voice(state, active_voice[voice_index]);
                    active_voice[voice_index] = -1;
                }
            }
        }
        // Process wavetable level adjustments.
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
        DrawText("White keys (C3 to B3): A, S, D, F, G, H, J", 10, 10, 20, DARKGRAY);
        DrawText("Black keys (C#3, D#3, F#3, G#3, A#3): W, E, T, Y, U", 10, 40, 20, DARKGRAY);
        DrawText("Adjust Levels: 1/2 for wt[0], 3/4 for wt[1], 5/6 for wt[2], 7/8 for wt[3]", 10, 70, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
    State_destroy(state);
    return 0;
}
