#pragma once

#include "steam/steam_api.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class Uploader
{
public:
    Uploader(PublishedFileId_t workshopID, AppId_t appID, bool createNewUgc);

    int UpdateItem(std::optional<fs::path> descriptionPath, std::optional<fs::path> previewPath,
        std::optional<fs::path> contentPath, std::optional<std::string> title,
        std::optional<ERemoteStoragePublishedFileVisibility> visibility,
        std::optional<std::vector<std::string>> tags, std::optional<fs::path> patchNotePath,
        std::optional<std::string> language);

    bool InitSteamAPI();
    bool ShutdownSteamAPI();
    static bool CheckProgress(UGCUpdateHandle_t updateHandle, EItemUpdateStatus *previousUpdateStatus);

private:
    PublishedFileId_t m_workshopID;
    AppId_t m_appID;
    bool m_isNew;

    //! Creates a new workshop item that defines the `m_workshopID` of the Uploader.
    void CreateWorkshopItem();

    //! Creates the handle used for updating various aspects of a workshop item.
    static UGCUpdateHandle_t CreateUpdateHandle(PublishedFileId_t workshopID);

    // Functions that change UGC item data

    static bool SetItemTitle(UGCUpdateHandle_t handle, const std::string &title);
    static bool SetItemDescription(UGCUpdateHandle_t handle, const std::string &pchDescription);
    static bool SetItemContent(UGCUpdateHandle_t handle, const std::string &content);
    static bool SetItemPreview(UGCUpdateHandle_t handle, const std::string &preview);
    static bool SetItemVisibility(UGCUpdateHandle_t handle,
        ERemoteStoragePublishedFileVisibility visibility);
    static bool SetTags(UGCUpdateHandle_t handle, const SteamParamStringArray_t *tags);
    static bool SetUploadLanguage(UGCUpdateHandle_t handle, const std::string &language);

    //! Submits the item update with the patch note.
    void SubmitItemUpdate(UGCUpdateHandle_t updateHandle, const std::string &content);

    // CALLBACKS

    //! Gets the result of updating the workshop item.
    void OnSubmitItemUpdateResult(SubmitItemUpdateResult_t *eResult, bool needUserAgreement);
    CCallResult<Uploader, SubmitItemUpdateResult_t> m_callResultSubmit;

    //! Gets the result of creating a new workshop item.
    void OnCreateWorkshopItemResult(CreateItemResult_t *eResult, bool needUserAgreement);
    CCallResult<Uploader, CreateItemResult_t> m_callResultCreate;
};