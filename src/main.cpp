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

using namespace std;

// The exe can be started from the project root or from build\Release,
// so look for the model in a few places.
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
    try {
        string modelPath = (argc > 1) ? argv[1] : findModel();
        Transcriber transcriber(modelPath);

        ThreadSafeQueue<vector<float>> frames;
        AudioCapture capture(frames);
        capture.start();

        Vad vad;
        bool wasSpeaking = false;

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
