#pragma once

#include "curl/curl.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <fstream>
#include <iostream>
#include <string>

// Helper function for libcurl to write response to a string
static size_t WriteCallback(void *buf, const size_t elemSize, const size_t elemCount, void *stream)
{
    static_cast<std::string *>(stream)->append(static_cast<char *>(buf), elemSize * elemCount);
    return elemSize * elemCount;
}

// Helper to fetch the latest version from GitHub using libcurl and nlohmann::json
inline std::string fetch_latest_version()
{
    std::string readBuffer;

    if (CURL *curl = curl_easy_init()) {
        // NOTE: GitHub API requires a user agent!
        curl_easy_setopt(curl, CURLOPT_URL,
            "https://api.github.com/repos/sirdoggyjvla/Steam-Uploader/releases/latest");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Steam-Uploader");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        const CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) {
            return "";
        }
    }

    try {
        const auto json = nlohmann::json::parse(readBuffer);
        return json.value("tag_name", "");
    } catch (...) {
        return "";
    }
}

// Helper to perform the update (replace with your actual update logic)
inline void perform_update()
{
    spdlog::info("Updating Steam-Uploader..");

// assets and exec names based on windows or linux
#ifdef _WIN32
    const std::string asset_name = "app-windows-latest.zip";
    const std::string exe_name = "app-windows-latest.exe";
    const std::string exe_target = "app-windows-latest.exe";
    const std::string dll_name = "steam_api64.dll";
    const std::string dll_target = "steam_api64.dll";
#else
    const std::string asset_name = "app-ubuntu-latest.zip";
    const std::string exe_name = "app-ubuntu-latest";
    const std::string exe_target = "app-ubuntu-latest";
    const std::string so_name = "SteamAPI/linux64/libsteam_api.so";
    const std::string so_target = "SteamAPI/linux64/libsteam_api.so";
#endif

    // Download the latest release info from GitHub API
    std::string json_data;

    if (CURL *curl = curl_easy_init()) {
#ifdef _WIN32
        curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
#endif
        curl_easy_setopt(curl, CURLOPT_URL,
            "https://api.github.com/repos/sirdoggyjvla/Steam-Uploader/releases/latest");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Steam-Uploader");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json_data);

        if (const CURLcode res = curl_easy_perform(curl); res != CURLE_OK) {
            spdlog::error("curl_easy_perform() failed: {}", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
    }

    // Parse JSON to find the correct asset
    std::string zip_url;
    try {
        const auto json = nlohmann::json::parse(json_data);
        for (const auto &asset : json["assets"]) {
            std::string name = asset.value("name", "");
            if (name == asset_name) {
                zip_url = asset.value("browser_download_url", "");
                break;
            }
        }
    } catch (...) {
        spdlog::error("Failed to parse release info.");
        spdlog::debug("Raw JSON data: {}", json_data);
        return;
    }
    if (zip_url.empty()) {
        spdlog::error("Could not find release asset: {}", asset_name);
        return;
    }

    // Download the update ZIP file.
    spdlog::info("Downloading ZIP from: ({})", zip_url);
#ifdef _WIN32
    std::system(("curl -L -o update.zip \"" + zip_url + "\"").c_str());
    std::system("powershell -Command \"Expand-Archive -Force update.zip temp-update\"");
    // Write a batch file to do the update after this process exits
    // TODO: Have the update scripts in a scripts/ folder in the repository eg. scripts/updater.bat.
    //       Download them with CURL to system temp folder and execute them.
    //       This writing the update script from the program itself stuff that is done here is bad.
    std::ofstream updater("run_update.bat");
    updater << "@echo off\n";
    updater << "echo Waiting for Steam-Uploader to exit...\n";
    updater << ":loop\n";
    updater << "tasklist | findstr /I \"app-windows-latest.exe\" >nul && (timeout /t 1 >nul & goto "
               "loop)\n";
    updater << "move /Y temp-update\\app-windows-latest.exe app-windows-latest.exe\n";
    updater << "move /Y temp-update\\steam_api64.dll steam_api64.dll\n";
    updater << "rmdir /S /Q temp-update\n";
    updater << "del update.zip\n";
    // updater << "start \"\" app-windows-latest.exe\n";
    updater << "del \"%~f0\"\n"; // Deletes itself
    updater.close();
    // Launch the updater and exit
    std::system("start run_update.bat");
    exit(0);
#else
    std::system(("curl -L -o update.zip \"" + zip_url + "\"").c_str());
    std::system("unzip -o update.zip");
    std::system(("mv -f " + exe_name + " " + exe_target).c_str());
    std::system(("mv -f " + so_name + " " + so_target).c_str());
    std::system("rm -f update.zip");
#endif

    spdlog::info("Update complete!");
}
