#include <windows.h>
#include <objbase.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

#include "action/VolumeControl.h"

VolumeControl::VolumeControl() {
    endpoint = nullptr;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IMMDeviceEnumerator* enumerator = nullptr;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)&enumerator))) {
        return;
    }

    // eRender is the playback device; the microphone would be eCapture.
    IMMDevice* device = nullptr;
    if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device))) {
        device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                         (void**)&endpoint);
        device->Release();
    }
    enumerator->Release();
}

VolumeControl::~VolumeControl() {
    if (endpoint != nullptr) {
        endpoint->Release();
    }
    CoUninitialize();
}

int VolumeControl::percent() const {
    if (endpoint == nullptr) {
        return -1;
    }
    float level = 0.0f;
    if (FAILED(endpoint->GetMasterVolumeLevelScalar(&level))) {
        return -1;
    }
    return (int)(level * 100.0f + 0.5f);
}

bool VolumeControl::setPercent(int value) {
    if (endpoint == nullptr) {
        return false;
    }
    if (value < 0) {
        value = 0;
    }
    if (value > 100) {
        value = 100;
    }
    return SUCCEEDED(endpoint->SetMasterVolumeLevelScalar(value / 100.0f, nullptr));
}

bool VolumeControl::changeBy(int delta) {
    int now = percent();
    if (now < 0) {
        return false;
    }
    return setPercent(now + delta);
}
