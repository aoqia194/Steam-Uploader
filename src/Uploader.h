#pragma once
#include <iostream>
#include "steam/steam_api.h"
class Uploader
{
public:
    UGCUpdateHandle_t UploadItem(PublishedFileId_t workshopID, const std::string& description);
    bool CheckProgress(UGCUpdateHandle_t updateHandle);

private:
    void OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* pResult, bool bIOFailure);
    CCallResult<Uploader, SubmitItemUpdateResult_t> m_callResultSubmit;
};