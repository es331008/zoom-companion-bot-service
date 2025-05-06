#ifndef ZOOMAUTHEVENTHANDLER_H
#define ZOOMAUTHEVENTHANDLER_H

// This shouldn't be needed but for whatever reason, it'll try to build with Unix if it's not included here
#include "httplib.h"

#include "auth_service_interface.h"
#include "../util/Log.h"

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

#endif // ZOOMAUTHEVENTHANDLER_H

