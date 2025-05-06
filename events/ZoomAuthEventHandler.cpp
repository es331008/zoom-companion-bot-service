#include "ZoomAuthEventHandler.h"

ZoomAuthEventHandler::ZoomAuthEventHandler() {}
ZoomAuthEventHandler::~ZoomAuthEventHandler() {}

void ZoomAuthEventHandler::onAuthenticationReturn(AuthResult result) {
    if (result == AuthResult::AUTHRET_SUCCESS) {
        Log::success("Zoom authentication successful");
    }
    else {
        Log::error("Zoom authentication failed");
    }
}
void ZoomAuthEventHandler::onLoginReturnWithReason(LOGINSTATUS, IAccountInfo*, LoginFailReason) {}
void ZoomAuthEventHandler::onLogout() {}
void ZoomAuthEventHandler::onZoomIdentityExpired() {}
void ZoomAuthEventHandler::onZoomAuthIdentityExpired() {}
void ZoomAuthEventHandler::onNotificationServiceStatus(SDKNotificationServiceStatus, SDKNotificationServiceError) {}