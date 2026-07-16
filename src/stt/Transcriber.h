#ifndef VOICECONTROL_TRANSCRIBER_H
#define VOICECONTROL_TRANSCRIBER_H

#include <string>
#include <vector>

using namespace std;

// Declared here so this header does not have to include whisper.h.
struct whisper_context;

// Turns recorded audio into text using whisper.cpp.
class Transcriber {
public:
    Transcriber(const string& modelPath);
    ~Transcriber();

    string transcribe(const vector<float>& audio);

private:
    whisper_context* ctx;
};

#endif
