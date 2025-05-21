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
    auto audioFormat = AudioStreamFormat::GetWaveFormatPCM(44100, 16, 2); // stereo 16-bit 44.1kHz
    stream_ = AudioInputStream::CreatePushStream(audioFormat);
    audioConfig_ = AudioConfig::FromStreamInput(stream_);
    recognizer_ = SpeechRecognizer::FromConfig(config_, audioConfig_);

    recognizer_->Recognizing.Connect([](const auto& e) {
        cout << "[Azure Recognizing] " << e.Result->Text << endl;
    });

    recognizer_->Recognized.Connect([](const auto& e) {
        cout << "[Azure Recognized] " << e.Result->Text << endl;

        recognizedText.push_back(e.Result->Text);
    });

    initialized_ = true;
}

void AzureSpeechManager::startStreaming() {
    if (running_) return;
    
    recognizer_->StartContinuousRecognitionAsync().get();
}

void AzureSpeechManager::pushAudio(uint8_t* data, size_t size)
{
    lock_guard<mutex> lock(mutex_);
    if (!stream_) return;

    size_t numSamples = size / sizeof(float);
    float* floatSamples = reinterpret_cast<float*>(data);

    std::vector<int16_t> pcm16(numSamples);
    for (size_t i = 0; i < numSamples; ++i) {
        float sample = CompanionUtils::clamp(floatSamples[i], -1.0f, 1.0f);
        pcm16[i] = static_cast<int16_t>(sample * 32767.0f);
    }

    stream_->Write(reinterpret_cast<uint8_t*>(pcm16.data()), static_cast<uint32_t>(pcm16.size() * sizeof(int16_t)));
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
