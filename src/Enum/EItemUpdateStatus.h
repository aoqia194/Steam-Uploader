#pragma once
#include <array>
#include <cstddef>
#include "steam/steam_api.h"


// array for EItemUpdateStatus
// https://partner.steamgames.com/doc/api/ISteamUGC#EItemUpdateStatus

// max size of the array
constexpr size_t EItemUpdateStatusCount = k_EItemUpdateStatusCommittingChanges + 1;

// type of elements in the array
struct EItemUpdateStatusInfo {
    const char* name; // directly link to the enum name
    const char* description; // it's description
};

// array
constexpr std::array<EItemUpdateStatusInfo, EItemUpdateStatusCount> EItemUpdateStatusTable {{
    { "k_EItemUpdateStatusInvalid", "The item update handle was invalid, the job might be finished, a SubmitItemUpdateResult_t call result should have been returned for it." }, // 0
    { "k_EItemUpdateStatusPreparingConfig", "The item update is processing configuration data." }, // 1
    { "k_EItemUpdateStatusPreparingContent", "The item update is reading and processing content files." }, // 2
    { "k_EItemUpdateStatusUploadingContent", "The item update is uploading content changes to Steam." }, // 3
    { "k_EItemUpdateStatusUploadingPreviewFile", "The item update is uploading new preview file image." }, // 4
    { "k_EItemUpdateStatusCommittingChanges", "The item update is committing all changes." }, // 5
}};

constexpr const char* GetEItemUpdateStatusName(EItemUpdateStatus eItemUpdateStatus) {
    return (eItemUpdateStatus >= 0 && static_cast<size_t>(eItemUpdateStatus) < EItemUpdateStatusTable.size())
        ? EItemUpdateStatusTable[static_cast<size_t>(eItemUpdateStatus)].name
        : "k_EItemUpdateStatusNone";
}

constexpr const char* GetEItemUpdateStatusDescription(EItemUpdateStatus eItemUpdateStatus) {
    return (eItemUpdateStatus >= 0 && static_cast<size_t>(eItemUpdateStatus) < EItemUpdateStatusTable.size())
        ? EItemUpdateStatusTable[static_cast<size_t>(eItemUpdateStatus)].description
        : "No description available.";
}
