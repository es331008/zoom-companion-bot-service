#include "ZoomSDKAudioRawDataDelegate.h"

void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData* data) {
    // Zoom bitrate is 32k, we need to downsample to 16k

    auto buffer = data->GetBuffer();
    auto size = data->GetBufferLen();

    // Safety check
    if (data->GetSampleRate() != 32000 || data->GetChannelNum() != 1) {
        std::cerr << "Unsupported audio format" << std::endl;
        return;
    }

    // THANKS, CHATGPT <3
    size_t numSamples = size / sizeof(int16_t);
    int16_t* samples32k = reinterpret_cast<int16_t*>(buffer);

    std::vector<int16_t> samples16k;
    samples16k.reserve(numSamples / 2);
    for (size_t i = 0; i + 1 < numSamples; i += 2) {
        int16_t avg = static_cast<int16_t>((samples32k[i] + samples32k[i + 1]) / 2);
        samples16k.push_back(avg);
    }

    AzureSpeechManager::getInstance().pushAudio(
        reinterpret_cast<uint8_t*>(samples16k.data()),
        samples16k.size() * sizeof(int16_t)
    );
}

void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) {

}

void ZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived(AudioRawData* data) {

}
