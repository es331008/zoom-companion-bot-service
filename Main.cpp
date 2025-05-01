#include "httplib.h"
#include "zoom_sdk.h"
#include "zoom_sdk_def.h"
#include "json.hpp"
#include "meeting_service_interface.h"
#include "auth_service_interface.h"
#include <string>
#include <locale>
#include <codecvt>
#include <Windows.h>

#include "ZoomAuthEventHandler.h"
#include "Globals.h"

using json = nlohmann::json;
using namespace ZOOM_SDK_NAMESPACE;
using namespace httplib;
using namespace std;

static IAuthServiceEvent* g_authEventHandler;

static bool initZoomSDK() {
    InitParam initParam;
    initParam.strWebDomain = L"https://zoom.us";
    initParam.enableLogByDefault = true;

    printf("[Init] Starting initialization of Zoom SDK...\n");

    SDKError err = InitSDK(initParam);

    if (err != SDKERR_SUCCESS) {
        printf("[Init] InitSDK failed with error: %d\n", err);
        return false;
    }

    printf("[Init] Zoom SDK initialized successfully!\n");
    return true;
}

static wstring getAuthJwt() {
    Client httpClient("localhost", 5000);
    auto response = httpClient.Get("/api/generate-sdk-jwt");
    if (response && response->status == 200) {
        json responseJson = json::parse(response->body);
        if (responseJson.contains("jwt") && responseJson["jwt"].is_string()) {
            string jwtStr = responseJson["jwt"];
            wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(jwtStr);
        }
        else {
            cerr << "[Auth] Missing or invalid 'jwt' field in response." << endl;
            return {};
        }
    }
    else {
        cerr << "[Auth] Request failed with status : " << response->status << endl;
    }
}

static bool authZoomSDK(const wstring& jwt_token) {
    SDKError authServiceInitReturnVal = CreateAuthService(&g_authService);
    if (authServiceInitReturnVal == SDKError::SDKERR_SUCCESS) {
        cout << "[Auth] Auth service created " << endl;
    }
    else {
        cerr << "[Auth] Failed to create auth service: " << authServiceInitReturnVal << endl;
        return false;
    }

    g_authEventHandler = new ZoomAuthEventHandler();
    g_authService->SetEvent(g_authEventHandler);

    AuthContext authContext;
    authContext.jwt_token = jwt_token.c_str();
    if (g_authService) {
        SDKError err = g_authService->SDKAuth(authContext);
        if (err != SDKERR_SUCCESS) {
            printf("[Auth] SDKAuth failed with error: %d\n", err);
            return false;
        }

        printf("[Auth] Zoom SDK authentication started\n");
    }
    return true;
}

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
    svr.Post("/api/join_meeting", [](const Request& req, Response& res) {
        if (!g_isAuthenticated || !g_meetingService) {
            cerr << "[Mtng] Zoom SDK not fully ready" << endl;
            res.status = 503;
            res.set_content("Zoom SDK not ready yet", "text/plain");
            return;
        }

        try {
            json requestBody = json::parse(req.body);

            if (requestBody.find("meetingId") == requestBody.end()) {
                res.status = 400;
                res.set_content("Error: Missing 'meetingId' in request body", "text/plain");
                return;
            }

            int64_t meetingId = requestBody["meetingId"];

            cout << "[Mtng] Attempting to join meeting: " << meetingId << endl;
        
            // Do this is another thread with unique meeting service
            tagJoinParam joinParam;
            joinParam.userType = SDK_UT_WITHOUT_LOGIN;
            
            JoinParam4WithoutLogin& withoutloginParam = joinParam.param.withoutloginuserJoin;
            withoutloginParam.meetingNumber = meetingId;
            withoutloginParam.userName = L"Botley";
            withoutloginParam.psw = L"220464";

            SDKError joinMeetingCallReturnValue = g_meetingService->Join(joinParam);
            if (joinMeetingCallReturnValue == SDKError::SDKERR_SUCCESS)
            {
                cout << "[Mtng] Join meeting call succeeded" << endl;
            }
            else {
                cout << "[Mtng] Join meeting call failed with error: " << joinMeetingCallReturnValue << endl;
            }

            res.status = 200;
            res.set_content("Joining meeting!", "text/plain");
        }
        catch (const exception& ex) {
            res.status = 500;
            res.set_content(string("Error: ") + ex.what(), "text/plain");
        }
        });
}

static void runMessagePump() {
    cout << "[MPmp] Starting message pump" << endl;

    MSG msg;
    while (true) {
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main()
{
    if (!initZoomSDK()) {
        return -1;
    }

    wstring jwt = getAuthJwt();
    if (!authZoomSDK(jwt)) {
        return -1;
    }

    // Message pumps runs on the main thread
    thread serverThread([]() {
        Server svr;
        addCORS(svr);
        configureRoutes(svr);

        this_thread::sleep_for(chrono::seconds(2));

        printf("[Srvr] Starting API server on port 5002...\n");
        svr.listen("0.0.0.0", 5002);
        });
    serverThread.detach();

    // Zoom uses Windows Message Loop, so we need to pump it constantly on the main thread or else we don't receive events
    runMessagePump();

    return 0;
}
