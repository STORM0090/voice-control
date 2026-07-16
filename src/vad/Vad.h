#ifndef VOICECONTROL_VAD_H
#define VOICECONTROL_VAD_H

#include <vector>

using namespace std;

// Voice activity detection.
// Feed it one frame at a time. It returns an empty vector while nothing useful
// happened, and returns the whole phrase once a spoken segment has finished.
class Vad {
public:
    Vad();

    vector<float> process(const vector<float>& frame);
    bool isSpeaking() const;

private:
    int state;
    int speechRun;   // loud frames in a row
    int silenceRun;  // quiet frames in a row
    vector<float> segment;
};

#endif
