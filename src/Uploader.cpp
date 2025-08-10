#include "Uploader.h"
#include "Util/EResult.h"

UGCUpdateHandle_t Uploader::UploadItem(PublishedFileId_t workshopID, const std::string& description)
{
    UGCUpdateHandle_t updateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), workshopID);

    // title
    bool titleSet = SteamUGC()->SetItemTitle(updateHandle, "Uploader test");
    std::cout << "Title set: " << titleSet << "\n";
    
    // description
    bool descriptionSet = SteamUGC()->SetItemDescription(updateHandle, description.c_str());
    std::cout << "Description set: " << descriptionSet << "\n";
    
    // content
    bool contentSet = SteamUGC()->SetItemContent(updateHandle, "/home/simon/Documents/test/testMod");
    std::cout << "Content set: " << contentSet << "\n";
    
    // preview
    bool previewSet = SteamUGC()->SetItemPreview(updateHandle, "/home/simon/Documents/Repositories/Steam-Uploader/preview.png");
    std::cout << "Preview set: " << previewSet << "\n";
    
    // visibility
    bool visibilitySet = SteamUGC()->SetItemVisibility(updateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);
    std::cout << "Visibility set: " << visibilitySet << "\n";


    
    // https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdate
    SteamAPICall_t apiCall = SteamUGC()->SubmitItemUpdate(updateHandle, "Test upload");
    m_callResultSubmit.Set(apiCall, this, &Uploader::OnSubmitItemUpdateResult);

    return updateHandle;
}

bool Uploader::CheckProgress(UGCUpdateHandle_t updateHandle)
{
    uint64 bytesProcessed = 0;
    uint64 bytesTotal = 0;
    EItemUpdateStatus status = SteamUGC()->GetItemUpdateProgress(updateHandle, &bytesProcessed, &bytesTotal);

    std::cout << "Update status: " << status << ", Bytes processed: " << bytesProcessed << ", Total bytes: " << bytesTotal << "\n";

    return (status != k_EItemUpdateStatusInvalid);
}

void Uploader::OnSubmitItemUpdateResult(SubmitItemUpdateResult_t* pResult, bool bIOFailure)
{
    if (bIOFailure) {
        std::cerr << "IO Failure during item update submission.\n";
        return;
    }
    if (pResult->m_eResult == k_EResultOK) {
        std::cout << "Item update submitted successfully!\n";
    } else {

        // Enum doc: https://partner.steamgames.com/doc/api/ISteamUGC#SubmitItemUpdateResult_t

        std::cerr << "Failed to submit item update. (" << GetEResultName(pResult->m_eResult) << ": " << GetEResultDescription(pResult->m_eResult) << ")\n";
    };
}