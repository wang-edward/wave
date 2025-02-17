module wave;

import stdio;
import stdlib;
import unistd;
import raylib;
import soundio;
import pthread;
import math;

// Instead of #define, use def:
def TABLE_SIZE = 1024;
def SAMPLE_RATE = 48000;
def M_PI = 3.141592653589793;

// Global wavetable array (arrays in C3 have the size next to the type)
static float[TABLE_SIZE] wavetable;

// Precompute the wavetable.
fn void init_wavetable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        // sin returns a double so cast it to float
        wavetable[i] = sin(2.0 * M_PI * i / TABLE_SIZE) as float;
    }
}

// In C3, user‐defined structs are declared without a trailing semicolon.
struct Osc {
    phase: double,
    phase_inc: double
}

// Set the oscillator frequency.
fn void osc_set_freq(osc: *Osc, freq: double) {
    osc->phase_inc = (TABLE_SIZE * freq) / SAMPLE_RATE;
}

// Global mutex from pthread.
pthread_mutex_t osc_mutex = PTHREAD_MUTEX_INITIALIZER;

// The write callback, called by SoundIo when more samples are needed.
fn void write_callback(outstream: *SoundIoOutStream, frame_count_min: int, frame_count_max: int) {
    // Convert userdata to our oscillator pointer.
    osc: *Osc = outstream->userdata as *Osc;
    frames_left: int = frame_count_max;

    while (frames_left > 0) {
        frame_count: int = frames_left;
        areas: *SoundIoChannelArea;
        err: int = soundio_outstream_begin_write(outstream, &areas, &frame_count);
        if (err != 0) {
            fprintf(stderr, "begin write error: %s\n", soundio_strerror(err));
            exit(1);
        }
        if (frame_count == 0) {
            break;
        }

        for (frame: int = 0; frame < frame_count; frame++) {
            sample: float;

            pthread_mutex_lock(&osc_mutex);
            pos: double = osc->phase;
            index0: int = pos as int;
            index1: int = (index0 + 1) % TABLE_SIZE;
            frac: double = pos - index0;
            sample = ((1.0 - frac) * wavetable[index0] + frac * wavetable[index1]) as float;

            osc->phase += osc->phase_inc;
            if (osc->phase >= TABLE_SIZE) {
                osc->phase -= TABLE_SIZE;
            }
            pthread_mutex_unlock(&osc_mutex);

            // Write the same sample to all output channels.
            for (ch: int = 0; ch < outstream->layout.channel_count; ch++) {
                ptr: *char = areas[ch].ptr + areas[ch].step * frame;
                // Cast the pointer to a float pointer and write our sample.
                *((ptr as *float)) = sample;
            }
        }

        err = soundio_outstream_end_write(outstream);
        if (err != 0) {
            fprintf(stderr, "end write error: %s\n", soundio_strerror(err));
            exit(1);
        }
        frames_left -= frame_count;
    }
}

fn int main(argc: int, argv: **char) {
    // Initialize the wavetable with a sine wave.
    init_wavetable();

    // Set up our oscillator to produce a 440 Hz tone.
    osc: Osc;
    osc.phase = 0.0;
    frequency: double = 440.0;
    osc_set_freq(&osc, frequency);

    // Initialize libsoundio.
    soundio: *SoundIo = soundio_create();
    if (soundio == null) {
        fprintf(stderr, "Out of memory.\n");
        return 1;
    }

    err: int = soundio_connect(soundio);
    if (err != 0) {
        fprintf(stderr, "Error connecting: %s\n", soundio_strerror(err));
        return 1;
    }
    soundio_flush_events(soundio);

    // Select the default output device.
    default_out_device_index: int = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0) {
        fprintf(stderr, "No output device found.\n");
        return 1;
    }
    device: *SoundIoDevice = soundio_get_output_device(soundio, default_out_device_index);
    if (device == null) {
        fprintf(stderr, "Unable to get output device.\n");
        return 1;
    }
    printf("Using output device: %s\n", device->name);

    // Create an output stream.
    outstream: *SoundIoOutStream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;
    outstream->sample_rate = SAMPLE_RATE;

    // Choose a stereo layout if available.
    if (device->layout_count > 0) {
        found: int = 0;
        for (i: int = 0; i < device->layout_count; i++) {
            if (device->layouts[i].channel_count == 2) {
                outstream->layout = device->layouts[i];
                found = 1;
                break;
            }
        }
        if (!found) {
            outstream->layout = device->layouts[0];
        }
    } else {
        outstream->layout = *soundio_channel_layout_get_default(2);
    }

    // Set the oscillator as userdata and assign the callback.
    outstream->userdata = &osc;
    outstream->write_callback = write_callback;

    err = soundio_outstream_open(outstream);
    if (err != 0) {
        fprintf(stderr, "Error opening stream: %s\n", soundio_strerror(err));
        return 1;
    }
    if (outstream->layout_error != 0) {
        fprintf(stderr, "Channel layout error: %s\n", soundio_strerror(outstream->layout_error));
    }

    err = soundio_outstream_start(outstream);
    if (err != 0) {
        fprintf(stderr, "Error starting stream: %s\n", soundio_strerror(err));
        return 1;
    }

    // Initialize Raylib window.
    InitWindow(480, 480, "wave");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Press UP/DOWN to change frequency", 10, 10, 20, DARKGRAY);

        // Use IsKeyPressed() for debouncing.
        if (IsKeyPressed(KEY_UP)) {
            pthread_mutex_lock(&osc_mutex);
            frequency *= 2;
            osc_set_freq(&osc, frequency);
            pthread_mutex_unlock(&osc_mutex);
        }
        if (IsKeyPressed(KEY_DOWN)) {
            pthread_mutex_lock(&osc_mutex);
            frequency /= 2;
            osc_set_freq(&osc, frequency);
            pthread_mutex_unlock(&osc_mutex);
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
