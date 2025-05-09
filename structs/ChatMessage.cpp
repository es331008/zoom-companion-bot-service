#include "ChatMessage.h"

void to_json(json& j, const ChatMessage& chatMessage) {
    j = json{
        {"senderName", chatMessage.senderName},
        {"senderId", chatMessage.senderId},
        {"receiverId", chatMessage.receiverId},
        {"message", chatMessage.message},
        {"timestamp", chatMessage.timestamp}
    };
}

void from_json(const json& j, ChatMessage& chatMessage) {
    j.at("senderName").get_to(chatMessage.senderName);
    j.at("senderId").get_to(chatMessage.senderId);
    j.at("receiverId").get_to(chatMessage.receiverId);
    j.at("message").get_to(chatMessage.message);
    j.at("timestamp").get_to(chatMessage.timestamp);
}