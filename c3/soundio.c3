module soundio;

def Callback = fn void(SoundIo *);
def CallbackErr = fn void(SoundIo *, int err);

enum SoundIoBackend : int {
    SOUNDIO_BACKEND_NONE,
    SOUNDIO_BACKEND_JACK,
    SOUNDIO_BACKEND_PULSEAUDIO,
    SOUNDIO_BACKEND_ALSA,
    SOUNDIO_BACKEND_COREAUDIO,
    SOUNDIO_BACKEND_WASAPI,
    SOUNDIO_BACKEND_DUMMY,
}

/// The size of this struct is not part of the API or ABI.
struct SoundIo {
    /// Optional. Put whatever you want here. Defaults to NULL.
    void *userdata;
    /// Optional callback. Called when the list of devices change. Only called
    /// during a call to ::soundio_flush_events or ::soundio_wait_events.
    Callback on_devices_change;
    /// Optional callback. Called when the backend disconnects. For example,
    /// when the JACK server shuts down. When this happens, listing devices
    /// and opening streams will always fail with
    /// SoundIoErrorBackendDisconnected. This callback is only called during a
    /// call to ::soundio_flush_events or ::soundio_wait_events.
    /// If you do not supply a callback, the default will crash your program
    /// with an error message. This callback is also called when the thread
    /// that retrieves device information runs into an unrecoverable condition
    /// such as running out of memory.
    ///
    /// Possible errors:
    /// * #SoundIoErrorBackendDisconnected
    /// * #SoundIoErrorNoMem
    /// * #SoundIoErrorSystemResources
    /// * #SoundIoErrorOpeningDevice - unexpected problem accessing device
    ///   information
    CallbackErr on_backend_disconnect;
    /// Optional callback. Called from an unknown thread that you should not use
    /// to call any soundio functions. You may use this to signal a condition
    /// variable to wake up. Called when ::soundio_wait_events would be woken up.
    Callback n_events_signal;

    /// Read-only. After calling ::soundio_connect or ::soundio_connect_backend,
    /// this field tells which backend is currently connected.
    SoundIoBackend current_backend;

    /// Optional: Application name.
    /// PulseAudio uses this for "application name".
    /// JACK uses this for `client_name`.
    /// Must not contain a colon (":").
    char *app_name;

    /// Optional: Real time priority warning.
    /// This callback is fired when making thread real-time priority failed. By
    /// default, it will print to stderr only the first time it is called
    /// a message instructing the user how to configure their system to allow
    /// real-time priority threads. This must be set to a function not NULL.
    /// To silence the warning, assign this to a function that does nothing.
    void (*emit_rtprio_warning)(void);

    /// Optional: JACK info callback.
    /// By default, libsoundio sets this to an empty function in order to
    /// silence stdio messages from JACK. You may override the behavior by
    /// setting this to `NULL` or providing your own function. This is
    /// registered with JACK regardless of whether ::soundio_connect_backend
    /// succeeds.
    void (*jack_info_callback)(const char *msg);
    /// Optional: JACK error callback.
    /// See SoundIo::jack_info_callback
    void (*jack_error_callback)(const char *msg);
}
