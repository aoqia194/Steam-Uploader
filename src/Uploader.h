#pragma once

// steam API
#include "steam/steam_api.h"

// random tools
// #include "Util/ExecCmd.h"
// #include "Util/WarningHook.h"
#include "Util/ReadFile.h"
#include "Util/AppID.h"

// enums
#include "Enum/EResult.h"
#include "Enum/EItemUpdateStatus.h"

// other
#include <iostream>
#include <thread>
#include <string>
using std::string;
#include <filesystem>
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::file_size;
#include <vector>


// main uploader class
class Uploader
{
public:
    // INIT
    Uploader(PublishedFileId_t workshopID, AppId_t appID);

    // METHODS
    // main call
    void UpdateItem(string description, string preview, string content, string title, ERemoteStoragePublishedFileVisibility visibility, string patchNotePath, string tags);


    bool UpdateAppID();
    bool InitSteamAPI();
    bool ShutdownSteamAPI();
    bool CheckProgress(UGCUpdateHandle_t updateHandle, EItemUpdateStatus* previousUpdateStatus);

private:
    // MEMBERS
    PublishedFileId_t m_workshopID;
    AppId_t m_appID;

    // METHODS
    
    // create handle
    UGCUpdateHandle_t CreateUpdateHandle(PublishedFileId_t workshopID);

    // update item informations
    bool SetItemTitle(UGCUpdateHandle_t handle, string pchTitle);
    bool SetItemDescription(UGCUpdateHandle_t handle, string pchDescription);
    bool SetItemContent(UGCUpdateHandle_t handle, string pchContent);
    bool SetItemPreview(UGCUpdateHandle_t handle, string pchPreview);
    bool SetItemVisibility(UGCUpdateHandle_t handle, ERemoteStoragePublishedFileVisibility eVisibility);
    bool SetTags(UGCUpdateHandle_t handle, const SteamParamStringArray_t* pchTags);

    // submit changes
    void SubmitItemUpdate(UGCUpdateHandle_t updateHandle, string pchContent);

    // CALLBACKS
    void OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* eResult, bool needUserAgreement);
    CCallResult<Uploader, SubmitItemUpdateResult_t> m_callResultSubmit;
};