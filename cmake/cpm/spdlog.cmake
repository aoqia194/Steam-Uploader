CPMAddPackage(
        URI "gh:gabime/spdlog@1.15.3"
        OPTIONS
        "SPDLOG_BUILD_EXAMPLE OFF"
)

# Disable dllexport warning (it's annoying)
target_compile_options(spdlog PRIVATE -Wno-ignored-attributes)
