#include "BotManager.h"

string BotManager::startBot(const string& joinUrl) {
    lock_guard<mutex> lock(mutex_);
    string meetingId, password;

    if (CompanionUtils::parseJoinUrl(joinUrl, meetingId, password)) {
        string botId(meetingId);

        Log::info("Starting bot_" + meetingId);
        auto bot = make_unique<BotInstance>(joinUrl);
        bot->setShutdownCallback([this, botId](BotInstance* instance) {
            thread([this, botId]() {
                this->stopBot(botId);
                }).detach();
            });

        botThreads_[botId] = thread([botPtr = bot.get()]() {
            botPtr->start();
        });

        bots_[botId] = move(bot);

        return botId;
    }
    else {
        throw runtime_error("Invalid joinUrl");
    }
}

void BotManager::stopBot(const string& botId) {
    unique_ptr<BotInstance> bot;
    thread botThread;

    Log::info("Stopping bot_" + botId);

    {
        lock_guard<mutex> lock(mutex_);
        if (bots_.count(botId)) {
            bot = move(bots_[botId]);
            bots_.erase(botId);

            botThread = move(botThreads_[botId]);
            botThreads_.erase(botId);
        }
        else {
            return;
        }
    }

    if (bot) {
        bot->stop();
    }

    if (botThread.joinable() && botThread.get_id() != this_thread::get_id()) {
        botThread.join();
    }
}

void BotManager::stopAllBots() {
    lock_guard<mutex> lock(mutex_);
    for (auto& pair : bots_) {
        pair.second->stop();
    }
    bots_.clear();
}

BotStatus BotManager::getBotStatus(const string& botId) {
    lock_guard<mutex> lock(mutex_);
    if (bots_.count(botId)) {
        return bots_[botId]->getStatus();
    }
    return BotStatus::NotStarted;
}