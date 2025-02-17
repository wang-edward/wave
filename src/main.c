#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <soundio/soundio.h>
#include <raylib.h>
#include <pthread.h>  // For mutex

#include "config.h"
#include "osc.h"
#include "vec.h"

// Global mutex to protect oscillator state.
static pthread_mutex_t osc_mutex = PTHREAD_MUTEX_INITIALIZER;

// The write callback: libsoundio calls this when it needs more audio samples.
// We now mix the outputs from all oscillators, each using its own wavetable.
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
    OscVec *osc_vec = (OscVec *)outstream->userdata;
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

            pthread_mutex_lock(&osc_mutex);
            // Mix output from each oscillator.
            for (size_t i = 0; i < osc_vec->size; i++) {
                struct Osc *osc = &osc_vec->data[i];
                double pos = osc->phase;
                int index0 = (int)pos;
                int index1 = (index0 + 1) % TABLE_SIZE;
                double frac = pos - index0;
                // Use the oscillator's own wavetable.
                float osc_sample = (float)((1.0 - frac) * osc->wavetable[index0] + frac * osc->wavetable[index1]);
                sample += osc_sample;

                // Increment phase and wrap around if necessary.
                osc->phase += osc->phase_inc;
                if (osc->phase >= TABLE_SIZE)
                    osc->phase -= TABLE_SIZE;
            }
            pthread_mutex_unlock(&osc_mutex);

            // Average the mixed sample.
            if (osc_vec->size > 0)
                sample /= osc_vec->size;

            // Write the sample to all output channels.
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

int main(int argc, char **argv) {
    // Create an oscillator vector.
    OscVec *osc_vec = oscvec_create(4);
    double freq0 = 440.0, freq1 = 550.0, freq2 = 660.0;
    struct Osc temp;

    // Create oscillator 0.
    temp.phase = 0.0;
    osc_init_wavetable(&temp);
    osc_set_freq(&temp, freq0);
    oscvec_push(osc_vec, temp);

    // Create oscillator 1.
    temp.phase = 0.0;
    osc_init_wavetable(&temp);
    osc_set_freq(&temp, freq1);
    oscvec_push(osc_vec, temp);

    // Create oscillator 2.
    temp.phase = 0.0;
    osc_init_wavetable(&temp);
    osc_set_freq(&temp, freq2);
    oscvec_push(osc_vec, temp);

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

    // Select the default output device.
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

    // Create an output stream.
    struct SoundIoOutStream *outstream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;
    outstream->sample_rate = SAMPLE_RATE;

    // Choose a channel layout.
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

    // Set our oscillator vector as userdata and assign the callback.
    outstream->userdata = osc_vec;
    outstream->write_callback = write_callback;

    // Open and start the output stream.
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
    InitWindow(480, 480, "Multiple Oscillators with Individual Wavetables");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Oscillator Frequency Controls:", 10, 10, 20, DARKGRAY);
        DrawText("Osc 0: UP/DOWN", 10, 40, 20, DARKGRAY);
        DrawText("Osc 1: RIGHT/LEFT", 10, 70, 20, DARKGRAY);
        DrawText("Osc 2: W/S", 10, 100, 20, DARKGRAY);

        // Adjust oscillator 0 with UP/DOWN keys.
        if (IsKeyPressed(KEY_UP)) {
            pthread_mutex_lock(&osc_mutex);
            freq0 += 10.0;
            osc_set_freq(&osc_vec->data[0], freq0);
            pthread_mutex_unlock(&osc_mutex);
        }
        if (IsKeyPressed(KEY_DOWN)) {
            pthread_mutex_lock(&osc_mutex);
            freq0 -= 10.0;
            if (freq0 < 1.0)
                freq0 = 1.0;
            osc_set_freq(&osc_vec->data[0], freq0);
            pthread_mutex_unlock(&osc_mutex);
        }

        // Adjust oscillator 1 with RIGHT/LEFT keys.
        if (IsKeyPressed(KEY_RIGHT)) {
            pthread_mutex_lock(&osc_mutex);
            freq1 += 10.0;
            osc_set_freq(&osc_vec->data[1], freq1);
            pthread_mutex_unlock(&osc_mutex);
        }
        if (IsKeyPressed(KEY_LEFT)) {
            pthread_mutex_lock(&osc_mutex);
            freq1 -= 10.0;
            if (freq1 < 1.0)
                freq1 = 1.0;
            osc_set_freq(&osc_vec->data[1], freq1);
            pthread_mutex_unlock(&osc_mutex);
        }

        // Adjust oscillator 2 with W/S keys.
        if (IsKeyPressed(KEY_W)) {
            pthread_mutex_lock(&osc_mutex);
            freq2 += 10.0;
            osc_set_freq(&osc_vec->data[2], freq2);
            pthread_mutex_unlock(&osc_mutex);
        }
        if (IsKeyPressed(KEY_S)) {
            pthread_mutex_lock(&osc_mutex);
            freq2 -= 10.0;
            if (freq2 < 1.0)
                freq2 = 1.0;
            osc_set_freq(&osc_vec->data[2], freq2);
            pthread_mutex_unlock(&osc_mutex);
        }

        EndDrawing();
    }

    // Clean up.
    CloseWindow();
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);
    oscvec_destroy(osc_vec);

    return 0;
}
