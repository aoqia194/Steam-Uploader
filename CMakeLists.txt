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
    # We don't need to configure curl, only want the include files!
    file(GLOB curl_sources "${curl_SOURCE_DIR}/include/curl/*.h")
    add_library(curl STATIC ${curl_sources})
    set_target_properties(curl PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(curl PUBLIC $<BUILD_INTERFACE:${curl_SOURCE_DIR}/include>)
endif ()

CPMAddPackage(URI "gh:nlohmann/json@3.12.0"
        OPTIONS "JSON_BuildTests OFF")

CPMAddPackage("gh:gabime/spdlog@1.15.3")

CPMAddPackage(
        URI "gh:jarro2783/cxxopts@3.3.1"
        OPTIONS
        "CXXOPTS_BUILD_EXAMPLES NO"
        "CXXOPTS_BUILD_TESTS NO"
        "CXXOPTS_ENABLE_INSTALL YES")

target_link_libraries(app PRIVATE
        curl
        nlohmann_json::nlohmann_json
        cxxopts::cxxopts
        spdlog::spdlog)

# Includes and libs

target_include_directories(app PRIVATE include)
target_link_libraries(app PRIVATE lib)
