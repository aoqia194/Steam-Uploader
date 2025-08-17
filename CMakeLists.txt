cmake_minimum_required(VERSION 3.15)
project(SteamUploader)

set(CMAKE_CXX_STANDARD 17)

# Gather all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(app ${SOURCES})

# Platform-specific settings
if(WIN32)
    # Windows settings
    target_include_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/curl/include
        ${CMAKE_SOURCE_DIR} # for steam
    )
    target_link_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/SteamAPI/win64
        ${CMAKE_SOURCE_DIR}/curl/lib
    )
    target_link_libraries(app steam_api64 curl)
    # Copy DLLs after build (optional)
    add_custom_command(TARGET app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/SteamAPI/win64/steam_api64.dll
            $<TARGET_FILE_DIR:app>
    )
    add_custom_command(TARGET app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/curl/lib/libcurl-x64.dll
            $<TARGET_FILE_DIR:app>
    )
else()
    # Linux settings
    find_package(CURL REQUIRED)
    target_include_directories(app PRIVATE
        /usr/include/nlohmann
        ${CMAKE_SOURCE_DIR} # for steam
    )
    target_link_directories(app PRIVATE
        ${CMAKE_SOURCE_DIR}/SteamAPI/linux64
    )
    target_link_libraries(app steam_api curl pthread)
    target_link_options(app PRIVATE -Wl,-rpath,'${CMAKE_SOURCE_DIR}/SteamAPI/linux64')
endif()