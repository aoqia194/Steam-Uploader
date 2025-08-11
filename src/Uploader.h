#pragma once

// steam API
#include "steam/steam_api.h"

// random tools
#include "Util/ExecCmd.h"
// #include "Util/WarningHook.h"

// enums
#include "Enum/EResult.h"
#include "Enum/EItemUpdateStatus.h"

// other
#include <iostream>
#include <fstream>
#include <thread>


// utility functions
void setAppID(AppId_t appid);
AppId_t getAppID(PublishedFileId_t publishedfileid);


// main uploader class
class Uploader
{
public:
    // INIT
    Uploader(PublishedFileId_t workshopID);

    // METHODS
    void UpdateItem();
    bool UpdateAppID();
    bool InitSteamAPI();
    bool ShutdownSteamAPI();
    void SetInformations(UGCUpdateHandle_t &updateHandle);
    bool CheckProgress(UGCUpdateHandle_t updateHandle);

private:
    // MEMBERS
    PublishedFileId_t m_workshopID;
    AppId_t m_appID;

    // METHODS
    
    // create handle
    UGCUpdateHandle_t CreateUpdateHandle(PublishedFileId_t workshopID);

    // update item informations
    bool SetItemTitle(UGCUpdateHandle_t handle, const char *pchTitle);
    bool SetItemDescription(UGCUpdateHandle_t handle, const char *pchDescription);
    bool SetItemContent(UGCUpdateHandle_t handle, const char *pchContent);
    bool SetItemPreview(UGCUpdateHandle_t handle, const char *pchPreview);
    bool SetItemVisibility(UGCUpdateHandle_t handle, ERemoteStoragePublishedFileVisibility eVisibility);

    // submit changes
    void SubmitItemUpdate(UGCUpdateHandle_t updateHandle, const char* pchContent);

    // CALLBACKS
    void OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* eResult, bool needUserAgreement);
    CCallResult<Uploader, SubmitItemUpdateResult_t> m_callResultSubmit;
};