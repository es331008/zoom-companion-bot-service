#pragma once

#include "../util/Singleton.h"
#include "../util/Log.h"

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <iostream>
#include <chrono>
#include <mmdeviceapi.h>
#include <audioclient.h>

using namespace std;
using namespace chrono;

class VirtualAudioInterface {

    using AudioCallback = std::function<void(const BYTE* data, size_t size)>;

public:
    VirtualAudioInterface();
    ~VirtualAudioInterface();

    bool start(AudioCallback audioCallback);
    void stop();
    void logWaveFormat(WAVEFORMATEX* pwfx);
    bool isRunning() const;

private: 
    thread captureThread_;
    atomic<bool> running_;
    atomic<bool> stopRequested_;
    AudioCallback audioCallback_;

    void captureLoop(int durationSeconds);

    IMMDeviceEnumerator* pEnumerator_;
    IMMDevice* pDevice_;
    IAudioClient* pAudioClient_;
    IAudioCaptureClient* pCaptureClient_;
    WAVEFORMATEX* pwfx_;


};

