// windows.h must come before "using namespace std;" (std::byte vs Windows byte).
#include <windows.h>
#include <objbase.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <mmreg.h>

#include "audio/AudioCapture.h"
#include "audio/Resampler.h"
#include "common/AudioTypes.h"

#include <stdexcept>

using namespace std;

static void check(HRESULT hr, const char* what) {
    if (FAILED(hr)) {
        throw runtime_error(string("WASAPI error in ") + what);
    }
}

AudioCapture::AudioCapture(ThreadSafeQueue<vector<float>>& output)
    : output(output), running(false) {
}

AudioCapture::~AudioCapture() {
    stop();
}

void AudioCapture::start() {
    running = true;
    worker = thread(&AudioCapture::run, this);
}

void AudioCapture::stop() {
    running = false;
    if (worker.joinable()) {
        worker.join();
    }
}

void AudioCapture::run() {
    check(CoInitializeEx(nullptr, COINIT_MULTITHREADED), "CoInitializeEx");

    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioClient* audioClient = nullptr;
    IAudioCaptureClient* captureClient = nullptr;
    WAVEFORMATEX* format = nullptr;
    HANDLE bufferReady = nullptr;

    check(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                           __uuidof(IMMDeviceEnumerator), (void**)&enumerator),
          "CoCreateInstance");
    check(enumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &device),
          "GetDefaultAudioEndpoint");
    check(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                           (void**)&audioClient),
          "Activate");
    check(audioClient->GetMixFormat(&format), "GetMixFormat");

    bool isFloat = (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
                   (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
                    format->wBitsPerSample == 32);
    if (!isFloat) {
        throw runtime_error("Microphone is not 32-bit float; not supported yet.");
    }

    int deviceRate = format->nSamplesPerSec;
    int channels = format->nChannels;

    REFERENCE_TIME bufferDuration = 10000000; // 1 second in 100-ns units
    check(audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                  bufferDuration, 0, format, nullptr),
          "Initialize");
    bufferReady = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    check(audioClient->SetEventHandle(bufferReady), "SetEventHandle");
    check(audioClient->GetService(__uuidof(IAudioCaptureClient),
                                  (void**)&captureClient),
          "GetService");

    Resampler resampler(deviceRate, kSampleRate);
    vector<float> pending; // samples waiting to fill the next frame

    check(audioClient->Start(), "Start");

    while (running) {
        WaitForSingleObject(bufferReady, 200);

        UINT32 packetLength = 0;
        captureClient->GetNextPacketSize(&packetLength);
        while (packetLength > 0) {
            BYTE* data = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            captureClient->GetBuffer(&data, &numFrames, &flags, nullptr, nullptr);

            vector<float> mono;
            mono.reserve(numFrames);
            if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                mono.resize(numFrames, 0.0f);
            } else {
                float* samples = (float*)data;
                for (UINT32 i = 0; i < numFrames; i++) {
                    float sum = 0.0f;
                    for (int c = 0; c < channels; c++) {
                        sum += samples[i * channels + c];
                    }
                    mono.push_back(sum / channels);
                }
            }
            captureClient->ReleaseBuffer(numFrames);

            vector<float> resampled = resampler.process(mono);
            for (const auto& s : resampled) {
                pending.push_back(s);
            }
            while ((int)pending.size() >= kFrameSize) {
                vector<float> frame(pending.begin(), pending.begin() + kFrameSize);
                output.push(move(frame));
                pending.erase(pending.begin(), pending.begin() + kFrameSize);
            }

            captureClient->GetNextPacketSize(&packetLength);
        }
    }

    audioClient->Stop();
    if (bufferReady) CloseHandle(bufferReady);
    if (captureClient) captureClient->Release();
    if (audioClient) audioClient->Release();
    if (device) device->Release();
    if (enumerator) enumerator->Release();
    if (format) CoTaskMemFree(format);
    CoUninitialize();
}
