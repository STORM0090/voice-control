#ifndef VOICECONTROL_SPEAKER_H
#define VOICECONTROL_SPEAKER_H

#include <string>

using namespace std;

// Declared here so this header does not have to include sapi.h.
struct ISpVoice;

// Says short confirmations out loud with the voice built into Windows.
class Speaker {
public:
    Speaker();
    ~Speaker();

    void say(const string& text);

private:
    ISpVoice* voice;
};

#endif
