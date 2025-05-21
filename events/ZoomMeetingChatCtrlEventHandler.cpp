#include "ZoomMeetingChatCtrlEventHandler.h"

void ZoomMeetingChatCtrlEventHandler::onChatMsgNotification(IChatMsgInfo* chatMsg, const zchar_t* content) {
    wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    ChatMessage msg;
    msg.senderName = CompanionUtils::zcharToString(chatMsg->GetSenderDisplayName());
    msg.senderId = chatMsg->GetSenderUserId();
    msg.receiverId = chatMsg->GetReceiverUserId();
    msg.message = CompanionUtils::zcharToString(chatMsg->GetContent());
    msg.timestamp = CompanionUtils::timeToString(chatMsg->GetTimeStamp());

    // There's a better way to do this
    auto* zoom = &Zoom::getInstance();
    zoom->meetingInfo.chatHistory.push_back(msg);
};