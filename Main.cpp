#include <csignal>

#include "Zoom.h"
#include "AzureSpeechManager.h"

#include "httplib.h"
#include "zoom_sdk.h"
#include "zoom_sdk_def.h"

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

            Log::info("Attempting to join meeting: " + joinUrl);
        
            auto* zoom = &Zoom::getInstance();
            zoom->join(joinUrl);
            
            res.status = 200;
            res.set_content("Joined meeting!", "text/plain");
        }
        catch (const exception& ex) {
            res.status = 500;
            res.set_content(string("Error: ") + ex.what(), "text/plain");
        }
        });
}

static void runMessagePump() {
    Log::info("Starting message pump");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Log::info("Message pump ended");
}

static void onExit() {
    auto* zoom = &Zoom::getInstance();
    zoom->leave();
    zoom->clean();
    AzureSpeechManager::getInstance().shutdown();

    Log::info("Exiting...");
}

static void onSignal(int signal) {
    onExit();
    _Exit(signal);
}

static SDKError run() {
    SDKError err{ SDKERR_SUCCESS };
    auto* zoom = &Zoom::getInstance();

    signal(SIGINT, onSignal);
    signal(SIGTERM, onSignal);

    atexit(onExit);

    err = zoom->init();
    if (Zoom::hasError(err, "SDK initialization complete"))
        return err;

    err = zoom->auth();
    if (Zoom::hasError(err, "SDK authentication started"))
        return err;

    return err;
}

int main()
{
    SDKError err = run();

    if (Zoom::hasError(err))
        Log::error("error");

    AzureSpeechManager::getInstance().initialize("9dOvrKeS2L1GhSCfcTrGlfrfNYn1bTg3ymEBPtM6e1jreJiRVw9jJQQJ99BEACYeBjFXJ3w3AAAYACOGh7to", "eastus");

    // Message pump runs on the main thread
    thread serverThread([]() {
        Server svr;
        addCORS(svr);
        configureRoutes(svr);

        this_thread::sleep_for(chrono::seconds(2));

        Log::info("Starting API server on port 5002...");
        svr.listen("0.0.0.0", 5002);
        });
    serverThread.detach();

    // Zoom uses Windows Message Loop, so we need to pump it constantly on the main thread or else we don't receive events
    runMessagePump();

    return 0;
}
