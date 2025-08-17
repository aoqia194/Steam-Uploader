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

    if (OS_ARCH STREQUAL "win64")
        set(steamworks_sdk_LIB_NAME "steam_api64.lib")
    elseif (OS_ARCH STREQUAL "osx")
        set(steamworks_sdk_LIB_NAME "libsteam_api.dylib")
    else ()
        set(steamworks_sdk_LIB_NAME "libsteam_api.so")
    endif ()

    target_link_libraries(steamworks_sdk INTERFACE "${steamworks_sdk_SOURCE_DIR}/redistributable_bin/${OS_ARCH}/${steamworks_sdk_LIB_NAME}")
endif ()
