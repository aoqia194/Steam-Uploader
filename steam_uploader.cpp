#include "steam/steam_api.h"
#include <iostream>
#include <thread>
#include "Uploader.h"


int main() {
    if (SteamAPI_Init()) {
        std::cout << "Steam API initialized!\n";
        // You can now use ISteamUGC to upload mods

        int workshopID = 3545758025;
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

        // SteamAPI_Shutdown();
    } else {
        std::cerr << "Failed to initialize Steam API.\n";
    }
    return 0;
}
