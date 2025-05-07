#include "Zoom.h"

static bool parseJoinUrl(const std::string& joinUrl, std::string& meetingId, std::string& password) {
    std::regex urlRegex(R"(https://[\w.-]+/j/(\d+)\?pwd=([\w-]+))");
    std::smatch match;

    if (std::regex_search(joinUrl, match, urlRegex) && match.size() == 3) {
        meetingId = match[1].str();
        password = match[2].str();
        return true;
    }
    return false;
}

Zoom::Zoom() {}

SDKError Zoom::init() {
    auto host = L"https://zoom.us";

    InitParam initParam;
    initParam.strWebDomain = host;
    initParam.enableLogByDefault = true;

    Log::info("Starting SDK initialization...");
    auto err = InitSDK(initParam);
    if (hasError(err)) {
        Log::error("InitSDK failed");
        return err;
    }

    return createServices();
}

SDKError Zoom::createServices() {
    Log::info("Creating services...");
    auto err = CreateMeetingService(&m_meetingService);
    if (hasError(err, "Meeting service created")) return err;

    auto meetingServiceEvent = new ZoomMeetingEventHandler();
    meetingServiceEvent->setOnMeetingJoin(onJoin);
    err = m_meetingService->SetEvent(meetingServiceEvent);
    if (hasError(err, "Meeting service configured")) return err;

    return err;
}

SDKError Zoom::auth() {
    Log::info("Starting SDK authentication");
    SDKError err{ SDKERR_UNINITIALIZE };

    err = CreateAuthService(&m_authService);
    if (hasError(err, "Auth service created")) return err;

    err = m_authService->SetEvent(new ZoomAuthEventHandler());
    if (hasError(err, "Auth service configured")) return err;

    getAuthJwt();

    AuthContext ctx;
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wJwt = converter.from_bytes(m_jwt);
    ctx.jwt_token = wJwt.c_str();

    return m_authService->SDKAuth(ctx);
}

void Zoom::getAuthJwt() {
    Client httpClient("localhost", 3000);

    auto response = httpClient.Get("/api/auth/generate-sdk-jwt");
    if (response && response->status == 200) {
        json responseJson = json::parse(response->body);
        if (responseJson.contains("jwt") && responseJson["jwt"].is_string()) {
            m_jwt = responseJson["jwt"];;
        }
        else {
            Log::error("[Auth] Missing or invalid 'jwt' field in response.");
            m_jwt = "";
        }
    }
    else {
        cerr << "[Auth] Request failed with status : " << response->status << endl;
    }
}

SDKError Zoom::join(string joinUrl) {
    string meetingId, password;

    if (parseJoinUrl(joinUrl, meetingId, password)) {
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
        param.isVideoOff = false;
        param.isAudioOff = false;

        return m_meetingService->Join(joinParam);
    }
    else {
        return SDKERR_UNINITIALIZE;
    }
}

SDKError Zoom::leave() {
    if (!m_meetingService)
        return SDKERR_UNINITIALIZE;

    auto status = m_meetingService->GetMeetingStatus();
    if (status == MEETING_STATUS_IDLE)
        return SDKERR_WRONG_USAGE;

    return  m_meetingService->Leave(LEAVE_MEETING);
}

SDKError Zoom::clean() {
    if (m_meetingService)
        DestroyMeetingService(m_meetingService);

    if (m_authService)
        DestroyAuthService(m_authService);

    return CleanUPSDK();
}

SDKError Zoom::registerChatHandlerAndSendWelcome() {
    IMeetingChatController* meetingChatController = m_meetingService->GetMeetingChatController();
    meetingChatController->SetEvent(new ZoomMeetingChatCtrlEventHandler());

    IChatMsgInfoBuilder* chatMsgInfoBuilder = meetingChatController->GetChatMessageBuilder();
    chatMsgInfoBuilder->SetContent(L"Hi everyone! I'm Meeting Bot, a virtual assistant here to help monitor and support today's session. I'm not a real participant, but I'm here to observe, take notes, or assist with automated tasks if needed. Let me know if there's anything you'd like me to pass along!");
    chatMsgInfoBuilder->SetMessageType(SDKChatMessageType_To_All);
    return meetingChatController->SendChatMsgTo(chatMsgInfoBuilder->Build());
}

SDKError Zoom::startRawRecording() {
    Log::info("Starting recording...");
    IMeetingRecordingController* recCtl = m_meetingService->GetMeetingRecordingController();

    SDKError err = recCtl->CanStartRawRecording();
    if (hasError(err, "Recording privilege granted")) {
        Log::info("Requesting local recording privilege");
        return recCtl->RequestLocalRecordingPrivilege();
    }

    err = recCtl->StartRawRecording();
    if (hasError(err, "Started raw recording"))
        return err;

    m_audioHelper = GetAudioRawdataHelper();
    if (!m_audioHelper)
        return SDKERR_UNINITIALIZE;

    if (!m_audioSource) {
        m_audioSource = new ZoomSDKAudioRawDataDelegate();
    }
    
    err = m_audioHelper->subscribe(m_audioSource);
    if (hasError(err, "Subscribed to raw audio"))
        return err;

    return SDKERR_SUCCESS;
};

SDKError Zoom::stopRawRecording() {
    Log::info("Stopping recording...");
    IMeetingRecordingController* recCtrl = m_meetingService->GetMeetingRecordingController();
    SDKError err = recCtrl->StopRawRecording();
    hasError(err, "Stopped raw recording");

    return err;
};

bool Zoom::hasError(const SDKError e, const string& action) {
    auto isError = e != SDKERR_SUCCESS;

    if (!action.empty()) {
        if (isError) {
            stringstream ss;
            ss << "failed to " << action << " with status " << e;
            Log::error(ss.str());
        }
        else {
            Log::success(action);
        }
    }
    return isError;
}