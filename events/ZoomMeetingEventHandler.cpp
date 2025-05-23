//#include "ZoomMeetingEventHandler.h"
//
//ZoomMeetingEventHandler::ZoomMeetingEventHandler() {}
//ZoomMeetingEventHandler::~ZoomMeetingEventHandler() {}
//
//void ZoomMeetingEventHandler::onMeetingStatusChanged(MeetingStatus status, int iResult) {
//    if (status == MEETING_STATUS_INMEETING) {
//        m_onMeetingJoin();
//    }
//
//    // Should really be looking at the meeting end code
//    if (status == MEETING_STATUS_ENDED) {
//        // There's a better way to do all this
//
//        //Get the zoom instance
//        auto* zoom = &Zoom::getInstance();
//
//        string concatOutput;
//        for (const auto& line : recognizedText) {
//            if (!line.empty()) {
//                concatOutput += line + "\n";
//            }
//        }
//
//        cout << concatOutput << endl;
//
//        Client httpClient("localhost", 3000);
//        json body = json{ {"voiceTranscript", concatOutput}, {"chatTranscript", zoom->meetingInfo.chatHistory} };
//
//        auto response = httpClient.Post("/api/zoom/summarize", body.dump(), "application/json");
//        if (response && response->status == 200) {
//            json responseJson = json::parse(response->body);
//            cout << responseJson << endl;
//            if (responseJson.contains("summary") && responseJson["summary"].is_string()) {
//                zoom->meetingInfo.meetingSummary = responseJson["summary"];;
//            }
//        }
//        else {
//            cerr << "[Summarize] Request failed with status : " << response->status << endl;
//        }
//
//        json j = zoom->meetingInfo;
//        cout << j.dump(4) << endl;
//        time_t epochTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
//        ofstream file("out/" + zoom->meetingInfo.meetingNumber + "_" + to_string(epochTime) + ".json");
//        if (file.is_open()) {
//            file << j.dump(4); // Pretty print with 4-space indent
//            file.close();
//            Log::info("Meeting data written to file");
//        }
//        else {
//            cerr << "Failed to open file.\n";
//        }
//    }
//}
//
//void ZoomMeetingEventHandler::onMeetingStatisticsWarningNotification(StatisticsWarningType type) {}
//void ZoomMeetingEventHandler::onMeetingParameterNotification(const MeetingParameter* meeting_param) {}
//void ZoomMeetingEventHandler::onSuspendParticipantsActivities() {}
//void ZoomMeetingEventHandler::onAICompanionActiveChangeNotice(bool bActive) {}
//void ZoomMeetingEventHandler::onMeetingTopicChanged(const zchar_t* sTopic) {}
//void ZoomMeetingEventHandler::onMeetingFullToWatchLiveStream(const zchar_t* sLiveStreamUrl) {}
//
//void ZoomMeetingEventHandler::setOnMeetingJoin(const function<void()>& callback) {
//    m_onMeetingJoin = callback;
//}