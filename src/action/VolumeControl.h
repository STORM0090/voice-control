#ifndef VOICECONTROL_VOLUME_CONTROL_H
#define VOICECONTROL_VOLUME_CONTROL_H

struct IAudioEndpointVolume; // avoids including the Core Audio headers here

// Sets the master volume by percentage instead of pressing volume keys.
class VolumeControl {
public:
    VolumeControl();
    ~VolumeControl();

    bool setPercent(int value);
    bool changeBy(int delta);

private:
    int percent() const; // -1 when it cannot be read

    IAudioEndpointVolume* endpoint;
};

#endif
