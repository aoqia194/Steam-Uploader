#include <iostream>
#include "steam/steam_api.h"
#include "Uploader.h"

UGCUpdateHandle_t Uploader::UploadItem(int workshopID, const std::string& description)
{
    UGCUpdateHandle_t updateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), workshopID);

    bool titleSet = SteamUGC()->SetItemTitle(updateHandle, "Uploader test");
    bool descriptionSet = SteamUGC()->SetItemDescription(updateHandle, description.c_str());
    bool contentSet = SteamUGC()->SetItemContent(updateHandle, "/home/simon/Documents/Repositories/Steam-Uploader/TestUpload");
    bool previewSet = SteamUGC()->SetItemPreview(updateHandle, "/home/simon/Documents/Repositories/Steam-Uploader/preview.png");
    bool visibilitySet = SteamUGC()->SetItemVisibility(updateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);

    std::cout << "Title set: " << titleSet << "\n";
    std::cout << "Description set: " << descriptionSet << "\n";
    std::cout << "Content set: " << contentSet << "\n";
    std::cout << "Preview set: " << previewSet << "\n";
    std::cout << "Visibility set: " << visibilitySet << "\n";

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
        std::cerr << "Failed to submit item update. Error: " << pResult->m_eResult << "\n";
        std::cerr << "Error enum: ";
        switch (pResult->m_eResult) {
            case k_EResultOK: std::cerr << "k_EResultOK"; break;
            case k_EResultFail: std::cerr << "k_EResultFail"; break;
            case k_EResultNoConnection: std::cerr << "k_EResultNoConnection"; break;
            case k_EResultInvalidPassword: std::cerr << "k_EResultInvalidPassword"; break;
            case k_EResultLoggedInElsewhere: std::cerr << "k_EResultLoggedInElsewhere"; break;
            case k_EResultInvalidProtocolVer: std::cerr << "k_EResultInvalidProtocolVer"; break;
            case k_EResultInvalidParam: std::cerr << "k_EResultInvalidParam"; break;
            case k_EResultFileNotFound: std::cerr << "k_EResultFileNotFound"; break;
            case k_EResultBusy: std::cerr << "k_EResultBusy"; break;
            case k_EResultInvalidState: std::cerr << "k_EResultInvalidState"; break;
            case k_EResultInvalidName: std::cerr << "k_EResultInvalidName"; break;
            case k_EResultInvalidEmail: std::cerr << "k_EResultInvalidEmail"; break;
            case k_EResultDuplicateName: std::cerr << "k_EResultDuplicateName"; break;
            case k_EResultAccessDenied: std::cerr << "k_EResultAccessDenied"; break;
            case k_EResultTimeout: std::cerr << "k_EResultTimeout"; break;
            case k_EResultBanned: std::cerr << "k_EResultBanned"; break;
            case k_EResultAccountNotFound: std::cerr << "k_EResultAccountNotFound"; break;
            case k_EResultInvalidSteamID: std::cerr << "k_EResultInvalidSteamID"; break;
            case k_EResultServiceUnavailable: std::cerr << "k_EResultServiceUnavailable"; break;
            case k_EResultNotLoggedOn: std::cerr << "k_EResultNotLoggedOn"; break;
            case k_EResultPending: std::cerr << "k_EResultPending"; break;
            case k_EResultEncryptionFailure: std::cerr << "k_EResultEncryptionFailure"; break;
            case k_EResultInsufficientPrivilege: std::cerr << "k_EResultInsufficientPrivilege"; break;
            case k_EResultLimitExceeded: std::cerr << "k_EResultLimitExceeded"; break;
            case k_EResultRevoked: std::cerr << "k_EResultRevoked"; break;
            case k_EResultExpired: std::cerr << "k_EResultExpired"; break;
            case k_EResultAlreadyRedeemed: std::cerr << "k_EResultAlreadyRedeemed"; break;
            case k_EResultDuplicateRequest: std::cerr << "k_EResultDuplicateRequest"; break;
            case k_EResultAlreadyOwned: std::cerr << "k_EResultAlreadyOwned"; break;
            default: std::cerr << "Unknown"; break;
        }
        std::cerr << std::endl;
    }
}