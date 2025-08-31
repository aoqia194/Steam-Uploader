#include "Uploader.h"

#include "Enum/EItemUpdateStatus.h"
#include "Enum/EResult.h"

#include "Util/AppID.h"
#include "Util/HasLanguage.h"
#include "Util/ReadFile.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <iterator>

// Uploader class implementation

// constructor
Uploader::Uploader(PublishedFileId_t workshopID, const AppId_t appID, const bool createNewUgc)
{
    // appID is a mandatory argument.
    if (appID == 0) {
        spdlog::error("appID is a required argument for Uploader");
        return;
    }
    this->m_appID = appID;
    setAppID(appID);

    this->steamInit = InitSteamAPI();
    if (!this->steamInit) { return; }

    // create new item if needed
    if (createNewUgc) {
        // catch user possibly doing wrong usage if they gave a wID and try to create a
        // new item
        if (workshopID != k_PublishedFileIdInvalid) {
            spdlog::error("Warning: You provided workshopID {} while creating a new item but "
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

    // // for use when setting everything up
    // this->m_isNew = createNewUgc;
}

// destructor
Uploader::~Uploader() {
    if (this->steamInit) {
        ShutdownSteamAPI();
    }
}


// initialize Steam API
bool Uploader::InitSteamAPI()
{
    const bool success = SteamAPI_Init();
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

int Uploader::UpdateItem(const std::optional<fs::path> &descriptionPath,
    const std::optional<fs::path> &previewPath, const std::optional<fs::path> &contentPath,
    const std::optional<std::string> &title, const std::optional<int8_t> visibility,
    const std::optional<std::vector<std::string>> &tags,
    const std::optional<fs::path> &patchNotePath, const std::optional<std::string> &language)
{
    UGCUpdateHandle_t updateHandle = CreateUpdateHandle(this->m_workshopID);

    SetItemDescription(updateHandle, descriptionPath);
    SetItemPreview(updateHandle, previewPath);
    SetItemContent(updateHandle, contentPath);
    SetItemTitle(updateHandle, title);
    SetItemVisibility(updateHandle, visibility);
    SetTags(updateHandle, tags);
    SetUploadLanguage(updateHandle, language);

    // Patch notes
    std::string patchNote;
    if (patchNotePath.has_value()) {
        if (!fs::exists(*patchNotePath)) {
            spdlog::error("Patch note file ({}) doesn't exist!", patchNotePath->string());
        } else {
            patchNote = readTxtFile(*patchNotePath);
        }
    }

    // Submit update to the workshop item.
    SubmitItemUpdate(updateHandle, patchNote);

    // Continually monitor and update item status for errors until it's complete.
    EItemUpdateStatus previousUpdateStatus = k_EItemUpdateStatusInvalid;
    while (CheckProgress(updateHandle, &previousUpdateStatus)) {
        SteamAPI_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cleanup after update.
    // SteamAPI_RunCallbacks();

    return 0;
}

UGCUpdateHandle_t Uploader::CreateUpdateHandle(const PublishedFileId_t workshopID)
{
    return SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), workshopID);
}

bool Uploader::SetItemTitle(const UGCUpdateHandle_t handle, const std::optional<std::string> &title)
{
    if (!title.has_value()) {
        return false;
    }

    if (title->size() > k_cchPublishedDocumentTitleMax) {
        spdlog::error("Title with length {} exceeds maximum length of {} characters.",
            k_cchPublishedDocumentTitleMax, title->size());
        return false;
    }

    const auto ret = SteamUGC()->SetItemTitle(handle, title->c_str());
    if (!ret) {
        spdlog::error("Failed to set item title to ({})", *title);
    }

    return ret;
}

bool Uploader::SetItemDescription(const UGCUpdateHandle_t handle,
    const std::optional<fs::path> &descriptionFile)
{
    if (!descriptionFile.has_value()) {
        return false;
    }

    const auto pathStr = descriptionFile->string();
    if (!fs::exists(*descriptionFile) || !fs::is_regular_file(*descriptionFile)) {
        spdlog::error("Description file ({}) doesn't exist!", pathStr);
        return false;
    }

    const auto description = readTxtFile(pathStr);
    if (description.size() > k_cchPublishedDocumentDescriptionMax) {
        spdlog::error("Description with length {} exceeds maximum length of {} characters.",
            description.size(), k_cchPublishedDocumentDescriptionMax);
        return false;
    }

    const auto ret = SteamUGC()->SetItemDescription(handle, description.c_str());
    if (!ret) {
        spdlog::error("Failed to set item description with status {}", ret);
    }

    return ret;
}

bool Uploader::SetItemContent(const UGCUpdateHandle_t handle,
    const std::optional<fs::path> &contentPath)
{
    if (!contentPath.has_value()) {
        return false;
    }

    const auto pathStr = contentPath->string();
    if (!fs::exists(*contentPath) || !fs::is_directory(*contentPath)) {
        spdlog::error("Content directory ({}) doesn't exist!", pathStr);
        return false;
    }

    const auto ret = SteamUGC()->SetItemContent(handle, pathStr.c_str());
    if (!ret) {
        spdlog::error("Failed to set item content with status {}", ret);
    }

    return ret;
}

bool Uploader::SetItemPreview(const UGCUpdateHandle_t handle,
    const std::optional<fs::path> &previewFile)
{
    if (!previewFile.has_value()) {
        return false;
    }

    const auto pathStr = previewFile->string();
    if (!fs::exists(*previewFile) || !fs::is_regular_file(*previewFile)) {
        spdlog::error("Preview file ({}) doesn't exist!", pathStr);
        return false;
    }

    if (const auto previewSize = fs::file_size(*previewFile) >= 1048576) {
        spdlog::error("Preview file with size {} exceeds maximum size of 1048576 bytes.",
            previewSize);
        return false;
    }

    const auto ret = SteamUGC()->SetItemPreview(handle, pathStr.c_str());
    if (!ret) {
        spdlog::error("Failed to set workshop item preview with status {}", ret);
    }

    return ret;
}

bool Uploader::SetItemVisibility(const UGCUpdateHandle_t handle,
    const std::optional<int8_t> visibility)
{
    if (!visibility.has_value()) {
        return false;
    }

    if (*visibility < k_ERemoteStoragePublishedFileVisibilityPublic ||
        *visibility > k_ERemoteStoragePublishedFileVisibilityUnlisted)
    {
        spdlog::error("Invalid visibility with value ({}). Must be between 0 and 3!", *visibility);
        return false;
    }

    const auto ret = SteamUGC()->SetItemVisibility(handle,
        static_cast<ERemoteStoragePublishedFileVisibility>(*visibility));
    if (!ret) {
        spdlog::error("Failed to set item visibility to ({})", *visibility);
    }

    return ret;
}

bool Uploader::SetTags(const UGCUpdateHandle_t handle,
    const std::optional<std::vector<std::string>> &tags)
{
    if (!tags.has_value()) {
        return false;
    }

    std::vector<const char *> tagsCstr{};
    tagsCstr.resize(tags->size());
    std::ranges::transform(tags->cbegin(), tags->cend(), tagsCstr.begin(), &std::string::c_str);

    SteamParamStringArray_t tagArray{};
    tagArray.m_ppStrings = tagsCstr.empty() ? nullptr : tagsCstr.data();
    tagArray.m_nNumStrings = static_cast<int32_t>(tags->size());

    const auto ret = SteamUGC()->SetItemTags(handle, &tagArray);
    if (!ret) {
        std::ostringstream os;
        std::ranges::copy(*tags, std::ostream_iterator<std::string>(os, ","));
        spdlog::error("Failed to set item tags to {}", os.str());
    }

    return ret;
}

bool Uploader::SetUploadLanguage(const UGCUpdateHandle_t handle,
    const std::optional<std::string> &language)
{
    if (!language.has_value() || *language == "english") {
        return false;
    }

    if (!IsValidSteamLanguageCode(*language)) {
        spdlog::error("Invalid language code {}", *language);
        return false;
    }

    const auto ret = SteamUGC()->SetItemUpdateLanguage(handle, language->c_str());
    if (!ret) {
        spdlog::error("Failed to set upload language to {}", *language);
    }

    return ret;
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
    const EItemUpdateStatus eItemUpdateStatus =
        SteamUGC()->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytes);

    const bool isValid = eItemUpdateStatus != k_EItemUpdateStatusInvalid;

    if (isValid && eItemUpdateStatus != *previousUpdateStatus) {
        spdlog::info("Update status: {}", EItemUpdateStatus_getDescription(eItemUpdateStatus));

        *previousUpdateStatus = eItemUpdateStatus;
    }

    return eItemUpdateStatus != k_EItemUpdateStatusInvalid;
}

// create new workshop item
void Uploader::CreateWorkshopItem()
{
    // https://partner.steamgames.com/doc/api/ISteamUGC#CreateItem
    // k_EWorkshopFileTypeCommunity can be swapped out to other existing options:
    // https://partner.steamgames.com/doc/api/ISteamRemoteStorage#EWorkshopFileType
    const SteamAPICall_t apiCall =
        SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity);
    m_callResultCreate.Set(apiCall, this, &Uploader::OnCreateWorkshopItemResult);

    // TODO: add a timeout limit or an indicator for the user that the item is being
    // created and awaiting Steam's response
    spdlog::info("Creating new workshop item...");
    while (true) {
        SteamAPI_RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // stop when the new workshop ID got set
        if (this->m_workshopID != k_PublishedFileIdInvalid) {
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
