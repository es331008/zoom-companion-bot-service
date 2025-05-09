#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <vector>
#include <string>

#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct ChatMessage {
    string senderName;
    int senderId;
    int receiverId;
    string message;
    string timestamp;
};

void to_json(json& j, const ChatMessage& chatMessage);
void from_json(const json& j, ChatMessage& chatMessage);

#endif // CHAT_MESSAGE_H
