#include "ZoomSDKAudioRawDataDelegate.h"

void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData* data) {
    // Convert audio here to azure format

    auto buffer = data->GetBuffer();
    auto size = data->GetBufferLen();
    //AzureSpeechManager::getInstance().pushAudio(reinterpret_cast<uint8_t*>(buffer), size);
}

void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) {

}

void ZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived(AudioRawData* data) {

}
