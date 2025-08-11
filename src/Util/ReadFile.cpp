#include "ReadFile.h"

// Reads the entire contents of a text file at 'path' into a std::string.
// Returns an empty string if the file cannot be opened.
string readTxtFile(const string& path) {
    if (!exists(path)) {
        return "";
    }

    std::ifstream file(path);
    if (!file) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

