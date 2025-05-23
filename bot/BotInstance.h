#pragma once

#include <thread>
#include <atomic>
#include <string>
#include <memory> 

#include "BotStatus.h"
#include "../zoom-client/ZoomClient.h"
#include "../util/CompanionUtils.h"
#include "../structs/MeetingInfo.h"
#include "../virtual-audio/VirtualAudioInterface.h"
#include "../azure/AzureSpeechManager.h"

using namespace std;

class BotInstance {
public:
    BotInstance(const string& joinUrl);
    ~BotInstance();

    void start();
    void stop();
    bool isRunning() const;
    BotStatus getStatus() const;
    void setMeetingInfo(const MeetingInfo& meetingInfo);
    MeetingInfo getMeetingInfo() const;
    void onAuthSuccess();
    void onAuthFailure() const;
    void onJoinSuccess();
    void onJoinFailure() const;
    void onTextRecognized(const string& text);
    void onChatReceived(const ChatMessage& msg);
    void onMeetingEnd();

    function<void(BotInstance*)> shutdownCallback_;
    void setShutdownCallback(function<void(BotInstance*)> callback);
private:
    string joinUrl_;
    thread botThread_;
    atomic<bool> running_;
    atomic<BotStatus> status_;
    unique_ptr<ZoomClient> zoomClient_;
    MeetingInfo meetingInfo_;
    unique_ptr <AzureSpeechManager> azureSpeechManager_;
    unique_ptr <VirtualAudioInterface> virtualAudioInterface_;
    vector<string> recognizedText_;

    void runBot();
};