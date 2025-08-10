#pragma once


#ifdef _WIN32
#include <windows.h>
#include <atlstr.h>
#else
#include <cstdio>
#include <cstring>
#include <string>
#endif

/**
 * Author: TarmoPikaro
 * URL: https://stackoverflow.com/a/35658917/12586927
 * License: https://creativecommons.org/licenses/by-sa/4.0/
 * Retrieved from https://github.com/TechnologicNick/SteamChangePreview
 * Modified to fit both Windows and Linux supposedly, not fully tested for now
 */

 
// Execute a command and get the results. (Only standard output)
#ifdef _WIN32
inline CStringA ExecCmd(const wchar_t* cmd)
{
	CStringA strResult;
	HANDLE hPipeRead, hPipeWrite;

	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
		return strResult;

	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi = { 0 };

	BOOL fSuccess = CreateProcessW(NULL, (LPWSTR)cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!fSuccess)
	{
		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);
		return strResult;
	}

	bool bProcessEnded = false;
	for (; !bProcessEnded;)
	{
		bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;
		for (;;)
		{
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;

			if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
				break;
			if (!dwAvail)
				break;
			if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				break;
			buf[dwRead] = 0;
			strResult += buf;
		}
	}

	CloseHandle(hPipeWrite);
	CloseHandle(hPipeRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return strResult;
}
#else
// Linux/Unix version
inline std::string ExecCmd(const char* cmd)
{
	std::string result;
	FILE* pipe = popen(cmd, "r");
	if (!pipe) return result;
	char buffer[1024];
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}
	pclose(pipe);
	return result;
}
#endif