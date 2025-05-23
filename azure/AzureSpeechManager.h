#ifndef AZURE_SPEECH_MANAGER_H
#define AZURE_SPEECH_MANAGER_H

// This is likely setting the ssl flag off on httplib
#include <speechapi_cxx.h>
#include <memory>
#include <mutex>
#include <iostream>
#include "../util/CompanionUtils.h"

class BotInstance;

using namespace std;

class AzureSpeechManager {
public:
    AzureSpeechManager(BotInstance& bot);
    ~AzureSpeechManager() = default;

    void initialize(const string& key, const string& region);
    void pushAudio(uint8_t* data, size_t size);
    void shutdown();
    void startStreaming();

private:
    mutex mutex_;
    bool initialized_ = false;

    shared_ptr<Microsoft::CognitiveServices::Speech::SpeechConfig> config_;
    shared_ptr<Microsoft::CognitiveServices::Speech::Audio::PushAudioInputStream> stream_;
    shared_ptr<Microsoft::CognitiveServices::Speech::Audio::AudioConfig> audioConfig_;
    shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> recognizer_;
    atomic<bool> running_;
    BotInstance& owner_;
};

#endif // AZURE_SPEECH_MANAGER_H