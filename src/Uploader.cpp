#include "Uploader.h"

// source: https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
void setAppID(AppId_t appid) {
	printf("Writing appid to steam_appid.txt\n");
	std::ofstream myfile;
	myfile.open("steam_appid.txt");
	myfile << appid;
	myfile.close();
}

// source: https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
#ifdef _WIN32
AppId_t getAppID(PublishedFileId_t publishedfileid) {
    std::wstring str(L"powershell -c \"[regex]::Matches((Invoke-WebRequest -Uri \\\"https://steamcommunity.com/sharedfiles/filedetails/?id=");
    str += std::to_wstring(publishedfileid);
    str += std::wstring(L"\\\" ).Content, 'data-appid=\\\"(\\d+?)\\\">').Groups[1].Value\"");
    auto out = ExecCmd(str.c_str());
    return strtoul(out, NULL, 10);
}
#else
AppId_t getAppID(PublishedFileId_t publishedfileid) {
    std::string str = "curl -s 'https://steamcommunity.com/sharedfiles/filedetails/?id=";
    str += std::to_string(publishedfileid);
    str += "' | grep -o 'data-appid=\"[0-9]*' | grep -o '[0-9]*'";
    std::string out = ExecCmd(str.c_str());
    return strtoul(out.c_str(), NULL, 10);
}
#endif






// Uploader class implementation

Uploader::Uploader(PublishedFileId_t workshopID) {
    this->m_workshopID = workshopID;

    AppId_t appID = getAppID(workshopID);
    this->m_appID = appID;
}

// main function
void Uploader::UpdateItem() {
    // update AppID and init Steam API
    if (!this->UpdateAppID() || !this->InitSteamAPI()) {
        return;
    }

    UGCUpdateHandle_t updateHandle = CreateUpdateHandle(this->m_workshopID);

    SetInformations(updateHandle);

    const char* pchContent = "Test upload"; // temp
    SubmitItemUpdate(updateHandle, pchContent);

    while (true) {
        bool isValid = CheckProgress(updateHandle);

        SteamAPI_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (!isValid) {
            break;
        }
    }

    ShutdownSteamAPI();
}

// update AppID by verifying if it's good first
bool Uploader::UpdateAppID() {
    AppId_t appID = this->m_appID;
    if (appID == 0) {
        std::cerr << "Failed to retrieve appID for workshop item: " << this->m_workshopID << "\n";
        return false;
    }

    // set appID needed for Steam API initialization
    setAppID(appID);
    return true;
}

// initialize Steam API
bool Uploader::InitSteamAPI() {
    bool success = SteamAPI_Init();
    if (!success) {
        std::cerr << "Failed to initialize Steam API for workshopID " << this->m_workshopID << " from appID " << this->m_appID << "!\n";
        return success;
    }

    std::cout << "Steam API initialized for workshopID " << this->m_workshopID << " from appID " << this->m_appID << "!\n";
    // EnableWarningMessageHook();
    return success;
}

// shutdown Steam API
bool Uploader::ShutdownSteamAPI() {
    SteamAPI_Shutdown();
    std::cout << "Steam API shutdown.\n";
    return true;
}





// create handler
UGCUpdateHandle_t Uploader::CreateUpdateHandle(PublishedFileId_t workshopID) {
    return SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), workshopID);
}

// update item informations
bool Uploader::SetItemTitle(UGCUpdateHandle_t handle, const char *pchTitle) {
    return SteamUGC()->SetItemTitle(handle, pchTitle);
}

bool Uploader::SetItemDescription(UGCUpdateHandle_t handle, const char *pchDescription) {
    return SteamUGC()->SetItemDescription(handle, pchDescription);
}

bool Uploader::SetItemContent(UGCUpdateHandle_t handle, const char *pchContent) {
    return SteamUGC()->SetItemContent(handle, pchContent);
}

bool Uploader::SetItemPreview(UGCUpdateHandle_t handle, const char *pchPreview) {
    return SteamUGC()->SetItemPreview(handle, pchPreview);
}

bool Uploader::SetItemVisibility(UGCUpdateHandle_t handle, ERemoteStoragePublishedFileVisibility eVisibility) {
    return SteamUGC()->SetItemVisibility(handle, eVisibility);
}


// send the update
void Uploader::SubmitItemUpdate(UGCUpdateHandle_t updateHandle, const char* pchContent) {
    // https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdate
    SteamAPICall_t apiCall = SteamUGC()->SubmitItemUpdate(updateHandle, pchContent);
    m_callResultSubmit.Set(apiCall, this, &Uploader::OnSubmitItemUpdateResult);
}



// setup various informations
void Uploader::SetInformations(UGCUpdateHandle_t &updateHandle)
{
    // add configs for descriptions etc
    SetItemTitle(updateHandle, "Uploader test 2");
    SetItemDescription(updateHandle, "Test Description");
    SetItemContent(updateHandle, "/home/simon/Documents/Repositories/Steam-Uploader/TestUpload");
    SetItemPreview(updateHandle, "/home/simon/Documents/Repositories/Steam-Uploader/preview.png");
    SetItemVisibility(updateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);
}

// used to check the current upload status
bool Uploader::CheckProgress(UGCUpdateHandle_t updateHandle)
{
    uint64 bytesProcessed = 0;
    uint64 bytes = 0;
    EItemUpdateStatus eItemUpdateStatus = SteamUGC()->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytes);

    bool isValid = eItemUpdateStatus != k_EItemUpdateStatusInvalid;

    if (isValid) {
        std::cout << "Update status: " << GetEItemUpdateStatusDescription(eItemUpdateStatus) << " (" << GetEItemUpdateStatusName(eItemUpdateStatus) << ")\n";
    };
    
    return (eItemUpdateStatus != k_EItemUpdateStatusInvalid);
}



// callback for submit
void Uploader::OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* eResult, bool needUserAgreement)
{
    if (needUserAgreement) {
        std::cerr << "User needs to accept the workshop legal agreement before submitting the item update.\n";
        return;
    }
    if (eResult->m_eResult == k_EResultOK) {
        std::cout << "Item update submitted successfully!\n";
    } else {
        // Enum doc: https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdateResult_t
        std::cerr << "Failed to submit item update. (" << GetEResultName(eResult->m_eResult) << ": " << GetEResultDescription(eResult->m_eResult) << ")\n";
    };
}