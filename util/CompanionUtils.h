#pragma once

#include <string>
#include <locale>
#include <codecvt>
#include <chrono>
#include <regex>

#include "httplib.h"
#include "zoom_sdk_def.h"

#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class CompanionUtils {
    typedef wchar_t zchar_t;

public:
    CompanionUtils() = delete;

    static float clamp(float v, float lo, float hi);
    static string zcharToString(const zchar_t* zstr);
    static bool parseJoinUrl(const std::string& joinUrl, std::string& meetingId, std::string& password);
    static string timeToString(time_t t);
    static bool hasSDKError(const SDKError e, const string& onSuccessMsg, const string& onErrorMsg);
};
