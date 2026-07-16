# VoiceControl

A small voice assistant for Windows written in C++. It listens to the
microphone, turns short spoken commands into text, and does simple things on
the PC (open an app, change volume, media keys). Everything runs locally.

The speech recognition uses whisper.cpp. The part that decides what a command
means is my own code, not an AI service.

## How it works

The audio goes through a few steps:

1. Audio capture (WASAPI) on a background thread
2. Voice activity detection (my own energy-based check)
3. Speech to text (whisper.cpp, `base.en` model, CUDA)
4. Command parser (keyword matching, my own code)
5. Actions (Windows API: ShellExecute, media/volume keys)
6. Voice reply (Windows SAPI)

## Progress

- [x] Stage 1 - audio capture, shows a live volume bar
- [x] Stage 2 - voice activity detection (finds where speech starts and ends)
- [x] Stage 3 - speech to text with whisper.cpp (CPU for now, CUDA next)
- [ ] Stage 4 - command parser
- [ ] Stage 5 - actions
- [ ] Stage 6 - voice reply

## Build

Needs CMake and a C++17 compiler (Visual Studio 2022 with the C++ workload).

whisper.cpp is included as a git submodule, so clone with `--recurse-submodules`,
or run `git submodule update --init` after a normal clone. You also need the
`base.en` model from https://huggingface.co/ggerganov/whisper.cpp in `models/`.

```
cmake -S . -B build
cmake --build build --config Release
build\Release\VoiceControl.exe
```

Speak a short command and it gets printed as text.
