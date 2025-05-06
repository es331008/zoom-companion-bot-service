#include "MeetingRecordingCtrlEventHandler.h"

void MeetingRecordingCtrlEventHandler::onRecordPrivilegeChanged(bool bCanRec) {
    if (m_onRecordingPrivilegeChanged)
        m_onRecordingPrivilegeChanged(bCanRec);
}