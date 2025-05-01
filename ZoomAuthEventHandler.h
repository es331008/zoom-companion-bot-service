#pragma once

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

using namespace ZOOM_SDK_NAMESPACE;
using namespace std;

class ZoomAuthEventHandler : public IAuthServiceEvent {
public:
    ZoomAuthEventHandler();
    ~ZoomAuthEventHandler();

    void onAuthenticationReturn(AuthResult result) override;
    void onLoginReturnWithReason(LOGINSTATUS, IAccountInfo*, LoginFailReason) override;
    void onLogout() override;
    void onZoomIdentityExpired() override;
    void onZoomAuthIdentityExpired() override;
    void onNotificationServiceStatus(SDKNotificationServiceStatus, SDKNotificationServiceError) override;
};

