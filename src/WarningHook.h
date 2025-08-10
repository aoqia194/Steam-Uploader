#pragma once
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdio>
#endif
#include "steam/steam_api.h"

void EnableWarningMessageHook();