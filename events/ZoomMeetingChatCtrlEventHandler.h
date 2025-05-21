#ifndef ZOOMMEETINGCHATCTRLEVENTHANDLER_H
#define ZOOMMEETINGCHATCTRLEVENTHANDLER_H

// This shouldn't be needed but for whatever reason, it'll try to build with Unix if it's not included here
#include "httplib.h"

#include <codecvt>
#include <ctime>

#include "meeting_service_components/meeting_chat_interface.h"
#include "../util/Log.h"
#include "../util/CompanionUtils.h"
#include "../structs/MeetingInfo.h"
#include "../Zoom.h"

using namespace ZOOM_SDK_NAMESPACE;
using namespace std;

class ZoomMeetingChatCtrlEventHandler : public IMeetingChatCtrlEvent {
public:
	ZoomMeetingChatCtrlEventHandler() {};
	~ZoomMeetingChatCtrlEventHandler() {};

	void onChatMsgNotification(IChatMsgInfo* chatMsg, const zchar_t* content = nullptr) override;
	void onChatStatusChangedNotification(ChatStatus* status_) override {};
	void onChatMsgDeleteNotification(const zchar_t* msgID, SDKChatMessageDeleteType deleteBy) override {};
	void onChatMessageEditNotification(IChatMsgInfo* chatMsg) override {};

	void onShareMeetingChatStatusChanged(bool isStart) override {};
	void onFileSendStart(ISDKFileSender* sender) override {};
	void onFileReceived(ISDKFileReceiver* receiver) override {};
	void onFileTransferProgress(SDKFileTransferInfo* info) override {};

};

#endif // ZOOMMEETINGCHATCTRLEVENTHANDLER_H