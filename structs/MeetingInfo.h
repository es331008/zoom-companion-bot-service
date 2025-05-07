#ifndef MEETING_INFO_H
#define MEETING_INFO_H

#include <vector>
#include <string>

#include "json.hpp"

#include "ChatMessage.h"

using namespace std;
using json = nlohmann::json;

struct MeetingInfo {
    string meetingNumber;
    string meetingTopic;

    vector<ChatMessage> chatHistory;
};

void to_json(json& j, const MeetingInfo& meetingInfo);
void from_json(const json& j, MeetingInfo& meetingInfo);

#endif // MEETING_INFO_H
