#ifndef ZOOMSDKAUDIORAWDATADELEGATE_H
#define ZOOMSDKAUDIORAWDATADELEGATE_H

#include <iostream>
#include <fstream>
#include <sstream>

#include "httplib.h"
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_audio_helper_interface.h"

#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKAudioRawDataDelegate : public IZoomSDKAudioRawDataDelegate {
public:
    ZoomSDKAudioRawDataDelegate() {};
    ~ZoomSDKAudioRawDataDelegate() {};

    void onMixedAudioRawDataReceived(AudioRawData* data) override;
    void onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) override;
    void onShareAudioRawDataReceived(AudioRawData* data) override;

    void onOneWayInterpreterAudioRawDataReceived(AudioRawData* data_, const zchar_t* pLanguageName) override {};
};

#endif //ZOOMSDKAUDIORAWDATADELEGATE_H