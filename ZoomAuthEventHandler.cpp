#include "ZoomAuthEventHandler.h"
#include "ZoomMeetingEventHandler.h"
#include "Globals.h"

static IMeetingServiceEvent* g_meetingEventHandler;

ZoomAuthEventHandler::ZoomAuthEventHandler() {}
ZoomAuthEventHandler::~ZoomAuthEventHandler() {}

void ZoomAuthEventHandler::onAuthenticationReturn(AuthResult result) {
    if (result == AuthResult::AUTHRET_SUCCESS) {
        cout << "[Auth] Zoom SDK authentication successful." << endl;
        g_isAuthenticated = true;

        SDKError meetingServiceInitReturnVal = CreateMeetingService(&g_meetingService);
        g_meetingEventHandler = new ZoomMeetingEventHandler();
        g_meetingService->SetEvent(g_meetingEventHandler);
        if (meetingServiceInitReturnVal == SDKError::SDKERR_SUCCESS) {
            cout << "[Mtng] Meeting service created " << endl;
        }
        else {
            cerr << "[Mtng] Failed to create auth service: " << meetingServiceInitReturnVal << endl;
        }
    }
    else {
        cerr << "[Auth] Authentication failed. Result: " << result << endl;
    }
}
void ZoomAuthEventHandler::onLoginReturnWithReason(LOGINSTATUS, IAccountInfo*, LoginFailReason) {}
void ZoomAuthEventHandler::onLogout() {}
void ZoomAuthEventHandler::onZoomIdentityExpired() {}
void ZoomAuthEventHandler::onZoomAuthIdentityExpired() {}
void ZoomAuthEventHandler::onNotificationServiceStatus(SDKNotificationServiceStatus, SDKNotificationServiceError) {}