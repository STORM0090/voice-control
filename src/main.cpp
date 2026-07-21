#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>

#include "common/AudioTypes.h"
#include "common/ThreadSafeQueue.h"
#include "audio/AudioCapture.h"
#include "vad/Vad.h"
#include "stt/Transcriber.h"
#include "command/CommandParser.h"
#include "action/ActionExecutor.h"
#include "tts/Speaker.h"

using namespace std;

// Run with: VoiceControl.exe --test
static int runParserTests() {
    struct Case {
        string text;
        int expected;
        string expectedArg; // checked only when set
    };
    vector<Case> cases = {
        {" Open Chrome.", CMD_OPEN_APP, "chrome.exe"},
        {"launch chrome", CMD_OPEN_APP, "chrome.exe"},
        {" Open Home.", CMD_OPEN_APP, "chrome.exe"},
        {" Open Spotify.", CMD_OPEN_APP, "spotify:"},
        {" Open music.", CMD_OPEN_APP, "spotify:"},
        {"open calculator", CMD_OPEN_APP, "calc.exe"},
        {" and calculator.", CMD_OPEN_APP, "calc.exe"}, // whisper mangled "open"
        {" Open Notepad.", CMD_OPEN_APP, "notepad.exe"},
        {"LOUDER!", CMD_VOLUME_UP},
        {" Volume up.", CMD_VOLUME_UP},
        {"plus", CMD_VOLUME_UP},
        {"quieter", CMD_VOLUME_DOWN},
        {"minus", CMD_VOLUME_DOWN},
        {" Mute.", CMD_MUTE_TOGGLE},
        {"unmute", CMD_MUTE_TOGGLE},
        {" Next track.", CMD_NEXT_TRACK},
        {"skip", CMD_NEXT_TRACK},
        {" Previous song.", CMD_PREV_TRACK},
        {" Play.", CMD_PLAY_PAUSE},
        {"pause", CMD_PLAY_PAUSE},
        {" Show desktop.", CMD_SHOW_DESKTOP},
        {"minimize", CMD_SHOW_DESKTOP},
        {" Lock screen.", CMD_LOCK_SCREEN},
        {"lock", CMD_NONE},               // one word must not lock the PC
        {" soundtrack ", CMD_NONE},       // "track" inside a word must not match
        {" Change music.", CMD_NONE},
        {"", CMD_NONE},
    };

    CommandParser parser;
    int failed = 0;
    for (const auto& c : cases) {
        Command cmd = parser.parse(c.text);
        if (cmd.type != c.expected) {
            cout << "FAIL: \"" << c.text << "\" -> got type " << cmd.type
                 << ", expected " << c.expected << "\n";
            failed++;
        } else if (!c.expectedArg.empty() && cmd.arg != c.expectedArg) {
            cout << "FAIL: \"" << c.text << "\" -> got arg \"" << cmd.arg
                 << "\", expected \"" << c.expectedArg << "\"\n";
            failed++;
        }
    }
    cout << (cases.size() - failed) << "/" << cases.size() << " parser tests passed\n";
    return failed == 0 ? 0 : 1;
}

// the exe may be started from the project root or from build\Release
static string findModel() {
    vector<string> candidates = {
        "models/ggml-base.en.bin",
        "../models/ggml-base.en.bin",
        "../../models/ggml-base.en.bin",
    };
    for (const auto& path : candidates) {
        ifstream f(path);
        if (f.good()) {
            return path;
        }
    }
    throw runtime_error("ggml-base.en.bin not found in models/");
}

int main(int argc, char** argv) {
    if (argc > 1 && string(argv[1]) == "--test") {
        return runParserTests();
    }

    try {
        string modelPath = (argc > 1) ? argv[1] : findModel();
        Transcriber transcriber(modelPath);

        ThreadSafeQueue<vector<float>> frames;
        AudioCapture capture(frames);
        capture.start();

        Vad vad;
        CommandParser parser;
        ActionExecutor executor;
        Speaker speaker;
        bool wasSpeaking = false;

        transcriber.setPrompt(parser.vocabulary());

        cout << "Listening... speak into the microphone (Ctrl+C to quit).\n";

        while (true) {
            vector<float> frame = frames.waitAndPop();
            vector<float> segment = vad.process(frame);

            if (vad.isSpeaking() && !wasSpeaking) {
                cout << "Speech started...\n";
            }
            wasSpeaking = vad.isSpeaking();

            if (!segment.empty()) {
                float seconds = (float)segment.size() / kSampleRate;
                cout << "Speech ended: " << seconds << " s -> ";
                cout.flush();

                auto t0 = chrono::steady_clock::now();
                string text = transcriber.transcribe(segment);
                auto t1 = chrono::steady_clock::now();
                long ms = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count();

                cout << "\"" << text << "\" (" << ms << " ms)\n";

                Command cmd = parser.parse(text);
                if (cmd.type == CMD_NONE) {
                    cout << "  -> not a known command\n";
                } else if (executor.execute(cmd)) {
                    cout << "  -> " << cmd.confirmation << "\n";
                    speaker.say(cmd.confirmation);
                    // a confirmation like "Volume up" is itself a command,
                    // so drop what the mic picked up while we were talking
                    frames.clear();
                } else {
                    cout << "  -> recognized, but not executed\n";
                }
            }
        }
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        cerr << "Press Enter to exit.\n";
        cin.get();
        return 1;
    }

    return 0;
}
