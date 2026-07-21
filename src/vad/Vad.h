#ifndef VOICECONTROL_VAD_H
#define VOICECONTROL_VAD_H

#include <vector>

using namespace std;

// Feed it one frame at a time. Returns the whole phrase once it ends,
// an empty vector otherwise.
class Vad {
public:
    Vad();

    vector<float> process(const vector<float>& frame);
    bool isSpeaking() const;

private:
    int state;
    int speechRun;
    int silenceRun;
    vector<float> segment;
};

#endif
