#include <iostream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// Helper function for libcurl to write response to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Helper to fetch the latest version from GitHub using libcurl and nlohmann::json
std::string fetch_latest_version() {
    CURL* curl = curl_easy_init();
    std::string readBuffer;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/sirdoggyjvla/Steam-Uploader/releases/latest");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Steam-Uploader"); // GitHub API requires a user agent
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK) return "";
    }
    try {
        auto json = nlohmann::json::parse(readBuffer);
        return json.value("tag_name", "");
    } catch (...) {
        return "";
    }
}

// Helper to perform the update (replace with your actual update logic)
void perform_update() {
    std::cout << "Updating Steam-Uploader..." << std::endl;

#if defined(_WIN32)
    const std::string asset_name = "app-windows-latest.zip";
    const std::string exe_name = "app-windows-latest.exe";
    const std::string exe_target = "app.exe";
    const std::string dll_name = "SteamAPI/win64/steam_api64.dll";
    const std::string dll_target = "SteamAPI/win64/steam_api64.dll";
#else
    const std::string asset_name = "app-ubuntu-latest.zip";
    const std::string exe_name = "app-ubuntu-latest";
    const std::string exe_target = "app";
    const std::string so_name = "SteamAPI/linux64/libsteam_api.so";
    const std::string so_target = "SteamAPI/linux64/libsteam_api.so";
#endif

    // Download the latest release info from GitHub API
    std::string api_url = "https://api.github.com/repos/sirdoggyjvla/Steam-Uploader/releases/latest";
    std::string json_data;
    {
        CURL* curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Steam-Uploader");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json_data);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
    }
    // Parse JSON to find the correct asset
    std::string zip_url;
    try {
        auto json = nlohmann::json::parse(json_data);
        for (const auto& asset : json["assets"]) {
            std::string name = asset.value("name", "");
            if (name == asset_name) {
                zip_url = asset.value("browser_download_url", "");
                break;
            }
        }
    } catch (...) {
        std::cerr << "Failed to parse release info." << std::endl;
        return;
    }
    if (zip_url.empty()) {
        std::cerr << "Could not find release asset: " << asset_name << std::endl;
        return;
    }
    // Download the ZIP file
    std::cout << "Downloading: " << zip_url << std::endl;
#if defined(_WIN32)
    std::system(("curl -L -o update.zip \"" + zip_url + "\"").c_str());
    // Extract the ZIP file (assumes PowerShell is available)
    std::system("powershell -Command \"Expand-Archive -Force update.zip .\"");
    // Replace the binary and DLL
    std::system(("move /Y \"" + exe_name + "\" \"" + exe_target + "\"").c_str());
    std::system(("move /Y \"" + dll_name + "\" \"" + dll_target + "\"").c_str());
    // Clean up
    std::system("del update.zip");
#else
    std::system(("curl -L -o update.zip \"" + zip_url + "\"").c_str());
    std::system("unzip -o update.zip");
    std::system(("mv -f " + exe_name + " " + exe_target).c_str());
    std::system(("mv -f " + so_name + " " + so_target).c_str());
    std::system("rm -f update.zip");
#endif
    std::cout << "Update complete." << std::endl;
}