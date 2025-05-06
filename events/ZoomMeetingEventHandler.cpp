#include "ZoomMeetingEventHandler.h"

ZoomMeetingEventHandler::ZoomMeetingEventHandler() {}
ZoomMeetingEventHandler::~ZoomMeetingEventHandler() {}

void ZoomMeetingEventHandler::onMeetingStatusChanged(MeetingStatus status, int iResult) {
    if (status == MEETING_STATUS_INMEETING) {
        m_onMeetingJoin();
    }
}

void ZoomMeetingEventHandler::onMeetingStatisticsWarningNotification(StatisticsWarningType type) {}
void ZoomMeetingEventHandler::onMeetingParameterNotification(const MeetingParameter* meeting_param) {}
void ZoomMeetingEventHandler::onSuspendParticipantsActivities() {}
void ZoomMeetingEventHandler::onAICompanionActiveChangeNotice(bool bActive) {}
void ZoomMeetingEventHandler::onMeetingTopicChanged(const zchar_t* sTopic) {}
void ZoomMeetingEventHandler::onMeetingFullToWatchLiveStream(const zchar_t* sLiveStreamUrl) {}

void ZoomMeetingEventHandler::setOnMeetingJoin(const function<void()>& callback) {
    m_onMeetingJoin = callback;
}