#pragma once

// steam API
#include "steam/steam_api.h"

// random tools
#include "ExecCmd.h"

// other
#include <fstream>
#include <string>


void setAppID(AppId_t appid);
AppId_t getAppID(PublishedFileId_t publishedfileid);