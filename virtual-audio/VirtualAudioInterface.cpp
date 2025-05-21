#include "VirtualAudioInterface.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "avrt.lib")

#define REFTIMES_PER_SEC  10000000

VirtualAudioInterface::VirtualAudioInterface() {
    running_ = false;
    stopRequested_ = false;
    pEnumerator_ = nullptr;
    pDevice_ = nullptr;
    pAudioClient_ = nullptr;
    pAudioClient_ = nullptr;
    pCaptureClient_ = nullptr;
    pCaptureClient_ = nullptr;
    pwfx_ = nullptr;
    CoInitialize(nullptr);
}

VirtualAudioInterface::~VirtualAudioInterface() {
    stop();
    if (pwfx_) {
        CoTaskMemFree(pwfx_);
        pwfx_ = nullptr;
    }
    CoUninitialize();
}

void VirtualAudioInterface::logWaveFormat(WAVEFORMATEX* pwfx) {
    if (!pwfx) return;

    Log::info("Audio Format Details:");
    Log::info("  Format Tag: 0x" + to_string(pwfx->wFormatTag));
    Log::info("  Channels: " + to_string(pwfx->nChannels));
    Log::info("  Samples per Sec: " + to_string(pwfx->nSamplesPerSec));
    Log::info("  Avg Bytes per Sec: " + to_string(pwfx->nAvgBytesPerSec));
    Log::info("  Block Align: " + to_string(pwfx->nBlockAlign));
    Log::info("  Bits per Sample: " + to_string(pwfx->wBitsPerSample));
    Log::info("  Extra Size: " + to_string(pwfx->cbSize));

    // Optional: print format type
    switch (pwfx->wFormatTag) {
    case WAVE_FORMAT_PCM:
        Log::info("  Format: PCM");
        break;
    case WAVE_FORMAT_IEEE_FLOAT:
        Log::info("  Format: IEEE Float");
        break;
    case WAVE_FORMAT_EXTENSIBLE:
        Log::info("  Format: Extensible");
        break;
    default:
        Log::info("  Format: Unknown");
    }
}

bool VirtualAudioInterface::start(AudioCallback audioCallback) {
    if (running_) return false;

    stopRequested_ = false;
    running_ = true;
    audioCallback_ = audioCallback;

    captureThread_ = thread(&VirtualAudioInterface::captureLoop, this, -1);
    return true;
}

void VirtualAudioInterface::stop() {
    if (!running_) return;

    stopRequested_ = true;
    if (captureThread_.joinable())
        captureThread_.join();

    running_ = false;
}

bool VirtualAudioInterface::isRunning() const {
    return running_;
}

void VirtualAudioInterface::captureLoop(int durationSeconds) {
    HRESULT hr;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator_);
    if (FAILED(hr)) return;

    hr = pEnumerator_->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice_);
    if (FAILED(hr)) return;

    hr = pDevice_->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient_);
    if (FAILED(hr)) return;

    hr = pAudioClient_->GetMixFormat(&pwfx_);
    logWaveFormat(pwfx_);
    if (FAILED(hr)) return;

    hr = pAudioClient_->Initialize(AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK,
        REFTIMES_PER_SEC, 0, pwfx_, NULL);
    if (FAILED(hr)) return;

    hr = pAudioClient_->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient_);
    if (FAILED(hr)) return;

    hr = pAudioClient_->Start();
    if (FAILED(hr)) return;

    auto startTime = steady_clock::now();

    while (!stopRequested_) {
        if (durationSeconds > 0) {
            auto elapsed = steady_clock::now() - startTime;
            if (duration_cast<seconds>(elapsed).count() >= durationSeconds)
                break;
        }

        Sleep(10); // small sleep to avoid hogging CPU

        UINT32 packetLength = 0;
        hr = pCaptureClient_->GetNextPacketSize(&packetLength);
        while (packetLength != 0) {
            BYTE* pData = nullptr;
            UINT32 numFramesAvailable = 0;
            DWORD flags = 0;

            hr = pCaptureClient_->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
            if (FAILED(hr)) break;

            if (pData && audioCallback_) {
                size_t byteCount = numFramesAvailable * pwfx_->nBlockAlign;
                audioCallback_(pData, byteCount);
            }

            pCaptureClient_->ReleaseBuffer(numFramesAvailable);
            hr = pCaptureClient_->GetNextPacketSize(&packetLength);
        }
    }

    pAudioClient_->Stop();

    if (pwfx_) CoTaskMemFree(pwfx_);
    if (pCaptureClient_) pCaptureClient_->Release();
    if (pAudioClient_) pAudioClient_->Release();
    if (pDevice_) pDevice_->Release();
    if (pEnumerator_) pEnumerator_->Release();

    pEnumerator_ = nullptr;
    pDevice_ = nullptr;
    pAudioClient_ = nullptr;
    pCaptureClient_ = nullptr;
    pwfx_ = nullptr;
}
