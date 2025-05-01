#pragma once

#include "httplib.h"
#include "zoom_sdk.h"
#include "zoom_sdk_def.h"
#include "json.hpp"
#include "meeting_service_interface.h"
#include "auth_service_interface.h"
#include <string>
#include <locale>
#include <codecvt>
#include <Windows.h>

using namespace ZOOM_SDK_NAMESPACE;
using namespace std;

class ZoomMeetingEventHandler : public IMeetingServiceEvent {
public:
    ZoomMeetingEventHandler();
    ~ZoomMeetingEventHandler();

    void onMeetingStatusChanged(MeetingStatus status, int iResult) override;
    void onMeetingStatisticsWarningNotification(StatisticsWarningType type) override;
    void onMeetingParameterNotification(const MeetingParameter* meeting_param) override;
    void onSuspendParticipantsActivities() override;
    void onAICompanionActiveChangeNotice(bool bActive) override;
    void onMeetingTopicChanged(const zchar_t* sTopic) override;
    void onMeetingFullToWatchLiveStream(const zchar_t* sLiveStreamUrl) override;
};