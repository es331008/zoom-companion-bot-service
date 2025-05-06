#ifndef ZOOMMEETINGEVENTHANDLER_H
#define ZOOMMEETINGEVENTHANDLER_H

// This shouldn't be needed but for whatever reason, it'll try to build with Unix if it's not included here
#include "httplib.h"

#include "meeting_service_interface.h"
#include "../util/Log.h"

using namespace ZOOM_SDK_NAMESPACE;
using namespace std;

class ZoomMeetingEventHandler : public IMeetingServiceEvent {
    function<void()> m_onMeetingJoin;

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

    void setOnMeetingJoin(const function<void()>& callback);
};

#endif // ZOOMMEETINGEVENTHANDLER_H