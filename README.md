# VoiceControl

A voice assistant for Windows written in C++. It listens to the microphone,
recognizes short spoken commands and controls the PC: opens apps, changes the
volume, skips tracks.

Everything runs on the machine. There are no cloud calls and no AI service
decides what a command means - that part is my own code.

## Commands

| Say | What happens |
| --- | --- |
| google, browser, open chrome | starts Chrome |
| open spotify, open music | starts Spotify |
| notepad, notes | starts Notepad |
| calculator, calc | starts Calculator |
| plus, volume up, louder | volume up |
| minus, volume down, quieter | volume down |
| mute, unmute | mute on/off |
| next track, skip | next track |
| previous track, go back | previous track |
| play, pause | play/pause |
| show desktop, minimize | minimizes all windows |
| lock screen | locks the PC |

## How it works

```
microphone -> WASAPI capture -> VAD -> whisper.cpp -> parser -> Windows API
                 (thread)              (GPU)                     + voice reply
```

1. **Audio capture** (`src/audio`) - WASAPI records the default microphone on a
   background thread, mixes it to mono and resamples it to the 16 kHz that
   whisper expects. Frames are handed to the main thread through a small
   thread-safe queue.
2. **Voice activity detection** (`src/vad`) - my own algorithm. It measures the
   energy of every 20 ms frame and runs a two state machine: three loud frames in
   a row start a phrase, twenty quiet ones end it. Using different thresholds for
   starting and stopping keeps it from flickering on short pauses between words.
   The last 100 ms before a phrase are kept and put back in front of it, because
   quiet consonants at the start would otherwise be cut off.
3. **Speech to text** (`src/stt`) - whisper.cpp with the `base.en` model. On a
   GTX 1660 SUPER a command takes about 350 ms on the GPU against 1100 ms on the
   CPU.
4. **Command parser** (`src/command`) - my own code, no AI. Rules live in a
   table, so a new command is one line. `VoiceControl.exe --test` runs the
   parser against recorded whisper output.
5. **Actions** (`src/action`) - `ShellExecute` for apps, `SendInput` with the
   media and volume keys for everything else.
6. **Voice reply** (`src/tts`) - Windows SAPI says a short confirmation.

## Things that turned out to be tricky

- **Whisper hears context, not single words.** Isolated words came back wrong
  ("chrome" as "Rome", "spotify" as "fortify"), while "open chrome" was correct
  almost every time - whisper is a language model and uses the surrounding
  words. Feeding it the list of known commands through `initial_prompt` fixed
  most of the single word cases.
- **The command words matter more than the code.** "louder" and "quieter" were
  misheard constantly, so the parser also accepts "plus" and "minus", which are
  short and come back clean. One syllable words are the worst case: "chrome" on
  its own kept arriving as "from" or "card", so "google" is a trigger too.
- **The assistant heard itself.** A spoken confirmation like "Volume up" is a
  valid command, so it triggered itself in a loop. The queue is cleared after
  every reply.
- **`std::byte` clashes with the Windows `byte` type**, so the Windows headers
  have to be included before `using namespace std;`.

## Build

Needs CMake and a C++17 compiler (Visual Studio with the C++ workload).

whisper.cpp is a git submodule, so clone with `--recurse-submodules`, or run
`git submodule update --init` afterwards. The `base.en` model goes into
`models/` and can be downloaded from
https://huggingface.co/ggerganov/whisper.cpp

```
cmake -S . -B build
cmake --build build --config Release
build\Release\VoiceControl.exe
```

For GPU speech recognition, install the CUDA Toolkit and add `-DGGML_CUDA=ON`
to the first command.
