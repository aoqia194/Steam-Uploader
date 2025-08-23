if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(steamworks_sdk_OS_ARCH "win64")
    set(steamworks_sdk_LIB_NAME "steam_api64")
    set(steamworks_sdk_LIB_EXT "dll")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(steamworks_sdk_OS_ARCH "linux64")
    set(steamworks_sdk_LIB_NAME "libsteam_api")
    set(steamworks_sdk_LIB_EXT "so")
else ()
    set(steamworks_sdk_OS_ARCH "osx")
    set(steamworks_sdk_LIB_NAME "libsteam_api")
    set(steamworks_sdk_LIB_EXT "dylib")
endif ()

CPMAddPackage(
        NAME steamworks_sdk
        URL https://partner.steamgames.com/downloads/steamworks_sdk_162.zip
        URL_HASH SHA256=8FF4D29DDDD225718164B9279DCFDE5EED1DB26FEE5DC49DCCA4906C7F1F0C1A
)
if (steamworks_sdk_ADDED)
    add_library(steamworks_sdk SHARED IMPORTED)

    set(steamworks_sdk_LIBDIR "${steamworks_sdk_SOURCE_DIR}/redistributable_bin/${steamworks_sdk_OS_ARCH}")
    set(steamworks_sdk_LIBFILE "${steamworks_sdk_LIBDIR}/${steamworks_sdk_LIB_NAME}.${steamworks_sdk_LIB_EXT}")
    set_target_properties(steamworks_sdk PROPERTIES
            IMPORTED_LOCATION ${steamworks_sdk_LIBFILE}
            INTERFACE_INCLUDE_DIRECTORIES "${steamworks_sdk_SOURCE_DIR}/public"
    )

    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set_target_properties(steamworks_sdk PROPERTIES
                IMPORTED_IMPLIB "${steamworks_sdk_LIBDIR}/${steamworks_sdk_LIB_NAME}.lib"
        )
    endif ()

    # Copy the library manually to the bin folder.
    # This is required for only steamworks sdk since it's an IMPORTED library.
#    add_custom_command(TARGET "${CMAKE_PROJECT_NAME}" POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            "${steamworks_sdk_LIBFILE}"
#            "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>"
#    )
endif ()
