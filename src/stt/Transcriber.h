#ifndef VOICECONTROL_TRANSCRIBER_H
#define VOICECONTROL_TRANSCRIBER_H

#include <string>
#include <vector>

using namespace std;

struct whisper_context; // avoids including whisper.h here

// Turns recorded audio into text with whisper.cpp.
class Transcriber {
public:
    Transcriber(const string& modelPath);
    ~Transcriber();

    string transcribe(const vector<float>& audio);

    // words to expect, nudges whisper towards our commands
    void setPrompt(const string& text);

private:
    whisper_context* ctx;
    string prompt;
};

#endif
