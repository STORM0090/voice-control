#include "audio/Resampler.h"

using namespace std;

Resampler::Resampler(int inputRate, int outputRate) {
    step = (double)inputRate / (double)outputRate;
    pos = 0.0;
}

vector<float> Resampler::process(const vector<float>& input) {
    for (const auto& s : input) {
        buffer.push_back(s);
    }

    vector<float> output;

    // step through the buffer, interpolating between neighbouring samples
    while (pos + 1.0 < (double)buffer.size()) {
        int i = (int)pos;
        double frac = pos - i;
        float value = buffer[i] * (1.0f - (float)frac) + buffer[i + 1] * (float)frac;
        output.push_back(value);
        pos += step;
    }

    int consumed = (int)pos;
    if (consumed > 0) {
        buffer.erase(buffer.begin(), buffer.begin() + consumed);
        pos -= consumed;
    }

    return output;
}
