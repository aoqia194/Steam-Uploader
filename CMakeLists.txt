cmake_minimum_required(VERSION 3.30)

set(SteamUploader_NAME "SteamUploader")
set(SteamUploader_DESC "A CLI tool to update UGC on the Steam workshop")
set(SteamUploader_VERSION 0.6.0)

project(SteamUploader VERSION ${SteamUploader_VERSION} DESCRIPTION ${SteamUploader_DESC})

# Set the CPM cache to .cache/ if there isn't one already.
# Prevents downloading the same things over and over.
if (NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(ENV{CPM_SOURCE_CACHE} "${CMAKE_PROJECT_DIR}/.cache/CPM")
endif ()

if (WIN32)
    set(OS_ARCH "win64")
    set(cmake_OS_ARCH "win64")

    if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "ARM64")
        set(cmake_OS_ARCH "win64a")
    endif ()
elseif (APPLE)
    set(OS_ARCH "osx")
elseif (UNIX)
    set(OS_ARCH "linux64")
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

# CPM Dependencies

include(cmake/CPM.cmake)

include(cmake/cpm/spdlog.cmake)
include(cmake/cpm/nlohmann_json.cmake)
include(cmake/cpm/curl.cmake)
include(cmake/cpm/steamworks_sdk.cmake)
include(cmake/cpm/cxxopts.cmake)

# Adding sources manually gives much faster builds than globbing the entire source dir
add_executable(app ${SOURCES}
        src/main.cpp
        src/Uploader.cpp
        src/Uploader.h
        src/Enum/EItemUpdateStatus.h
        src/Enum/EResult.h
        src/Util/AppID.cpp
        src/Util/AppID.h
        src/Util/ExecCmd.h
        src/Util/HasLanguage.h
        src/Util/ReadFile.cpp
        src/Util/ReadFile.h
        src/Util/Updater.h
        src/Util/WarningHook.cpp
        src/Util/WarningHook.h
)
target_compile_features(app PRIVATE cxx_std_23)
target_compile_definitions(app PRIVATE
        PROJECT_NAME="${SteamUploader_NAME}"
        PROJECT_DESCRIPTION="${SteamUploader_DESC}"
        PROJECT_VERSION="${SteamUploader_VERSION}")

target_link_libraries(app PRIVATE
        steamworks_sdk
        nlohmann_json
        CURL::libcurl
        cxxopts::cxxopts
        spdlog::spdlog)
