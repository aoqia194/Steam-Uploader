#include "Uploader.h"


// FIX: Using the helper function here causes a "Segmentation fault"
SteamParamStringArray_t stringToSteamArray(const string& tags) {
    // Split tags by comma
    std::vector<std::string> tagList;
    std::stringstream ss(tags);
    std::string tag;
    while (std::getline(ss, tag, ',')) {
        // Trim whitespace before and after the tag (not in between)
        tag.erase(0, tag.find_first_not_of(" \t\n\r"));
        tag.erase(tag.find_last_not_of(" \t\n\r") + 1);
        if (!tag.empty()) tagList.push_back(tag);
    }

    // Prepare array of C strings
    std::vector<const char*> tagCStrs;
    for (const auto& t : tagList) tagCStrs.push_back(t.c_str());

    // transform to Steam string array
    // https://partner.steamgames.com/doc/api/ISteamRemoteStorage#SteamParamStringArray_t
    SteamParamStringArray_t tagArray;
    tagArray.m_ppStrings = tagCStrs.data();
    tagArray.m_nNumStrings = static_cast<int>(tagCStrs.size());

    return tagArray;
}


// Uploader class implementation

// init
Uploader::Uploader(PublishedFileId_t workshopID, AppId_t appID) {
    this->m_workshopID = workshopID;
    // AppId_t appID = getAppID(workshopID);
    this->m_appID = appID;
}

// main function
void Uploader::UpdateItem(string descriptionPath, string previewPath, string contentPath, string title, ERemoteStoragePublishedFileVisibility visibility, string patchNotePath, string tags) {
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
            // verify description size respects the limits
            if (description.size() > k_cchPublishedDocumentDescriptionMax) {
                std::cerr << "Error: Description exceeds maximum length of " << k_cchPublishedDocumentDescriptionMax << " characters. (Current: " << description.size() << ")\n";
            } else {
                bool success = SetItemDescription(updateHandle, description.c_str());
                if (!success) {
                    std::cerr << "Error: Failed to set item description.\n";
                }
            }
        }
    }

    // handle preview
    if (!previewPath.empty()) {
        if (!exists(previewPath) || !is_regular_file(previewPath)) {
            std::cerr << "Error: Invalid preview path (" << previewPath << "). Parameter must be a valid file.\n";
        } else if (file_size(previewPath) > 1048576) {
            std::cerr << "Error: Preview file is too large (" << file_size(previewPath) << " bytes). Maximum allowed is 1 MB.\n";
        } else {
            bool success = SetItemPreview(updateHandle, previewPath.c_str());
            if (!success) {
                std::cerr << "Error: Failed to set item preview. Suggested formats include JPG, PNG and GIF.\n";
            }
        }
    }

    // handle content
    if (!contentPath.empty()) {
        if (!exists(contentPath) || !is_directory(contentPath)) {
            std::cerr << "Invalid content path: " << contentPath << ". Parameter must be a valid folder.\n";
        } else {
            bool success = SetItemContent(updateHandle, contentPath.c_str());
            if (!success) {
                std::cerr << "Error: Failed to set item content.\n";
            }
        }
    }

    // handle title
    if (!title.empty()) {
        if (title.size() > k_cchPublishedDocumentTitleMax) {
            std::cerr << "Error: Title exceeds maximum length of " << k_cchPublishedDocumentTitleMax << " characters. (Current: " << title.size() << ")\n";
        } else {
            SetItemTitle(updateHandle, title.c_str());
        }
    }

    // handle visibility
    if (visibility != static_cast<ERemoteStoragePublishedFileVisibility>(-1)) {
        // verify the given visibility value is valid
        if (visibility >= 0 && visibility <= 3) {
            SetItemVisibility(updateHandle, visibility);
        } else {
            std::cerr << "Error: Invalid visibility value (" << visibility << "). Must be between 0 and 3.\n";
        }
    }
    
    // handle tags
    if (tags != "$EMPTY") {
        // inline stringToSteamArray due to a segmentation fault if it isn't
        // Split tags by comma
        std::vector<std::string> tagList;
        std::stringstream ss(tags);
        std::string tag;
        while (std::getline(ss, tag, ',')) {
            tag.erase(0, tag.find_first_not_of(" \t\n\r"));
            tag.erase(tag.find_last_not_of(" \t\n\r") + 1);
            if (!tag.empty()) tagList.push_back(tag);
        }

        // Prepare array of C strings
        std::vector<const char*> tagCStrs;
        for (const auto& t : tagList) tagCStrs.push_back(t.c_str());

        SteamParamStringArray_t tagArray;
        tagArray.m_ppStrings = tagCStrs.empty() ? nullptr : tagCStrs.data();
        tagArray.m_nNumStrings = static_cast<int>(tagCStrs.size());

        bool success = SetTags(updateHandle, &tagArray);
        if (!success) {
            std::cerr << "Error: Failed to set item tags.\n";
        }
    }


    // handle patch note
    string patchNote = ""; // default
    if (!patchNotePath.empty()) {
        if (!exists(patchNotePath)) {
            std::cerr << "Invalid patch note path: " << patchNotePath << ". Parameter must be a valid file.\n";
        } else {
            patchNote = readTxtFile(patchNotePath);
        }
    }



    // push update
    SubmitItemUpdate(updateHandle, patchNote);

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

bool Uploader::SetTags(UGCUpdateHandle_t handle, const SteamParamStringArray_t* pchTags) {
    return SteamUGC()->SetItemTags(handle, pchTags);
}

// send the update
void Uploader::SubmitItemUpdate(UGCUpdateHandle_t updateHandle, string pchContent) {
    // https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdate
    SteamAPICall_t apiCall = SteamUGC()->SubmitItemUpdate(updateHandle, pchContent.c_str());
    m_callResultSubmit.Set(apiCall, this, &Uploader::OnSubmitItemUpdateResult);
}

// used to check the current upload status
bool Uploader::CheckProgress(UGCUpdateHandle_t updateHandle, EItemUpdateStatus* previousUpdateStatus)
{
    uint64 bytesProcessed = 0;
    uint64 bytes = 0;
    EItemUpdateStatus eItemUpdateStatus = SteamUGC()->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytes);

    bool isValid = eItemUpdateStatus != k_EItemUpdateStatusInvalid;

    if (isValid && eItemUpdateStatus != *previousUpdateStatus) {
        std::cout << "Update status: " << GetEItemUpdateStatusDescription(eItemUpdateStatus) << "\n";

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