#include <math.h>
#include <pthread.h>
#include <raylib.h>
#include <soundio/soundio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "state.h"

// Global mutex to protect State during audio callback and UI updates.
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

// The audio write callback.
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
            // Write the sample to each channel.
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

// Note mapping structure.
typedef struct {
    int key;         // Raylib key code.
    int voice;       // voice index (0 .. NUM_VOICES-1)
    double freq;     // Frequency in Hz.
} NoteMapping;

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Create the global state.
    State *state = State_create();

    // Define a simple mapping for a few keys.
    NoteMapping mappings[] = {
        { KEY_A, 0, 261.63 },  // C3
        { KEY_W, 1, 277.18 },
        { KEY_S, 2, 293.66 },
        { KEY_E, 3, 311.13 },
        { KEY_D, 4, 329.63 },
        // Add more mappings as desired.
    };
    const int num_mappings = sizeof(mappings)/sizeof(mappings[0]);

    // Initialize libsoundio.
    struct SoundIo *soundio = soundio_create();
    if (!soundio) {
        fprintf(stderr, "Out of memory.\n");
        return 1;
    }
    int err = soundio_connect(soundio);
    if (err) {
        fprintf(stderr, "Error connecting: %s\n", soundio_strerror(err));
        return 1;
    }
    soundio_flush_events(soundio);
    int default_out_device_index = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0) {
        fprintf(stderr, "No output device found.\n");
        return 1;
    }
    struct SoundIoDevice *device = soundio_get_output_device(soundio, default_out_device_index);
    if (!device) {
        fprintf(stderr, "Unable to get output device.\n");
        return 1;
    }
    printf("Using output device: %s\n", device->name);

    // Create output stream.
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
    if (err) {
        fprintf(stderr, "Error opening stream: %s\n", soundio_strerror(err));
        return 1;
    }
    if (outstream->layout_error)
        fprintf(stderr, "Channel layout error: %s\n", soundio_strerror(outstream->layout_error));
    err = soundio_outstream_start(outstream);
    if (err) {
        fprintf(stderr, "Error starting stream: %s\n", soundio_strerror(err));
        return 1;
    }

    // Initialize Raylib window.
    InitWindow(640, 480, "Polyphonic Synthesizer");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // For each mapping, if the key is down, set the note; else, clear the voice.
        pthread_mutex_lock(&state_mutex);
        for (int i = 0; i < num_mappings; i++) {
            if (IsKeyDown(mappings[i].key)) {
                State_set_note(state, mappings[i].voice, mappings[i].freq);
            } else {
                State_clear_voice(state, mappings[i].voice);
            }
        }
        pthread_mutex_unlock(&state_mutex);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Press keys A, W, S, E, D to play notes", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
    State_destroy(state);
    return 0;
}
