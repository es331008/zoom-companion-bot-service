#ifndef AZURE_SPEECH_MANAGER_H
#define AZURE_SPEECH_MANAGER_H

// This is likely setting the ssl flag off on httplib
#include <speechapi_cxx.h>
#include <memory>
#include <mutex>
#include <iostream>
#include "../util/Singleton.h"
#include "../Globals.h"

using namespace std;

class AzureSpeechManager : public Singleton<AzureSpeechManager>
{
    friend class Singleton<AzureSpeechManager>;

public:
    void initialize(const string& key, const string& region);
    void pushAudio(uint8_t* data, size_t size);
    void shutdown();

private:
    AzureSpeechManager();
    ~AzureSpeechManager();

    mutex mutex_;
    bool initialized_ = false;

    shared_ptr<Microsoft::CognitiveServices::Speech::SpeechConfig> config_;
    shared_ptr<Microsoft::CognitiveServices::Speech::Audio::PushAudioInputStream> stream_;
    shared_ptr<Microsoft::CognitiveServices::Speech::Audio::AudioConfig> audioConfig_;
    shared_ptr<Microsoft::CognitiveServices::Speech::SpeechRecognizer> recognizer_;
};

#endif // AZURE_SPEECH_MANAGER_H