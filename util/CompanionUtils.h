#ifndef COMPANION_UTILS_H
#define COMPANION_UTILS_H

#include <string>
#include <locale>
#include <codecvt>
#include <chrono>
#include <regex>

using namespace std;

class CompanionUtils {
    typedef wchar_t zchar_t;

public:
    CompanionUtils() = delete;

    static float clamp(float v, float lo, float hi);
    static string zcharToString(const zchar_t* zstr);
    static bool parseJoinUrl(const std::string& joinUrl, std::string& meetingId, std::string& password);
    static string timeToString(time_t t);
};

#endif // COMPANION_UTILS_H