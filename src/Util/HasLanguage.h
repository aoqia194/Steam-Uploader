#pragma once

#include <string>
#include <unordered_set>

inline std::unordered_set<std::string> valid_languages = {
    "arabic",
    "bulgarian",
    "schinese",
    "tchinese",
    "czech",
    "danish",
    "dutch",
    "english",
    "finnish",
    "french",
    "german",
    "greek",
    "hungarian",
    "indonesian",
    "italian",
    "japanese",
    "koreana",
    "norwegian",
    "polish",
    "portuguese",
    "brazilian",
    "romanian",
    "russian",
    "spanish",
    "latam",
    "swedish",
    "thai",
    "turkish",
    "ukrainian",
    "vietnamese",
};

inline bool IsValidSteamLanguageCode(const std::string &code)
{
    return valid_languages.contains(code);
}
