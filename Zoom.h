#ifndef ZOOM_H
#define ZOOM_H



#include <string>
#include <locale>
#include <codecvt>
#include <chrono>

#include "util/Singleton.h"
#include "util/Log.h"

#include "httplib.h"

#include "zoom_sdk.h"
#include "./events/ZoomAuthEventHandler.h"
#include "./events/ZoomMeetingEventHandler.h"
#include "./events/MeetingRecordingCtrlEventHandler.h"
#include "./events/ZoomMeetingChatCtrlEventHandler.h"
#include "ZoomSDKAudioRawDataDelegate.h"
#include "MeetingInfo.h"

#include "json.hpp"
#include "zoom_sdk_def.h"

#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "meeting_service_components/meeting_recording_interface.h"
//#include "meeting_service_components/meeting_participants_ctrl_interface.h"
#include "rawdata/rawdata_audio_helper_interface.h"
#include "rawdata/zoom_rawdata_api.h"
#include "auth_service_interface.h"

using namespace std;
using namespace ZOOMSDK;
using namespace httplib;
using json = nlohmann::json;

typedef chrono::time_point<chrono::system_clock> time_point;

class Zoom : public Singleton<Zoom> {
    friend class Singleton<Zoom>;

    string m_jwt;

    time_point m_iat;
    time_point m_exp;

    IAuthService* m_authService;
    IMeetingService* m_meetingService;
    IZoomSDKAudioRawDataHelper* m_audioHelper;
    ZoomSDKAudioRawDataDelegate* m_audioSource;

    SDKError createServices();
    void getAuthJwt();

    // Again, this should be centralized
    static string zcharToString(const zchar_t* zstr) {
        if (!zstr) return "";

        std::wstring wstr(zstr);
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }

    function<void(bool)> onRecordingPrivilegeChanged = [&](bool canRec) {
        if (canRec)
            startRawRecording();
        else
            stopRawRecording();
    };

    function<void()> onJoin = [&]() {
        Log::success("Joined meeting!");
        registerChatHandlerAndSendWelcome();

        // Set meeting info
        meetingInfo.meetingNumber = to_string(m_meetingService->GetMeetingInfo()->GetMeetingNumber());
        meetingInfo.meetingTopic = zcharToString(m_meetingService->GetMeetingInfo()->GetMeetingTopic());

        //IMeetingRecordingController* recordingCtrl = m_meetingService->GetMeetingRecordingController();
        //IMeetingRecordingCtrlEvent* recordingEvent = new MeetingRecordingCtrlEventHandler(onRecordingPrivilegeChanged);
        //recordingCtrl->SetEvent(recordingEvent);
        //startRawRecording();
    };

public:
    // This stuff should be cleared whenever a new meeting is joined
    MeetingInfo meetingInfo;

    Zoom();
    SDKError init();
    SDKError auth();

    SDKError join(string joinUrl);
    SDKError leave();

    SDKError registerChatHandlerAndSendWelcome();
    SDKError startRawRecording();
    SDKError stopRawRecording();

    SDKError clean();

    static bool hasError(SDKError e, const string& action = "");
};

#endif //ZOOM_H