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

extern IAuthService* g_authService;
extern IMeetingService* g_meetingService;
extern bool g_isAuthenticated;
