#include "MeetingInfo.h"

void to_json(json& j, const MeetingInfo& meetingInfo) {
    j = json{
        {"meetingNumber", meetingInfo.meetingNumber},
        {"meetingTopic", meetingInfo.meetingTopic},
        {"chatHistory", meetingInfo.chatHistory},
        {"meetingSummary", meetingInfo.meetingSummary}
    };
}

void from_json(const json& j, MeetingInfo& meetingInfo) {
    j.at("meetingNumber").get_to(meetingInfo.meetingNumber);
    j.at("meetingTopic").get_to(meetingInfo.meetingTopic);
    j.at("chatHistory").get_to(meetingInfo.chatHistory);
    j.at("meetingSummary").get_to(meetingInfo.meetingSummary);
}