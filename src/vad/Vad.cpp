#include "vad/Vad.h"
#include "common/AudioTypes.h"

#include <cmath>

using namespace std;

static const float kEnergyThreshold = 0.02f;
static const int kStartFrames = 3;   // ~60 ms of sound to start
static const int kEndFrames = 20;    // ~400 ms of silence to stop

// Consonants like "ch" or "s" are quiet and fall below the threshold, so the
// audio just before a phrase is kept and put back in front of it.
static const int kPreRollSamples = kSampleRate / 10; // 100 ms

enum { SILENCE, SPEECH };

static float rms(const vector<float>& frame) {
    if (frame.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& s : frame) {
        sum += s * s;
    }
    return sqrt(sum / frame.size());
}

Vad::Vad() {
    state = SILENCE;
    speechRun = 0;
    silenceRun = 0;
}

bool Vad::isSpeaking() const {
    return state == SPEECH;
}

vector<float> Vad::process(const vector<float>& frame) {
    float energy = rms(frame);
    bool loud = energy > kEnergyThreshold;

    if (state == SILENCE) {
        if (loud) {
            if (speechRun == 0) {
                segment = preRoll;
            }
            speechRun++;
            for (const auto& s : frame) {
                segment.push_back(s);
            }
        } else {
            speechRun = 0;
            segment.clear();
            for (const auto& s : frame) {
                preRoll.push_back(s);
            }
            if ((int)preRoll.size() > kPreRollSamples) {
                preRoll.erase(preRoll.begin(), preRoll.end() - kPreRollSamples);
            }
        }

        if (speechRun >= kStartFrames) {
            state = SPEECH;
            silenceRun = 0;
        }
        return {};
    }

    for (const auto& s : frame) {
        segment.push_back(s);
    }
    if (loud) {
        silenceRun = 0;
    } else {
        silenceRun++;
    }

    if (silenceRun >= kEndFrames) {
        state = SILENCE;
        speechRun = 0;
        vector<float> done = move(segment);
        segment.clear();
        return done;
    }
    return {};
}
