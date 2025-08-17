#include "Uploader.h"

#include "Enum/EItemUpdateStatus.h"
#include "Enum/EResult.h"

#include "Util/AppID.h"
#include "Util/HasLanguage.h"
#include "Util/ReadFile.h"

#include "spdlog/spdlog.h"

// Uploader class implementation

Uploader::Uploader(PublishedFileId_t workshopID, const AppId_t appID, const bool createNewUgc)
{
    // appID is a mandatory argument.
    if (appID == 0) {
        spdlog::error("appID is a required argument for Uploader");
        return;
    }
    this->m_appID = appID;
    setAppID(appID);

    InitSteamAPI();

    // create new item if needed
    if (createNewUgc) {
        // catch user possibly doing wrong usage if they gave a wID and try to create a
        // new item
        if (workshopID != 0) {
            spdlog::error("Warning: You provided a workshopID {} while creating a new item but "
                          "this ID will be ignored since a new one is created instead.",
                workshopID);
        }

        this->m_workshopID = 0; // default value used when creating the item
        CreateWorkshopItem();   // this will set workshopID to the newly created item
                                // workshopID

        // workshopID can't be 0 if no new item is being created
    } else if (workshopID == 0) {
        spdlog::error("workshopID is a required argument. Either specify it, or use --new.");
        ShutdownSteamAPI();
        return;

    } else {
        this->m_workshopID = workshopID;
    }

    // for use when setting everything up
    this->m_isNew = createNewUgc;
}

// initialize Steam API
bool Uploader::InitSteamAPI()
{
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
bool Uploader::ShutdownSteamAPI()
{
    spdlog::debug("ShutdownSteamAPI called - Shutting down SteamAPI.");
    SteamAPI_Shutdown();
    return true;
}

// TODO: Should probably split this up into multiple subfunctions, this function is massive!
int Uploader::UpdateItem(std::optional<fs::path> descriptionPath,
    std::optional<fs::path> previewPath, std::optional<fs::path> contentPath,
    std::optional<std::string> title,
    std::optional<ERemoteStoragePublishedFileVisibility> visibility,
    std::optional<std::vector<std::string>> tags, std::optional<fs::path> patchNotePath,
    std::optional<std::string> language)
{
    UGCUpdateHandle_t updateHandle = CreateUpdateHandle(this->m_workshopID);

    // Description
    if (descriptionPath.has_value()) {
        const auto pathStr = descriptionPath->string();

        if (!fs::exists(pathStr)) {
            spdlog::error("Description file ({}) doesn't exist!", pathStr);
        } else {
            const auto description = readTxtFile(pathStr);
            if (description.size() > k_cchPublishedDocumentDescriptionMax) {
                spdlog::error("Description with length {} exceeds maximum length of {} characters.",
                    description.size(), k_cchPublishedDocumentDescriptionMax);
            } else if (const auto ret = SetItemDescription(updateHandle, pathStr); !ret) {
                spdlog::error("Failed to set item description with status {}", ret);
            }
        }
    }

    // Preview
    if (previewPath.has_value()) {
        const auto pathStr = previewPath->string();

        if (!fs::exists(*previewPath) || !fs::is_regular_file(*previewPath)) {
            spdlog::warn("Preview file ({}) doesn't exist!", pathStr);
        } else if (const auto previewSize = fs::file_size(*previewPath) >= 1048576) {
            spdlog::warn("Preview file with size {} exceeds maximum size of 1048576 bytes.",
                previewSize);
        } else if (const auto ret = SetItemPreview(updateHandle, pathStr); !ret) {
            spdlog::error("Failed to set workshop item preview with status {}", ret);
        }
    }

    // Content
    if (contentPath.has_value()) {
        const auto pathStr = contentPath->string();

        if (!fs::exists(*contentPath) || !fs::is_directory(*contentPath)) {
            spdlog::error("Content directory ({}) doesn't exist!", pathStr);
        } else if (const auto ret = SetItemContent(updateHandle, pathStr.c_str()); !ret) {
            spdlog::error("Failed to set item content with status {}", ret);
        }
    }

    // Title
    if (title.has_value()) {
        if (title->size() > k_cchPublishedDocumentTitleMax) {
            spdlog::error("Title with length {} exceeds maximum length of {} characters.",
                k_cchPublishedDocumentTitleMax, title->size());
        } else if (const auto ret = SetItemTitle(updateHandle, *title); !ret) {
            spdlog::error("Failed to set item title to ({})", *title);
        }
    }

    // Visibility
    if (const auto visibilityIdx = static_cast<int8_t>(*visibility); visibilityIdx != -1) {
        if (visibility >= k_ERemoteStoragePublishedFileVisibilityPublic &&
            visibility <= k_ERemoteStoragePublishedFileVisibilityUnlisted)
        {
            if (!SetItemVisibility(updateHandle, *visibility)) {
                spdlog::error("Failed to set item visibility to ({})", visibilityIdx);
            }
        } else {
            spdlog::error("Invalid visibility with value ({}). Must be between 0 and 3!",
                visibilityIdx);
        }
    }

    // Tags
    if (tags.has_value()) {
        // Modify tags in-place. They are copied into the function, so it's ok.
        std::vector<const char *> tagsCstr{};
        tagsCstr.resize(tags->size());
        std::ranges::transform(tags->cbegin(), tags->cend(), tagsCstr.begin(), &std::string::c_str);

        SteamParamStringArray_t tagArray{};
        tagArray.m_ppStrings = tagsCstr.empty() ? nullptr : tagsCstr.data();
        tagArray.m_nNumStrings = static_cast<int32_t>(tags->size());

        if (!SetTags(updateHandle, &tagArray)) {
            std::ostringstream os;
            std::ranges::copy(*tags, std::ostream_iterator<std::string>(os, ","));
            spdlog::error("Failed to set item tags to {}", os.str());
        }
    }

    // Language
    if (language.has_value() && *language != "english") {
        if (!IsValidSteamLanguageCode(*language)) {
            spdlog::error("Invalid language code {}", *language);
        } else if (!SetUploadLanguage(updateHandle, *language)) {
            spdlog::error("Failed to set upload language to {}", *language);
        }
    }

    // Patch notes
    // TODO: Maybe a SetPatchNote function here for similarity to all the other ones?
    std::string patchNote = "";
    if (patchNotePath.has_value()) {
        if (!fs::exists(*patchNotePath)) {
            spdlog::error("Patch note file ({}) doesn't exist!", patchNotePath->string());
        } else {
            patchNote = readTxtFile(*patchNotePath);
        }
    }

    // Submit update to the workshop item.
    SubmitItemUpdate(updateHandle, patchNote);

    // Continually monitor and update item status for errors.
    EItemUpdateStatus previousUpdateStatus = k_EItemUpdateStatusInvalid;
    while (CheckProgress(updateHandle, &previousUpdateStatus)) {
        SteamAPI_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup after update.
    SteamAPI_RunCallbacks();
    ShutdownSteamAPI();
    return 0;
}

UGCUpdateHandle_t Uploader::CreateUpdateHandle(const PublishedFileId_t workshopID)
{
    return SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), workshopID);
}

bool Uploader::SetItemTitle(const UGCUpdateHandle_t handle, const std::string &title)
{
    return SteamUGC()->SetItemTitle(handle, title.c_str());
}

bool Uploader::SetItemDescription(const UGCUpdateHandle_t handle, const std::string &pchDescription)
{
    return SteamUGC()->SetItemDescription(handle, pchDescription.c_str());
}

bool Uploader::SetItemContent(const UGCUpdateHandle_t handle, const std::string &content)
{
    return SteamUGC()->SetItemContent(handle, content.c_str());
}

bool Uploader::SetItemPreview(const UGCUpdateHandle_t handle, const std::string &preview)
{
    return SteamUGC()->SetItemPreview(handle, preview.c_str());
}

bool Uploader::SetItemVisibility(const UGCUpdateHandle_t handle,
    const ERemoteStoragePublishedFileVisibility visibility)
{
    return SteamUGC()->SetItemVisibility(handle, visibility);
}

bool Uploader::SetTags(const UGCUpdateHandle_t handle, const SteamParamStringArray_t *tags)
{
    return SteamUGC()->SetItemTags(handle, tags);
}

bool Uploader::SetUploadLanguage(const UGCUpdateHandle_t handle, const std::string &language)
{
    return SteamUGC()->SetItemUpdateLanguage(handle, language.c_str());
}

void Uploader::SubmitItemUpdate(const UGCUpdateHandle_t updateHandle, const std::string &content)
{
    // https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdate
    const SteamAPICall_t apiCall = SteamUGC()->SubmitItemUpdate(updateHandle, content.c_str());
    m_callResultSubmit.Set(apiCall, this, &Uploader::OnSubmitItemUpdateResult);
}

// used to check the current upload status
bool Uploader::CheckProgress(const UGCUpdateHandle_t updateHandle,
    EItemUpdateStatus *previousUpdateStatus)
{
    uint64 bytesProcessed = 0;
    uint64 bytes = 0;
    EItemUpdateStatus eItemUpdateStatus =
        SteamUGC()->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytes);

    bool isValid = eItemUpdateStatus != k_EItemUpdateStatusInvalid;

    if (isValid && eItemUpdateStatus != *previousUpdateStatus) {
        spdlog::info("Update status: {}", EItemUpdateStatus_getDescription(eItemUpdateStatus));

        *previousUpdateStatus = eItemUpdateStatus;
    }

    return (eItemUpdateStatus != k_EItemUpdateStatusInvalid);
}

// create new workshop item
void Uploader::CreateWorkshopItem()
{
    // https://partner.steamgames.com/doc/api/ISteamUGC#CreateItem
    // k_EWorkshopFileTypeCommunity can be swapped out to other existing options:
    // https://partner.steamgames.com/doc/api/ISteamRemoteStorage#EWorkshopFileType
    SteamAPICall_t apiCall =
        SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity);
    m_callResultCreate.Set(apiCall, this, &Uploader::OnCreateWorkshopItemResult);

    // TODO: add a timeout limit or an indicator for the user that the item is being
    // created and awaiting Steam's response
    spdlog::info("Creating new workshop item...");
    while (true) {
        SteamAPI_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // stop when the new workshop ID got set
        if (this->m_workshopID != 0) {
            break;
        }
    }
}

void Uploader::OnSubmitItemUpdateResult(SubmitItemUpdateResult_t *eResult,
    const bool needUserAgreement)
{
    if (needUserAgreement) {
        spdlog::error("Please accept the Steam Workshop legal agreement before "
                      "submitting an item update.");
        return;
    }
    if (eResult->m_eResult == k_EResultOK) {
        spdlog::info("Item update submitted successfully!");
    } else {
        // https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdateResult_t
        spdlog::error("Failed to submit item update. ({}: {})", EResult_getName(eResult->m_eResult),
            EResult_getDescription(eResult->m_eResult));
    };
}

void Uploader::OnCreateWorkshopItemResult(CreateItemResult_t *eResult, const bool needUserAgreement)
{
    if (needUserAgreement) {
        spdlog::error(
            "Please accept the Steam Workshop legal agreement before submitting an item update.");
        return;
    }
    if (eResult->m_eResult == k_EResultOK) {
        spdlog::info("Item created successfully at workshop {}!", eResult->m_nPublishedFileId);
        this->m_workshopID = eResult->m_nPublishedFileId;
    } else {
        // https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdateResult_t
        spdlog::error("Failed to submit item update. ({}: {})", EResult_getName(eResult->m_eResult),
            EResult_getDescription(eResult->m_eResult));
    }
}
