#include "ZoomClient.h"
#include "../bot/BotInstance.h"

ZoomClient::ZoomClient(BotInstance& owner) : owner_(owner), meetingService_(nullptr), authService_(nullptr), meetingChatController_(nullptr) {}

SDKError ZoomClient::init() {
    auto host = L"https://zoom.us";

    InitParam initParam;
    initParam.strWebDomain = host;
    initParam.enableLogByDefault = true;

    Log::info("Starting SDK initialization...");
    return InitSDK(initParam);
}

SDKError ZoomClient::createServices() {
    Log::info("Creating services...");
    
    auto err = CreateMeetingService(&meetingService_);
    if (CompanionUtils::hasSDKError(err, "Meeting service created", "Unable to create meeting service")) return err;

    err = CreateAuthService(&authService_);
    if (CompanionUtils::hasSDKError(err, "Auth service created", "Unable to create auth service")) return err;

    // Add additional service creation here, follow above pattern and return SDKError

    return err;
}

SDKError ZoomClient::configureServices() {
    Log::info("Configuring services...");

    auto err = meetingService_->SetEvent(this);
    if (CompanionUtils::hasSDKError(err, "Meeting service configured", "Unable to configure meeting service")) return err;

    err = authService_->SetEvent(this);
    if (CompanionUtils::hasSDKError(err, "Auth service configured", "Unable to configure auth service")) return err;

    // Add additional service configuration here, follow above pattern and return SDKError

    return err;
}

SDKError ZoomClient::auth() {
    string jwt_;
    SDKError err{ SDKERR_UNINITIALIZE };
    Client httpClient("localhost", 8000);

    auto response = httpClient.Get("/api/auth/generate-sdk-jwt");
    if (response && response->status == 200) {
        json responseJson = json::parse(response->body);
        if (responseJson.contains("jwt") && responseJson["jwt"].is_string()) {
            jwt_ = responseJson["jwt"];;
        }
        else {
            Log::error("[Auth] Missing or invalid 'jwt' field in response.");
            return err;
        }
    }
    else {
        Log::error("[Auth] Request failed with status : " + response->status);
        return err;
    }

    AuthContext ctx;
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wJwt = converter.from_bytes(jwt_);
    ctx.jwt_token = wJwt.c_str();

    return authService_->SDKAuth(ctx);
}

SDKError ZoomClient::joinMeeting(const string &joinUrl) {
    string meetingId, password;

    if (CompanionUtils::parseJoinUrl(joinUrl, meetingId, password)) {
        auto meetingNumber = stoull(meetingId);
        auto userName = L"Meeting Bot";
        wstring wpassword(password.begin(), password.end());
        auto psw = wpassword.c_str();

        JoinParam joinParam;
        joinParam.userType = ZOOM_SDK_NAMESPACE::SDK_UT_WITHOUT_LOGIN;

        JoinParam4WithoutLogin& param = joinParam.param.withoutloginuserJoin;

        param.meetingNumber = meetingNumber;
        param.userName = userName;
        param.psw = psw;
        param.vanityID = nullptr;
        param.customer_key = nullptr;
        param.webinarToken = nullptr;
        param.isVideoOff = true;
        param.isAudioOff = false;

        return meetingService_->Join(joinParam);
    }
    else {
        return SDKERR_UNINITIALIZE;
    }
}

void ZoomClient::clean() {
    if (meetingService_)
        DestroyMeetingService(meetingService_);

    if (authService_)
        DestroyAuthService(authService_);

    CleanUPSDK();
}

SDKError ZoomClient::configureChatController() {
    meetingChatController_ = meetingService_->GetMeetingChatController();
    return meetingChatController_->SetEvent(this);
}

SDKError ZoomClient::sendChatMessage(const string& message, SDKChatMessageType chatMessageType) {
    IChatMsgInfoBuilder* chatMsgInfoBuilder = meetingChatController_->GetChatMessageBuilder();
    chatMsgInfoBuilder->SetContent(L"Hi everyone! I'm Meeting Bot, a virtual assistant here to help monitor and support today's session. I'm not a real participant, but I'm here to observe, take notes, or assist with automated tasks if needed. Let me know if there's anything you'd like me to pass along!");
    chatMsgInfoBuilder->SetMessageType(SDKChatMessageType_To_All);
    return meetingChatController_->SendChatMsgTo(chatMsgInfoBuilder->Build());
}

void ZoomClient::configureMeetingInfo() {
    MeetingInfo meetingInfo;
    meetingInfo.meetingNumber = to_string(meetingService_->GetMeetingInfo()->GetMeetingNumber());
    meetingInfo.meetingTopic = CompanionUtils::zcharToString(meetingService_->GetMeetingInfo()->GetMeetingTopic());

    owner_.setMeetingInfo(meetingInfo);
}

SDKError ZoomClient::leaveMeeting() {
    return meetingService_->Leave(LEAVE_MEETING);
}

void ZoomClient::onAuthenticationReturn(AuthResult result) {
    if (result == AuthResult::AUTHRET_SUCCESS) {
        Log::success("Zoom authentication successful");
        owner_.onAuthSuccess();
    }
    else {
        Log::error("Zoom authentication failed");
        owner_.onAuthFailure();
    }
}

void ZoomClient::onMeetingStatusChanged(MeetingStatus status, int iResult) {
    switch (status) {
    case MEETING_STATUS_ENDED:
    case MEETING_STATUS_FAILED:
    case MEETING_STATUS_DISCONNECTING:
        owner_.onMeetingEnd();
        break;
    case MEETING_STATUS_INMEETING:
        owner_.onJoinSuccess();
        break;
    default:
        break;
    }
}

void ZoomClient::onChatMsgNotification(IChatMsgInfo* chatMsg, const zchar_t* content) {
    ChatMessage msg;

    msg.senderName = CompanionUtils::zcharToString(chatMsg->GetSenderDisplayName());
    msg.senderId = chatMsg->GetSenderUserId();
    msg.receiverId = chatMsg->GetReceiverUserId();
    msg.message = CompanionUtils::zcharToString(chatMsg->GetContent());
    msg.timestamp = CompanionUtils::timeToString(chatMsg->GetTimeStamp());

    owner_.onChatReceived(msg);
}

