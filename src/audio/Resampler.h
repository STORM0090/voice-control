#ifndef VOICECONTROL_RESAMPLER_H
#define VOICECONTROL_RESAMPLER_H

#include <vector>

using namespace std;

// Changes the sample rate, e.g. 48000 -> 16000. Keeps state between calls.
class Resampler {
public:
    Resampler(int inputRate, int outputRate);
    vector<float> process(const vector<float>& input);

private:
    double step;
    double pos;
    vector<float> buffer;
};

#endif
