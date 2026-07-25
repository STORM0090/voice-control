# VoiceControl

A voice assistant for Windows written in C++. It listens to the microphone,
recognizes short spoken commands and controls the PC: opens apps, changes the
volume, skips tracks.

Everything runs on the machine. There are no cloud calls, and no AI service
decides what a command means - that part is my own code.

## Commands

- google, browser, open chrome - starts Chrome
- open spotify, open music - starts Spotify
- notepad, notes - starts Notepad
- calculator, calc - starts Calculator
- plus, volume up, louder - volume up by 10 percent
- minus, volume down, quieter - volume down by 10 percent
- max volume, full volume - volume to 100 percent
- half volume - volume to 50 percent
- mute, unmute - mute on and off
- next track, skip - next track
- previous track, go back - previous track
- play, pause - play and pause
- show desktop, minimize - minimizes all windows
- lock screen - locks the PC

## How it works

The audio goes through six steps.

1. Audio capture, in src/audio. WASAPI records the default microphone on a
background thread, mixes it to mono and resamples it to the 16 kHz that whisper
expects. Frames go to the main thread through a small thread-safe queue.

2. Voice activity detection, in src/vad. This is my own algorithm. It measures
the energy of every 20 ms frame and runs a two state machine: three loud frames
in a row start a phrase, twenty quiet ones end it. Using different limits for
starting and stopping keeps it from flickering on short pauses between words.
The last 100 ms before a phrase are kept and put back in front of it, because
quiet consonants at the start would otherwise be cut off.

3. Speech to text, in src/stt. This part uses whisper.cpp with the base.en
model. On a GTX 1660 SUPER a command takes about 350 ms on the GPU against
1100 ms on the CPU.

4. Command parser, in src/command. My own code, no AI. The rules live in a
table, so adding a command is one line. Running VoiceControl.exe --test checks
the parser against real whisper output.

5. Actions, in src/action. ShellExecute starts apps and SendInput presses the
media keys. The volume goes through the Core Audio API instead, because one
press of a volume key moves it by about two percent, which is too coarse to set
a level like fifty percent.

6. Voice reply, in src/tts. Windows SAPI says a short confirmation.

## Things that turned out to be tricky

Whisper uses context, not only sound. Single words came back wrong, chrome as
"Rome" and spotify as "fortify", while "open chrome" was right almost every
time. Passing the list of known commands through initial_prompt fixed most of
the single word cases.

The command words matter more than the code. Louder and quieter were misheard
constantly, so the parser also accepts plus and minus. One syllable words are
the worst case: chrome on its own kept arriving as "from" or "card", so google
is a trigger too.

The assistant heard itself. A spoken confirmation like "Volume up" is a valid
command, so it kept triggering itself in a loop. The audio queue is cleared
after every reply.

The type std::byte clashes with the Windows byte type, so the Windows headers
have to be included before using namespace std.

## Build

Needs CMake and a C++17 compiler, for example Visual Studio with the C++
workload.

whisper.cpp is a git submodule, so clone with --recurse-submodules, or run
git submodule update --init afterwards. The base.en model goes into the models
folder and can be downloaded from https://huggingface.co/ggerganov/whisper.cpp

    cmake -S . -B build
    cmake --build build --config Release
    build\Release\VoiceControl.exe

For speech recognition on the GPU, install the CUDA Toolkit and add
-DGGML_CUDA=ON to the first command.
