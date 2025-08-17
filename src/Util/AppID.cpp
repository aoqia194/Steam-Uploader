#include "AppID.h"

#include "ExecCmd.h"
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

// This method has flaws, when the item gets hidden or friend only once, then the appID can't be
// retrieved, even if you set it back to visible (0) thus I kept it bcs it's quite a nice tool but
// by default it's prefered to give the appID directly source:
// https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
//! Used to retrieve the appID for a specific workshopID.
AppId_t getAppID(const PublishedFileId_t publishedfileid)
{
#ifdef _WIN32
    std::wstring wstr(L"powershell -c \"[regex]::Matches((Invoke-WebRequest -Uri "
                      L"\\\"https://steamcommunity.com/sharedfiles/filedetails/?id=");
    wstr += std::to_wstring(publishedfileid);
    wstr += std::wstring(L"\\\" ).Content, 'data-appid=\\\"(\\d+?)\\\">').Groups[1].Value\"");

    // Convert std::wstring to std::string (UTF-8)
    const int size_needed =
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, nullptr, nullptr);

    const auto out = ExecCmd(str.c_str());
    return strtoul(out.c_str(), nullptr, 10);
#else
    std::string str = "curl -s 'https://steamcommunity.com/sharedfiles/filedetails/?id=";
    str += std::to_string(publishedfileid);
    str += "' | grep -o 'data-appid=\"[0-9]*' | grep -o '[0-9]*'";
    const std::string out = ExecCmd(str.c_str());
    return strtoul(out.c_str(), nullptr, 10);
#endif
}
