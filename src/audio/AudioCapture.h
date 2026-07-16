#ifndef VOICECONTROL_AUDIO_CAPTURE_H
#define VOICECONTROL_AUDIO_CAPTURE_H

#include <thread>
#include <atomic>
#include <vector>
#include "common/ThreadSafeQueue.h"

using namespace std;

// Records the default microphone on a background thread and pushes
// mono 16 kHz frames into the given queue.
class AudioCapture {
public:
    AudioCapture(ThreadSafeQueue<vector<float>>& output);
    ~AudioCapture();

    void start();
    void stop();

private:
    void run();

    ThreadSafeQueue<vector<float>>& output;
    thread worker;
    atomic<bool> running;
};

#endif
