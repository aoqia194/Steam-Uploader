#pragma once

#include "steam/steam_api.h"

#include <string>
#include <string_view>
#include <unordered_map>

constexpr size_t EItemUpdateStatusMax = k_EItemUpdateStatusCommittingChanges + 1;

//! A map for names of EItemUpdateStatus members.
//! See more: https://partner.steamgames.com/doc/api/ISteamUGC#EItemUpdateStatus
inline std::unordered_map<EItemUpdateStatus, std::string> EItemUpdateStatus_names{
    {             k_EItemUpdateStatusInvalid,              "k_EItemUpdateStatusInvalid"},
    {     k_EItemUpdateStatusPreparingConfig,      "k_EItemUpdateStatusPreparingConfig"},
    {    k_EItemUpdateStatusPreparingContent,     "k_EItemUpdateStatusPreparingContent"},
    {    k_EItemUpdateStatusUploadingContent,     "k_EItemUpdateStatusUploadingContent"},
    {k_EItemUpdateStatusUploadingPreviewFile, "k_EItemUpdateStatusUploadingPreviewFile"},
    {   k_EItemUpdateStatusCommittingChanges,    "k_EItemUpdateStatusCommittingChanges"},
};

// clang-format off

//! A map for descriptions of EItemUpdateStatus members.
//! See more: https://partner.steamgames.com/doc/api/ISteamUGC#EItemUpdateStatus
inline std::unordered_map<EItemUpdateStatus, std::string> EItemUpdateStatus_descriptions {
    { k_EItemUpdateStatusInvalid, "The item update handle was invalid, the job might be finished,"
        " a SubmitItemUpdateResult_t call result should have been returned for it." },
    { k_EItemUpdateStatusPreparingConfig, "The item update is processing configuration data." },
    { k_EItemUpdateStatusPreparingContent, "The item update is reading and processing content files." },
    { k_EItemUpdateStatusUploadingContent, "The item update is uploading content changes to Steam." },
    { k_EItemUpdateStatusUploadingPreviewFile, "The item update is uploading new preview file image." },
    { k_EItemUpdateStatusCommittingChanges, "The item update is committing all changes." },
};

// clang-format on

inline std::string_view EItemUpdateStatus_getName(const EItemUpdateStatus status)
{
    if (status < 0) {
        return "None";
    }

    if (status >= EItemUpdateStatusMax) {
        return "Invalid";
    }

    return EItemUpdateStatus_names[status];
}

inline std::string_view EItemUpdateStatus_getDescription(const EItemUpdateStatus status)
{
    if (status < 0) {
        return "None";
    }

    if (status >= EItemUpdateStatusMax) {
        return "Invalid";
    }

    return EItemUpdateStatus_descriptions[status];
}
