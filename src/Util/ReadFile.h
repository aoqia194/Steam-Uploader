#pragma once

#include <fstream>
#include <sstream>
#include <string>
using std::string;
#include <filesystem>
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

string readTxtFile(const string& path);