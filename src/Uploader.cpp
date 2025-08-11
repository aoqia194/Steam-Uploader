#include "Uploader.h"








// Uploader class implementation

// init
Uploader::Uploader(PublishedFileId_t workshopID, AppId_t appID) {
    this->m_workshopID = workshopID;
    // AppId_t appID = getAppID(workshopID);
    this->m_appID = appID;
}

// main function
void Uploader::UpdateItem(string descriptionPath, string previewPath, string contentPath, string title, ERemoteStoragePublishedFileVisibility visibility) {
    // update AppID and init Steam API
    if (!this->UpdateAppID() || !this->InitSteamAPI()) {
        return;
    }

    // create handle
    UGCUpdateHandle_t updateHandle = CreateUpdateHandle(this->m_workshopID);

    // TODO: verify that the provided description respects the limitations. See doc on the Steam API function to set the description for a handle which gives the value to check.
    // handle description
    if (!descriptionPath.empty()) {
        if (!exists(descriptionPath)) {
            std::cerr << "Invalid description path: " << descriptionPath << ". Parameter must be a valid file.\n";
        } else {
            string description = readTxtFile(descriptionPath);
            SetItemDescription(updateHandle, description.c_str());
        }
    }

    // TODO: verify preview respects the limitations
    // handle preview
    if (!previewPath.empty()) {
        if (!exists(previewPath) || !is_regular_file(previewPath)) {
            std::cerr << "Invalid preview path: " << previewPath << ". Parameter must be a valid file.\n";
        } else {
            SetItemPreview(updateHandle, previewPath.c_str());
        }
    }

    // handle content
    if (!contentPath.empty()) {
        if (!exists(contentPath) || !is_directory(contentPath)) {
            std::cerr << "Invalid content path: " << contentPath << ". Parameter must be a valid folder.\n";
        } else {
            SetItemContent(updateHandle, contentPath.c_str());
        }
    }

    // TODO: verify title respects the limitations, if there is any ?
    // handle title
    if (!title.empty()) {
        SetItemTitle(updateHandle, title.c_str());
    }

    // TODO: verify the provided value is 0 to 3, other values might break ?
    // handle visibility
    if (visibility != static_cast<ERemoteStoragePublishedFileVisibility>(-1)) {
        SetItemVisibility(updateHandle, visibility);
    }

    // SetInformations(updateHandle);

    const char* pchContent = "Test upload"; // temp
    SubmitItemUpdate(updateHandle, pchContent);

    // Use a default value that is not part of the enum by casting an invalid integer
    EItemUpdateStatus previousUpdateStatus = k_EItemUpdateStatusInvalid;
    while (true) {
        bool isValid = CheckProgress(updateHandle, &previousUpdateStatus);

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
bool Uploader::SetItemTitle(UGCUpdateHandle_t handle, string pchTitle) {
    return SteamUGC()->SetItemTitle(handle, pchTitle.c_str());
}

bool Uploader::SetItemDescription(UGCUpdateHandle_t handle, string pchDescription) {
    return SteamUGC()->SetItemDescription(handle, pchDescription.c_str());
}

bool Uploader::SetItemContent(UGCUpdateHandle_t handle, string pchContent) {
    return SteamUGC()->SetItemContent(handle, pchContent.c_str());
}

bool Uploader::SetItemPreview(UGCUpdateHandle_t handle, string pchPreview) {
    return SteamUGC()->SetItemPreview(handle, pchPreview.c_str());
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
bool Uploader::CheckProgress(UGCUpdateHandle_t updateHandle, EItemUpdateStatus* previousUpdateStatus)
{
    uint64 bytesProcessed = 0;
    uint64 bytes = 0;
    EItemUpdateStatus eItemUpdateStatus = SteamUGC()->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytes);

    bool isValid = eItemUpdateStatus != k_EItemUpdateStatusInvalid;

    if (isValid && eItemUpdateStatus != *previousUpdateStatus) {
        std::cout << "Update status: " << GetEItemUpdateStatusDescription(eItemUpdateStatus) << " (" << GetEItemUpdateStatusName(eItemUpdateStatus) << ")\n";

        *previousUpdateStatus = eItemUpdateStatus;
    }

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