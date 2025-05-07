#include "ZoomMeetingEventHandler.h"

ZoomMeetingEventHandler::ZoomMeetingEventHandler() {}
ZoomMeetingEventHandler::~ZoomMeetingEventHandler() {}

void ZoomMeetingEventHandler::onMeetingStatusChanged(MeetingStatus status, int iResult) {
    if (status == MEETING_STATUS_INMEETING) {
        m_onMeetingJoin();
    }

    // Should really be looking at the meeting end code
    if (status == MEETING_STATUS_ENDED) {
        // There's a better way to do all this

        //Get the zoom instance
        auto* zoom = &Zoom::getInstance();
        
        // Get the meeting info
        Log::info("Meeting info");
        Log::info(zoom->meetingInfo.meetingNumber);
        Log::info(zoom->meetingInfo.meetingTopic);

        // Get the chat history
        Log::info("Meeting chat");
        for (const auto& msg : zoom->meetingInfo.chatHistory) {
            Log::info(msg.senderName);
            Log::info(to_string(msg.receiverId));
            Log::info(msg.message);
            Log::info(msg.timestamp);
        }

        // Get the summary
        Log::info("Meeting summary");

        json j = zoom->meetingInfo;
        std::cout << j.dump(4) << std::endl;
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