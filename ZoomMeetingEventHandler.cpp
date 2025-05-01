#include "ZoomMeetingEventHandler.h"

ZoomMeetingEventHandler::ZoomMeetingEventHandler() {}
ZoomMeetingEventHandler::~ZoomMeetingEventHandler() {}

void ZoomMeetingEventHandler::onMeetingStatusChanged(MeetingStatus status, int iResult) {
    cout << "[Mtng] Meeting status changed: " << status << ", result: " << iResult << endl;

    if (status == MEETING_STATUS_INMEETING) {
        wcout << L"[Meeting] We're now fully in the meeting!" << endl;
    }
}

void ZoomMeetingEventHandler::onMeetingStatisticsWarningNotification(StatisticsWarningType type) {}
void ZoomMeetingEventHandler::onMeetingParameterNotification(const MeetingParameter* meeting_param) {}
void ZoomMeetingEventHandler::onSuspendParticipantsActivities() {}
void ZoomMeetingEventHandler::onAICompanionActiveChangeNotice(bool bActive) {}
void ZoomMeetingEventHandler::onMeetingTopicChanged(const zchar_t* sTopic) {}
void ZoomMeetingEventHandler::onMeetingFullToWatchLiveStream(const zchar_t* sLiveStreamUrl) {}