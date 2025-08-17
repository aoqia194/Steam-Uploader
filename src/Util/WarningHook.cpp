#include "WarningHook.h"

#include "spdlog/spdlog.h"

// Doesn't seem to output anything, probably because the code isn't triggering any warning or errors ?

extern "C" void SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
#ifdef _WIN32
	::OutputDebugStringA(pchDebugText);
#else
	fprintf(stderr, "%s", pchDebugText);
#endif
	if (nSeverity >= 1)
	{
		// place to set a breakpoint for catching API errors
		int x = 3;
		x = x;
	}
}

void EnableWarningMessageHook()
{
	SteamUtils()->SetWarningMessageHook( &SteamAPIDebugTextHook );
    spdlog::debug("Warning message hook enabled.\n");
}