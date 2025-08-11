#pragma once


#ifdef _WIN32
#include <windows.h>
#include <string>
#include <algorithm>
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
inline std::string ExecCmd(const char* cmd)
{
	std::string result;
	HANDLE hRead, hWrite;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) return result;

	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(STARTUPINFOA);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = hWrite;
	si.hStdError = hWrite;
	si.wShowWindow = SW_HIDE;

	std::string cmdline = "cmd.exe /C ";
	cmdline += cmd;

	BOOL success = CreateProcessA(
		NULL,
		&cmdline[0],
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi
	);

	CloseHandle(hWrite);

	if (!success) {
		CloseHandle(hRead);
		return result;
	}

	char buffer[1024];
	DWORD bytesRead;
	while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = 0;
		result += buffer;
	}

	CloseHandle(hRead);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	// Remove trailing CRLF for consistency with Linux version
	result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());

	return result;
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