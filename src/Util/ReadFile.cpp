#include "ReadFile.h"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <fstream>

//! Reads the entire contents of a text file at path into a string.
//! @return empty string if the file cannot be opened
std::string readTxtFile(const std::filesystem::path &path)
{
    const std::ifstream file(path);
    if (!file.is_open() || !file.good()) {
        spdlog::error("Failed to open text file at path {}", path.string());
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

