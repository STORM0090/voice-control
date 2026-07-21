#ifndef VOICECONTROL_SPEAKER_H
#define VOICECONTROL_SPEAKER_H

#include <string>

using namespace std;

struct ISpVoice; // avoids including sapi.h here

// Speaks short confirmations with the built-in Windows voice.
class Speaker {
public:
    Speaker();
    ~Speaker();

    void say(const string& text);

private:
    ISpVoice* voice;
};

#endif
