#include "BotManager.h"

string BotManager::startBot(const string& joinUrl) {
    lock_guard<mutex> lock(mutex_);
    string meetingId, password;

    if (CompanionUtils::parseJoinUrl(joinUrl, meetingId, password)) {
        string botId(meetingId);

        Log::info("Starting bot_" + meetingId);
        auto bot = make_unique<BotInstance>(joinUrl);
        bot->setShutdownCallback([this, botId](BotInstance* instance) {
            this->stopBot(botId);
        });
        bot->start();
        bots_[botId] = move(bot);

        return botId;
    }
    else {
        throw runtime_error("Invalid joinUrl");
    }
}

void BotManager::stopBot(const string& botId) {
    lock_guard<mutex> lock(mutex_);

    if (bots_.count(botId)) {
        bots_[botId]->stop();
        bots_.erase(botId);
    }
}

void BotManager::stopAllBots() {
    lock_guard<mutex> lock(mutex_);
    for (auto& pair : bots_) {
        pair.second->stop();
    }
    bots_.clear();
}

BotStatus BotManager::getBotStatus(const std::string& botId) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (bots_.count(botId)) {
        return bots_[botId]->getStatus();
    }
    return BotStatus::NotStarted;
}