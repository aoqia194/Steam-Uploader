#include <thread>
#include <fstream>
#include <string>
#include "Uploader.h"
#include "WarningHook.h"
#include "ExecCmd.h"

// Tries to implement this:
// https://partner.steamgames.com/doc/features/workshop/implementation#uploading_a_workshop_item



// source: https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
void SetAppId(AppId_t appid) {
	printf("Writing appid to steam_appid.txt\n");
	std::ofstream myfile;
	myfile.open("steam_appid.txt");
	myfile << appid;
	myfile.close();
}

// source: https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
#ifdef _WIN32
AppId_t getAppid(PublishedFileId_t publishedfileid) {
    std::wstring str(L"powershell -c \"[regex]::Matches((Invoke-WebRequest -Uri \\\"https://steamcommunity.com/sharedfiles/filedetails/?id=");
    str += std::to_wstring(publishedfileid);
    str += std::wstring(L"\\\" ).Content, 'data-appid=\\\"(\\d+?)\\\">').Groups[1].Value\"");
    auto out = ExecCmd(str.c_str());
    return strtoul(out, NULL, 10);
}
#else
AppId_t getAppid(PublishedFileId_t publishedfileid) {
    std::string str = "curl -s 'https://steamcommunity.com/sharedfiles/filedetails/?id=";
    str += std::to_string(publishedfileid);
    str += "' | grep -o 'data-appid=\"[0-9]*' | grep -o '[0-9]*'";
    std::string out = ExecCmd(str.c_str());
    return strtoul(out.c_str(), NULL, 10);
}
#endif

int main() {
    PublishedFileId_t workshopID = 3545758025;
    AppId_t appid = getAppid(workshopID);
    if (appid == 0) {
        std::cerr << "Failed to retrieve AppId for workshop item: " << workshopID << "\n";
        return 1;
    }
    
    SetAppId(appid);
    if (SteamAPI_Init()) {
        std::cout << "Steam API initialized!\n";
        EnableWarningMessageHook();


        uint32 itemState = SteamUGC()->GetItemState(workshopID);
        std::cout << "Item state: " << itemState << "\n";

        std::string description = "This is the description of the workshop item.";

        std::cout << description << "\n";
        std::cout << SteamUtils()->GetAppID() << "\n";

        Uploader uploader;
        UGCUpdateHandle_t updateHandle = uploader.UploadItem(workshopID, description);

        uint64 bytesProcessed = 0;
        uint64 bytesTotal = 0;

        while (true) {
            bool isValid = uploader.CheckProgress(updateHandle);

            SteamAPI_RunCallbacks();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (!isValid) {
                std::cerr << "Invalid item update status.\n";
                break;
            }
        }

        SteamAPI_Shutdown();
    } else {
        std::cerr << "Failed to initialize Steam API.\n";
    }
    return 0;
}
