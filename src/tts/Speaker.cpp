#include <windows.h>
#include <objbase.h>
#include <sapi.h>

#include "tts/Speaker.h"

#include <stdexcept>

using namespace std;

Speaker::Speaker() {
    voice = nullptr;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    HRESULT hr = CoCreateInstance(__uuidof(SpVoice), nullptr, CLSCTX_ALL,
                                  __uuidof(ISpVoice), (void**)&voice);
    if (FAILED(hr)) {
        throw runtime_error("Could not create the SAPI voice");
    }
}

Speaker::~Speaker() {
    if (voice != nullptr) {
        voice->Release();
    }
    CoUninitialize();
}

void Speaker::say(const string& text) {
    if (voice == nullptr) {
        return;
    }
    // SAPI wants wide strings; our confirmations are plain ASCII
    wstring wide(text.begin(), text.end());
    voice->Speak(wide.c_str(), 0, nullptr); // 0 = return only when finished
}
