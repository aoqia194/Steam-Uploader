#include "AppID.h"

// source: https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
// updates the steam_appid.txt file which the steam API reads to know the appID to use
void setAppID(AppId_t appid) {
	printf("Writing appid to steam_appid.txt\n");
    std::ofstream myfile;
    myfile.open("steam_appid.txt", std::ios::out | std::ios::trunc);
	myfile << appid;
	myfile.close();
}





// This method has flaws, when the item gets hidden or friend only once, then the appID can't be retrieved, even if you set it back to visible (0)
// thus I kept it bcs it's quite a nice tool but by default it's prefered to give the appID directly

// source: https://github.com/TechnologicNick/SteamChangePreview/blob/master/SteamChangePreview/SteamChangePreview.cpp
#ifdef _WIN32
// used to retrieve the appID for a specific workshopID
AppId_t getAppID(PublishedFileId_t publishedfileid) {
    std::wstring str(L"powershell -c \"[regex]::Matches((Invoke-WebRequest -Uri \\\"https://steamcommunity.com/sharedfiles/filedetails/?id=");
    str += std::to_wstring(publishedfileid);
    str += std::wstring(L"\\\" ).Content, 'data-appid=\\\"(\\d+?)\\\">').Groups[1].Value\"");
    auto out = ExecCmd(str.c_str());
    return strtoul(out, NULL, 10);
}
#else
// Linux equivalent
AppId_t getAppID(PublishedFileId_t publishedfileid) {
    std::string str = "curl -s 'https://steamcommunity.com/sharedfiles/filedetails/?id=";
    str += std::to_string(publishedfileid);
    str += "' | grep -o 'data-appid=\"[0-9]*' | grep -o '[0-9]*'";
    std::string out = ExecCmd(str.c_str());
    return strtoul(out.c_str(), NULL, 10);
}
#endif