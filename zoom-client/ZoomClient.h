#pragma once

#include "httplib.h"
#include "json.hpp"
#include "zoom_sdk.h"
#include "auth_service_interface.h"
#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"

#include "../util/CompanionUtils.h"
#include "../util/Log.h"

class BotInstance;

using namespace ZOOMSDK;
using namespace httplib;
using json = nlohmann::json;

class ZoomClient : public IAuthServiceEvent, IMeetingServiceEvent, IMeetingChatCtrlEvent {
public:
	ZoomClient(BotInstance& bot);
    ~ZoomClient() = default;
	SDKError init();
	SDKError createServices();
	SDKError configureServices();
	SDKError auth();
	SDKError joinMeeting(const string& joinUrl);
    SDKError configureChatController();
    void configureMeetingInfo();
    SDKError sendChatMessage(const string& message, SDKChatMessageType chatMessageType);
	void clean();
    SDKError leaveMeeting();

    // IAuthServiceEvent functions
    void onAuthenticationReturn(AuthResult result) override;
    void onLoginReturnWithReason(LOGINSTATUS, IAccountInfo*, LoginFailReason) override {};
    void onLogout() override {};
    void onZoomIdentityExpired() override {};
    void onZoomAuthIdentityExpired() override {};
    void onNotificationServiceStatus(SDKNotificationServiceStatus status, SDKNotificationServiceError error) override {};

    // IMeetingServiceEvent functions
    void onMeetingStatusChanged(MeetingStatus status, int iResult) override;
    void onMeetingStatisticsWarningNotification(StatisticsWarningType type) override {};
    void onMeetingParameterNotification(const MeetingParameter* meeting_param) override {};
    void onSuspendParticipantsActivities() override {};
    void onAICompanionActiveChangeNotice(bool bActive) override {};
    void onMeetingTopicChanged(const zchar_t* sTopic) override {};
    void onMeetingFullToWatchLiveStream(const zchar_t* sLiveStreamUrl) override {};

    // IMeetingChatCtrlEvent functions
    void onChatMsgNotification(IChatMsgInfo* chatMsg, const zchar_t* content = nullptr) override;
    void onChatStatusChangedNotification(ChatStatus* status_) override {};
    void onChatMsgDeleteNotification(const zchar_t* msgID, SDKChatMessageDeleteType deleteBy) override {};
    void onChatMessageEditNotification(IChatMsgInfo* chatMsg) override {};
    void onShareMeetingChatStatusChanged(bool isStart) override {};
    void onFileSendStart(ISDKFileSender* sender) override {};
    void onFileReceived(ISDKFileReceiver* receiver) override {};
    void onFileTransferProgress(SDKFileTransferInfo* info) override {};
private:
    IAuthService* authService_;
    IMeetingService* meetingService_;
    IMeetingChatController* meetingChatController_;
    BotInstance& owner_;
};

