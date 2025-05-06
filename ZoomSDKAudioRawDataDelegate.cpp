#include "ZoomSDKAudioRawDataDelegate.h"

void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData* data) {
    std::cout << "[AudioRawData] Capturing mixed audio: "
        << "Sample Rate = " << data->GetSampleRate()
        << ", Channel Num = " << data->GetChannelNum()
        << ", Data Size = " << data->GetBufferLen()
        << std::endl;
}

void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) {

}

void ZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived(AudioRawData* data) {

}
