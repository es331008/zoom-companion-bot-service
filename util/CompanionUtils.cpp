#include "CompanionUtils.h"

float CompanionUtils::clamp(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

string CompanionUtils::zcharToString(const zchar_t* zstr) {
    if (!zstr) return "";

    wstring wstr = wstring(reinterpret_cast<const wchar_t*>(zstr));
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

bool CompanionUtils::parseJoinUrl(const string& joinUrl, string& meetingId, string& password) {
    regex urlRegex(R"(https://[\w.-]+/j/(\d+)\?pwd=([\w-]+))");
    smatch match;

    if (regex_search(joinUrl, match, urlRegex) && match.size() == 3) {
        meetingId = match[1].str();
        password = match[2].str();
        return true;
    }
    return false;
}

string CompanionUtils::timeToString(time_t t) {
    return to_string(t);
}