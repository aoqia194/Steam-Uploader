#include "Uploader.h"

#include "spdlog/spdlog.h"

// Uploader class implementation

Uploader::Uploader(PublishedFileId_t workshopID, AppId_t appID, bool isNew) {
    // an appID needs to be provided, it's a mandatory parameter
    if (appID == 0) {
        spdlog::error("appID is a required argument.");
        return;
    }

    // set app ID
    this->m_appID = appID;
    setAppID(appID); // set in the file

    // init Steam API
    InitSteamAPI();

    // create new item if needed
    if (isNew) {
        // catch user possibly doing wrong usage if they gave a wID and try to create a new item
        if (workshopID != 0) {
            spdlog::error("Warning: You provided a workshopID {} while creating a new item but this ID will be ignored since a new one is created instead.", workshopID);
        }

        this->m_workshopID = 0; // default value used when creating the item
        CreateWorkshopItem(); // this will set workshopID to the newly created item workshopID

    // workshopID can't be 0 if no new item is being created
    } else if (workshopID == 0) {
        spdlog::error("workshopID is a required argument. Either specify it, or use --new.");
        ShutdownSteamAPI();
        return;

    } else {
        this->m_workshopID = workshopID;
    }

    // for use when setting everything up
    this->m_isNew = isNew;
}

// initialize Steam API
bool Uploader::InitSteamAPI() {
    bool success = SteamAPI_Init();
    if (!success) {
        spdlog::error("Failed to initialize Steam API from appID ({})", this->m_appID);
        return success;
    }

    spdlog::debug("Steam API initialized from appID ({})", this->m_appID);
    // EnableWarningMessageHook();
    return success;
}

// shutdown Steam API
bool Uploader::ShutdownSteamAPI() {
  spdlog::debug("ShutdownSteamAPI called - Shutting down SteamAPI.");
  SteamAPI_Shutdown();
  return true;
}





// TODO: Jvla, you should probably split this up into multiple subfunctions, this function is massive!
int Uploader::UpdateItem(
        // at least one of
        string descriptionPath, string previewPath, string contentPath, string title, ERemoteStoragePublishedFileVisibility visibility, string tags, 
        // optional
        string patchNotePath, string language
    ) {
    // create handle
    UGCUpdateHandle_t updateHandle = CreateUpdateHandle(this->m_workshopID);

    // handle description
    if (!descriptionPath.empty()) {
        if (!exists(descriptionPath)) {
            spdlog::error("Description file ({}) doesn't exist!", descriptionPath);
        } else {
            string description = readTxtFile(descriptionPath);
            // verify description size respects the limits
            if (description.size() > k_cchPublishedDocumentDescriptionMax) {
                spdlog::error("Description with length {} exceeds maximum length of {} characters.", description.size(), k_cchPublishedDocumentDescriptionMax);
            } else {
                bool success = SetItemDescription(updateHandle, description.c_str());
                if (!success) {
                    spdlog::error("Failed to set item description because SetItemDescription returned {}", success);
                }
            }
        }
    }

    // handle preview
    if (!previewPath.empty()) {
        if (!exists(previewPath) || !is_regular_file(previewPath)) {
            spdlog::warn("Preview file ({}) doesn't exist!", previewPath);
        } else if (const auto previewSize = file_size(previewPath) >= 1048576) {
            spdlog::warn("Preview file with size {} exceeds maximum size of 1048576 bytes.", previewSize);
        } else {
            bool success = SetItemPreview(updateHandle, previewPath.c_str());
            if (!success) {
                spdlog::error("Failed to set workshop item preview.");
            }
        }
    }

    // handle content
    if (!contentPath.empty()) {
        if (!exists(contentPath) || !is_directory(contentPath)) {
            spdlog::error("Content file ({}) doesn't exist!", contentPath);
        } else {
            bool success = SetItemContent(updateHandle, contentPath.c_str());
            if (!success) {
                spdlog::error("Failed to set item content.");
            }
        }
    }

    // handle title
    if (!title.empty()) {
        if (title.size() > k_cchPublishedDocumentTitleMax) {
            spdlog::error("Title with length {} exceeds maximum length of {} characters.", k_cchPublishedDocumentTitleMax, title.size());
        } else {
            bool success = SetItemTitle(updateHandle, title.c_str());
            if (!success) {
                spdlog::error("Failed to set item title to ({}).", title);
            }
        }
    }

    // handle visibility
    const auto visibilityIdx = static_cast<int8_t>(visibility);
    if (visibilityIdx != -1) {
        if (visibility >= k_ERemoteStoragePublishedFileVisibilityPublic && visibility <= k_ERemoteStoragePublishedFileVisibilityUnlisted) {
            bool success = SetItemVisibility(updateHandle, visibility);
            if (!success) {
                spdlog::error("Failed to set item visibility to ({}).", visibilityIdx);
            }
        } else {
            spdlog::error("Invalid visibility with value ({}). Must be between 0 and 3!", visibilityIdx);
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
            spdlog::error("Failed to set item tags to {}.", tags);
        }
    }



    // handle language
    if (language != "english") {
        // verify it's a valid language for Steam
        if (!IsValidSteamLanguageCode(language)) {
            spdlog::error("Invalid language code {}.", language);
        } else {
            bool success = SetUploadLanguage(updateHandle, language);
            if (!success) {
                spdlog::error("Failed to set upload language to {}.", language);
            }
        }
    }

    // handle patch note
    string patchNote = ""; // default
    if (!patchNotePath.empty()) {
        if (!exists(patchNotePath)) {
            spdlog::error("Patch note file ({}) doesn't exist!");
        } else {
            patchNote = readTxtFile(patchNotePath);
        }
    }



    // push update
    SubmitItemUpdate(updateHandle, patchNote);

    // default enum value
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

    return 0;
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

bool Uploader::SetUploadLanguage(UGCUpdateHandle_t handle, string language) {
    return SteamUGC()->SetItemUpdateLanguage(handle, language.c_str());
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
        spdlog::info("Update status: {}", GetEItemUpdateStatusDescription(eItemUpdateStatus));

        *previousUpdateStatus = eItemUpdateStatus;
    }

    return (eItemUpdateStatus != k_EItemUpdateStatusInvalid);
}


// create new workshop item
void Uploader::CreateWorkshopItem() {
    // https://partner.steamgames.com/doc/api/ISteamUGC#CreateItem
    // k_EWorkshopFileTypeCommunity can be swapped out to other existing options:
    // https://partner.steamgames.com/doc/api/ISteamRemoteStorage#EWorkshopFileType
    SteamAPICall_t apiCall = SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity);
    m_callResultCreate.Set(apiCall, this, &Uploader::OnCreateWorkshopItemResult);

    // TODO: add a timeout limit or an indicator for the user that the item is being created and awaiting Steam's response
    spdlog::info("Creating new workshop item...");
    while (true) {
        SteamAPI_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
        // stop when the new workshop ID got set
        if (this->m_workshopID != 0) {
            break;
        }
    }

    return;
}


// callback for submit
void Uploader::OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* eResult, bool needUserAgreement)
{
    if (needUserAgreement) {
        spdlog::error("Please accept the Steam Workshop legal agreement before submitting an item update.");
        return;
    }
    if (eResult->m_eResult == k_EResultOK) {
        spdlog::info("Item update submitted successfully!");
    } else {
        // Enum doc: https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdateResult_t
        spdlog::error("Failed to submit item update. ({}: {})", GetEResultName(eResult->m_eResult), GetEResultDescription(eResult->m_eResult));
    };
}

// callback for new workshop item
void Uploader::OnCreateWorkshopItemResult(CreateItemResult_t* eResult, bool needUserAgreement)
{
    if (needUserAgreement) {
        spdlog::error("Please accept the Steam Workshop legal agreement before submitting an item update.");
        return;
    }
    if (eResult->m_eResult == k_EResultOK) {
        spdlog::info("Item created successfully at workshop {}!", eResult->m_nPublishedFileId);
        this->m_workshopID = eResult->m_nPublishedFileId; // update workshop ID
    } else {
        // Enum doc: https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdateResult_t
        spdlog::error("Failed to submit item update. ({}: {})", GetEResultName(eResult->m_eResult), GetEResultDescription(eResult->m_eResult));
    }
}
