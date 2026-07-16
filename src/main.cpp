#include <iostream>
#include <string>
#include <vector>

#include "common/AudioTypes.h"
#include "common/ThreadSafeQueue.h"
#include "audio/AudioCapture.h"
#include "vad/Vad.h"
#include "stt/Transcriber.h"

using namespace std;

int main(int argc, char** argv) {
    string modelPath = "models/ggml-base.en.bin";
    if (argc > 1) {
        modelPath = argv[1];
    }

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
            cout << "\"" << transcriber.transcribe(segment) << "\"\n";
        }
    }

    return 0;
}
