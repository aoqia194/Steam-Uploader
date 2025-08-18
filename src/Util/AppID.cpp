#include "AppID.h"

#include "spdlog/spdlog.h"

#include <fstream>

// source:
// https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
//! Updates the steam_appid.txt file which the steam API reads to know what appID to use
void setAppID(const AppId_t appID)
{
    spdlog::debug("Writing appID to steam_appid.txt file...");

    std::ofstream file("steam_appid.txt", std::ios::out | std::ios::trunc);
    if (!file.is_open() || !file.good()) {
        spdlog::error("Failed to write appID to steam_appid file.");
        return;
    }

    file << appID;
    file.close();
}
