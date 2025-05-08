#include "AzureSpeechManager.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

AzureSpeechManager::AzureSpeechManager() = default;
AzureSpeechManager::~AzureSpeechManager() = default;

void AzureSpeechManager::initialize(const string& key, const string& region)
{
    lock_guard<mutex> lock(mutex_);
    if (initialized_) return;

    config_ = SpeechConfig::FromSubscription(key, region);
    stream_ = AudioInputStream::CreatePushStream();
    audioConfig_ = AudioConfig::FromStreamInput(stream_);
    recognizer_ = SpeechRecognizer::FromConfig(config_, audioConfig_);

    recognizer_->Recognizing.Connect([](const auto& e) {
        cout << "[Azure Recognizing] " << e.Result->Text << endl;
    });

    recognizer_->Recognized.Connect([](const auto& e) {
        cout << "[Azure Recognized] " << e.Result->Text << endl;

        recognizedText.push_back(e.Result->Text);
    });

    recognizer_->StartContinuousRecognitionAsync().get();
    initialized_ = true;
}

void AzureSpeechManager::pushAudio(uint8_t* data, size_t size)
{
    lock_guard<mutex> lock(mutex_);
    if (stream_) {
        stream_->Write(data, static_cast<uint32_t>(size));
    }
}

void AzureSpeechManager::shutdown()
{
    lock_guard<mutex> lock(mutex_);
    if (recognizer_) {
        recognizer_->StopContinuousRecognitionAsync().get();
    }
    recognizer_.reset();
    stream_.reset();
    config_.reset();
    audioConfig_.reset();
    initialized_ = false;
}
