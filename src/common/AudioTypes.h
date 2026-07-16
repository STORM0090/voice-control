#ifndef VOICECONTROL_AUDIO_TYPES_H
#define VOICECONTROL_AUDIO_TYPES_H

// whisper needs 16 kHz mono, so that is the format we produce.
const int kSampleRate = 16000;
const int kFrameMs = 20;
const int kFrameSize = kSampleRate * kFrameMs / 1000; // 320 samples

#endif
