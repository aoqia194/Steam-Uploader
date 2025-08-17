cmake_minimum_required(VERSION 3.30)

set(SteamUploader_VERSION 0.6.0)

project(SteamUploader VERSION ${SteamUploader_VERSION})

# Set the CPM cache to .cache/ if there isn't one already.
# Prevents downloading the same things over and over.
if (NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(ENV{CPM_SOURCE_CACHE} "${CMAKE_PROJECT_DIR}/.cache/CPM")
endif ()

# Adding sources manually gives much faster builds than globbing the entire source dir
add_executable(app ${SOURCES}
        src/steam_uploader.cpp
        src/Uploader.cpp
        src/Util/AppID.cpp
        src/Util/ReadFile.cpp
        src/Util/WarningHook.cpp)
target_compile_features(app PRIVATE cxx_std_23)
target_compile_definitions(app PRIVATE PROJECT_VERSION="${SteamUploader_VERSION}")

# CPM Dependencies

include(cmake/CPM.cmake)

# Set up CURL manually, since we only need the include/ files.
CPMAddPackage(
        NAME curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG "curl-8_15_0"
        OPTIONS "BUILD_EXAMPLES OFF" "CURL_BUILD_TESTING OFF"
        DOWNLOAD_ONLY YES)
if (curl_ADDED)
    add_library(curl INTERFACE)
    target_compile_definitions(curl INTERFACE CURL_HEADER_ONLY)
    target_include_directories(curl INTERFACE "${curl_SOURCE_DIR}/include")
endif ()

# Set up Steamworks SDK. We can download it directly from the website.
CPMAddPackage(
        NAME steamworks_sdk
        URL https://partner.steamgames.com/downloads/steamworks_sdk_162.zip
        URL_HASH SHA256=8FF4D29DDDD225718164B9279DCFDE5EED1DB26FEE5DC49DCCA4906C7F1F0C1A
)
if (steamworks_sdk_ADDED)
    add_library(steamworks_sdk INTERFACE IMPORTED)
    target_compile_definitions(steamworks_sdk INTERFACE STEAMWORKS_SDK_HEADER_ONLY)
    target_include_directories(steamworks_sdk INTERFACE "${steamworks_sdk_SOURCE_DIR}/public")
    target_link_directories(steamworks_sdk INTERFACE "${steamworks_sdk_SOURCE_DIR}/public/steam/lib")
endif ()

# Set up nlohmann/json manually, since the git repo is MASSIVE and we only need the included headers which is a separate download.
CPMAddPackage(
        NAME nlohmann_json
        VERSION 3.12.0
        URL "https://github.com/nlohmann/json/releases/download/v3.12.0/include.zip"
        URL_HASH SHA256=b8cb0ef2dd7f57f18933997c9934bb1fa962594f701cd5a8d3c2c80541559372
)
if (nlohmann_json_ADDED)
    add_library(nlohmann_json INTERFACE IMPORTED)
    target_include_directories(nlohmann_json INTERFACE ${nlohmann_json_SOURCE_DIR}/include)
endif()

CPMAddPackage("gh:gabime/spdlog@1.15.3")

CPMAddPackage(
        URI "gh:jarro2783/cxxopts@3.3.1"
        OPTIONS
        "CXXOPTS_BUILD_EXAMPLES NO"
        "CXXOPTS_BUILD_TESTS NO"
        "CXXOPTS_ENABLE_INSTALL YES")

target_link_libraries(app PRIVATE
        curl
        steamworks_sdk
        nlohmann_json
        cxxopts::cxxopts
        spdlog::spdlog)

# Includes and libs

target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE lib)
