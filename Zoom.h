#ifndef ZOOM_H
#define ZOOM_H

#include <string>
#include <locale>
#include <codecvt>
#include <chrono>

#include "util/Singleton.h"
#include "util/Log.h"

#include "httplib.h"
#include "zoom_sdk.h"
#include "./events/ZoomAuthEventHandler.h"
#include "./events/ZoomMeetingEventHandler.h"

#include "zoom_sdk_def.h"
#include "json.hpp"
#include "meeting_service_interface.h"
#include "meeting_service_components/meeting_chat_interface.h"
#include "auth_service_interface.h"

using namespace std;
using namespace ZOOMSDK;
using namespace httplib;
using json = nlohmann::json;

typedef chrono::time_point<chrono::system_clock> time_point;

class Zoom : public Singleton<Zoom> {
    friend class Singleton<Zoom>;

    string m_jwt;

    time_point m_iat;
    time_point m_exp;

    IAuthService* m_authService;
    IMeetingService* m_meetingService;

    SDKError createServices();
    void getAuthJwt();

    function<void()> onJoin = [&]() {
        Log::success("Joined meeting!");
        sendWelcomeChat();
    };

public:
    Zoom();
    SDKError init();
    SDKError auth();

    SDKError join(string joinUrl);
    SDKError leave();

    SDKError sendWelcomeChat();

    SDKError clean();

    static bool hasError(SDKError e, const string& action = "");
};

#endif //ZOOM_H