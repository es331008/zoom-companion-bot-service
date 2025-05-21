#ifndef ZOOM_H
#define ZOOM_H

#include <string>
#include <locale>
#include <codecvt>
#include <chrono>

#include "util/Singleton.h"
#include "util/Log.h"
#include "util/CompanionUtils.h"

#include "httplib.h"

#include "zoom_sdk.h"
#include "./events/ZoomAuthEventHandler.h"
#include "./events/ZoomMeetingEventHandler.h"
#include "./events/ZoomMeetingChatCtrlEventHandler.h"
#include "./structs/MeetingInfo.h"

#include "json.hpp"
#include "zoom_sdk_def.h"

#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "meeting_service_components/meeting_recording_interface.h"
#include "auth_service_interface.h"
#include "./virtual-audio/VirtualAudioInterface.h"
#include "../speech/AzureSpeechManager.h"

using namespace std;
using namespace ZOOMSDK;
using namespace httplib;
using namespace chrono;
using json = nlohmann::json;

typedef time_point<system_clock> ctm_time_point;

class Zoom : public Singleton<Zoom> {
    friend class Singleton<Zoom>;

    string m_jwt;

    ctm_time_point m_iat;
    ctm_time_point m_exp;

    IAuthService* m_authService;
    IMeetingService* m_meetingService;

    SDKError createServices();
    void getAuthJwt();

    function<void()> onJoin = [&]() {
        Log::success("Joined meeting!");
        registerChatHandlerAndSendWelcome();

        meetingInfo.meetingNumber = to_string(m_meetingService->GetMeetingInfo()->GetMeetingNumber());
        meetingInfo.meetingTopic = CompanionUtils::zcharToString(m_meetingService->GetMeetingInfo()->GetMeetingTopic());

        AzureSpeechManager::getInstance().startStreaming();
        VirtualAudioInterface::getInstance().start(
            [&](const BYTE* data, size_t size) {
                AzureSpeechManager::getInstance().pushAudio(const_cast<BYTE*>(data), size);
            }
        );
    };

public:
    MeetingInfo meetingInfo;
    vector<string> recognizedText;

    Zoom() {};
    SDKError init();
    SDKError auth();

    SDKError join(string joinUrl);
    SDKError leave();

    SDKError registerChatHandlerAndSendWelcome();

    SDKError clean();

    static bool hasError(SDKError e, const string& action = "");
};

#endif //ZOOM_H