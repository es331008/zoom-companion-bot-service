#include "BotInstance.h"

BotInstance::BotInstance(const string& joinUrl)
    : joinUrl_(joinUrl), 
    running_(false), 
    status_(BotStatus::NotStarted),
    azureSpeechManager_(make_unique<AzureSpeechManager>(*this)),
    virtualAudioInterface_(make_unique<VirtualAudioInterface>()) {
    azureSpeechManager_->initialize("9dOvrKeS2L1GhSCfcTrGlfrfNYn1bTg3ymEBPtM6e1jreJiRVw9jJQQJ99BEACYeBjFXJ3w3AAAYACOGh7to", "eastus");
}

BotInstance::~BotInstance() {
    stop();
}

void BotInstance::start() {
    if (running_) return;
    running_ = true;

    botThread_ = thread(&BotInstance::runBot, this);
}

void BotInstance::stop() {
    if (!running_) return;
    running_ = false;

    // Add any cleanup logic here
    azureSpeechManager_->shutdown();
    virtualAudioInterface_->stop();
    zoomClient_->clean();

    // TODO: Fix this thread deadlock caused by it trying to join itself
    if (botThread_.joinable())
        botThread_.join();
}

bool BotInstance::isRunning() const {
    return running_;
}

BotStatus BotInstance::getStatus() const {
    return status_;
}

void BotInstance::onAuthSuccess() {
    auto err = zoomClient_->joinMeeting(joinUrl_);
    if (CompanionUtils::hasSDKError(err, "Successfully joined meeting", "Unable to join meeting")) {
        status_ = BotStatus::JoinFailed;
        return;
    }
}

void BotInstance::onAuthFailure() const {
    Log::error("Failed to authenticate with Zoom");
    // stop the bot here
}

void BotInstance::onJoinSuccess() {
    auto err = zoomClient_->configureChatController();
    if (CompanionUtils::hasSDKError(err, "Chat controller configured", "Unable to configure chat controller")) {
        status_ = BotStatus::Crashed;
        return;
    }

    err = zoomClient_->sendChatMessage(
        "Hi everyone! I'm Meeting Bot, a virtual assistant here to help monitor and support today's session. I'm not a real participant, but I'm here to observe, take notes, or assist with automated tasks if needed. Let me know if there's anything you'd like me to pass along!",
        SDKChatMessageType_To_All
    );
    if (CompanionUtils::hasSDKError(err, "Welcome chat sent", "Unable to send welcome chat")) {
        status_ = BotStatus::Crashed;
        return;
    }

    zoomClient_->configureMeetingInfo();

    Log::info("Starting virtual audio interface");
    virtualAudioInterface_->start(
        [&](const BYTE* data, size_t size) {
            azureSpeechManager_->pushAudio(const_cast<BYTE*>(data), size);
        }
    );

    Log::info("Starting Azure stream");
    azureSpeechManager_->startStreaming();
}

void BotInstance::onJoinFailure() const {
    Log::error("Failed to join meeting");
    // stop the bot here
}

void BotInstance::setMeetingInfo(const MeetingInfo& meetingInfo) {
    meetingInfo_ = meetingInfo;
}

MeetingInfo BotInstance::getMeetingInfo() const {
    return meetingInfo_;
}

void BotInstance::runBot() {
    try {
        SDKError err{ SDKERR_SUCCESS };
        status_ = BotStatus::Running;
        running_ = true;
        zoomClient_ = make_unique<ZoomClient>(*this);

        err = zoomClient_->init();
        if (CompanionUtils::hasSDKError(err, "SDK initialized", "Unable to initialize SDK")) {
            status_ = BotStatus::Crashed;
            return;
        }

        err = zoomClient_->createServices();
        if (CompanionUtils::hasSDKError(err, "Service creation complete", "Unable to create services")) {
            status_ = BotStatus::Crashed;
            return;
        }

        err = zoomClient_->configureServices();
        if (CompanionUtils::hasSDKError(err, "Service configuration complete", "Unable to configure services")) {
            status_ = BotStatus::Crashed;
            return;
        }

        err = zoomClient_->auth();
        if (CompanionUtils::hasSDKError(err, "SDK authentication process started...", "Unable to start SDK authentication process")) {
            status_ = BotStatus::Crashed;
            return;
        }

        while (running_) {
            // Run message pump on the current thread to receive events
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            this_thread::sleep_for(chrono::milliseconds(10));
        }

        status_ = BotStatus::Stopped;
    } catch (const exception& e) {
        Log::error(string("Bot crashed: ") + e.what());
        status_ = BotStatus::Crashed;
    } catch (...) {
        Log::error("Bot crashed due to unknown error");
        status_ = BotStatus::Crashed;
    }
}

void BotInstance::onTextRecognized(const string& text) {
    recognizedText_.push_back(text);
}

void BotInstance::onChatReceived(const ChatMessage& msg) {
    meetingInfo_.chatHistory.push_back(msg);
}

void BotInstance::onMeetingEnd() {
    Log::info("Disconnecting from meeting");
    if (shutdownCallback_) {
        shutdownCallback_(this);
    }
}

void BotInstance::setShutdownCallback(function<void(BotInstance*)> callback) {
    shutdownCallback_ = move(callback);
}