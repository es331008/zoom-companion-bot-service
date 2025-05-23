#include <csignal>

#include "./bot/BotManager.h"
#include "httplib.h"

// TODO: Update logging to act more like spring sleuth
// TODO: Dump meeting info on meeting end (disconnect)
// TODO: Save meeting info
// TODO: Convert to use CMAKE and vcpkg
// TODO: Add unit tests

static void addCORS(Server& svr) {
    svr.Options(R"(.*)", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Access-Control-Max-Age", "86400");
        res.status = 204; // No Content
    });

    svr.set_pre_routing_handler([](const Request& req, Response& res) {
        if (req.method != "OPTIONS") {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        }
        return Server::HandlerResponse::Unhandled;
    });
}

static void configureRoutes(Server& svr) {
    svr.Post("/api/bot/join-meeting", [](const Request& req, Response& res) {
        try {
            json requestBody = json::parse(req.body);

            if (requestBody.find("joinUrl") == requestBody.end()) {
                res.status = 400;
                res.set_content("Error: Missing 'joinUrl' in request body", "text/plain");
                return;
            }

            string joinUrl = requestBody["joinUrl"];

            auto& botManager = BotManager::getInstance();
            botManager.startBot(joinUrl);
            
            res.status = 200;
            res.set_content("Joined meeting!", "text/plain");
        }
        catch (const exception& ex) {
            res.status = 500;
            res.set_content(string("Error: ") + ex.what(), "text/plain");
        }
        });
}

static void onExit() {
    // Any cleanup logic goes here

    Log::info("Exiting...");
}

static void onSignal(int signal) {
    onExit();
    _Exit(signal);
}

int main()
{
    Server svr;
    addCORS(svr);
    configureRoutes(svr);

    this_thread::sleep_for(chrono::seconds(2));

    Log::info("Starting bot server on port 5002...");
    svr.listen("0.0.0.0", 5002);

    return 0;
}
