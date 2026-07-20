#include "stt/Transcriber.h"

#include "whisper.h"

#include <stdexcept>

using namespace std;

Transcriber::Transcriber(const string& modelPath) {
    whisper_context_params cparams = whisper_context_default_params();
    ctx = whisper_init_from_file_with_params(modelPath.c_str(), cparams);
    if (ctx == nullptr) {
        throw runtime_error("Could not load whisper model: " + modelPath);
    }
}

Transcriber::~Transcriber() {
    if (ctx != nullptr) {
        whisper_free(ctx);
    }
}

void Transcriber::setPrompt(const string& text) {
    prompt = text;
}

string Transcriber::transcribe(const vector<float>& audio) {
    // whisper does not like very short clips, so pad quiet audio up to 1 second.
    vector<float> input = audio;
    if (input.size() < 16000) {
        input.resize(16000, 0.0f);
    }

    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.language = "en";
    params.n_threads = 4;
    params.translate = false;
    params.no_timestamps = true;
    params.single_segment = true; // commands are short, one segment is enough
    params.print_progress = false;
    params.print_realtime = false;
    params.print_timestamps = false;
    params.print_special = false;
    if (!prompt.empty()) {
        // Short isolated words are easy to misread, so give whisper the
        // list of words we actually expect.
        params.initial_prompt = prompt.c_str();
    }

    if (whisper_full(ctx, params, input.data(), (int)input.size()) != 0) {
        throw runtime_error("whisper_full failed");
    }

    string text;
    for (int i = 0; i < whisper_full_n_segments(ctx); i++) {
        text += whisper_full_get_segment_text(ctx, i);
    }
    return text;
}
