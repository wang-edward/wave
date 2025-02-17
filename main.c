#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <soundio/soundio.h>
#include <raylib.h>

#define TABLE_SIZE 1024
#define SAMPLE_RATE 48000

// The wavetable will hold one cycle of a sine wave.
static float wavetable[TABLE_SIZE];

// Precompute the wavetable.
void init_wavetable(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        wavetable[i] = (float)sin(2.0 * M_PI * i / TABLE_SIZE);
    }
}

// Structure to hold our oscillator state.
struct Osc {
    double phase;      // current phase (in table index units)
    double phase_inc;  // phase increment per sample
};

void osc_set_freq(struct Osc *osc, double freq) {
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
}



// The write callback: libsoundio calls this when it needs more audio samples.
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
    struct Osc *osc = (struct Osc *)outstream->userdata;
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

        // For each frame, compute the sample from the wavetable oscillator.
        for (int frame = 0; frame < frame_count; frame++) {
            // Compute the table indices for interpolation.
            double pos = osc->phase;
            int index0 = (int)pos;
            int index1 = (index0 + 1) % TABLE_SIZE;
            double frac = pos - index0;
            // Linear interpolation between wavetable samples.
            float sample = (float)((1.0 - frac) * wavetable[index0] + frac * wavetable[index1]);

            // Write the same sample to all output channels.
            for (int ch = 0; ch < outstream->layout.channel_count; ch++) {
                char *ptr = areas[ch].ptr + areas[ch].step * frame;
                *((float *)ptr) = sample; // using SOUNDIO_FORMAT_FLOAT32NE
            }

            // Increment phase and wrap around if necessary.
            osc->phase += osc->phase_inc;
            if (osc->phase >= TABLE_SIZE)
                osc->phase -= TABLE_SIZE;
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
    // Initialize the wavetable with a sine wave.
    init_wavetable();

    // Set up our oscillator to produce a 440 Hz tone.
    struct Osc osc;
    osc.phase = 0.0;
    double frequency = 440.0;
    osc.phase_inc = (TABLE_SIZE * frequency) / SAMPLE_RATE;

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
    // Use the native-endian float format.
    outstream->format = SoundIoFormatFloat32NE;
    outstream->sample_rate = SAMPLE_RATE;

    // Attempt to choose a stereo channel layout if available.
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
        // If there are no layouts provided, you can set a default layout.
        // (SOUNDIO_CHANNEL_LAYOUT_STEREO might be undefined; if so, set the channel_count manually.)
        outstream->layout = *soundio_channel_layout_get_default(2);
    }

    // Set our oscillator state as userdata and assign the callback.
    outstream->userdata = &osc;
    outstream->write_callback = write_callback;

    // Open the output stream.
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

    InitWindow(480, 480, "wave");

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        if (IsKeyDown(KEY_UP)) {
            frequency *= 2;
            osc_set_freq(&osc, frequency);
        } else if (IsKeyDown(KEY_DOWN)) {
            frequency /= 2;
            osc_set_freq(&osc, frequency);
        }

        EndDrawing();
    }

    // Clean up.
    CloseWindow();
    soundio_outstream_destroy(outstream);
    soundio_device_unref(device);
    soundio_destroy(soundio);

    return 0;
}
