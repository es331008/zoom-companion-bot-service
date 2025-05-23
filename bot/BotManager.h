#pragma once

#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>
#include "BotInstance.h"

#include "../util/Singleton.h"

using namespace std;

class BotManager: public Singleton<BotManager> {
    friend class Singleton<BotManager>;

public:
    string startBot(const string& joinUrl);
    void stopBot(const string& botId);
    void stopAllBots();
    BotStatus getBotStatus(const string& botId);

private:
    BotManager() = default;

    mutex mutex_;
    unordered_map<string, unique_ptr<BotInstance>> bots_;
};