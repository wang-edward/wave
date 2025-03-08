import std::io;
import soundio;

extern fn SoundIo *soundio_create();

fn int main() {
    std::io::printf("yo\n");
    soundio::SoundIo *soundio = soundio_create();
    return 0;
}
