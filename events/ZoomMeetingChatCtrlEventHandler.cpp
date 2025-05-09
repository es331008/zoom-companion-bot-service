#include "ZoomMeetingChatCtrlEventHandler.h"

// Move these convert functions to a util class
static string zcharToString(const zchar_t* zstr) {
    if (!zstr) return "";

    std::wstring wstr(zstr);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

static string timeToString(time_t t) {
    return to_string(t);
}

void ZoomMeetingChatCtrlEventHandler::onChatMsgNotification(IChatMsgInfo* chatMsg, const zchar_t* content) {
    wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    ChatMessage msg;
    msg.senderName = zcharToString(chatMsg->GetSenderDisplayName());
    msg.senderId = chatMsg->GetSenderUserId();
    msg.receiverId = chatMsg->GetReceiverUserId();
    msg.message = zcharToString(chatMsg->GetContent());
    msg.timestamp = timeToString(chatMsg->GetTimeStamp());

    // There's a better way to do this
    auto* zoom = &Zoom::getInstance();
    zoom->meetingInfo.chatHistory.push_back(msg);
};