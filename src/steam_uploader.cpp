#include "steam/steam_api.h"

#include "Util/Updater.h"

#include "Uploader.h"

#include "cxxopts.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <string>

#define PROJECT_NAME "Steam-Uploader"
#define PROJECT_DESCRIPTION "A CLI tool to update UGC on the Steam workshop"
// PROJECT_VERSION defined in CMake!

#define LOGGER_PATTERN "[%Y-%m-%d %H:%M:%S] [%^%l%$] %v"
#define BACKTRACE_SIZE 5

// Implements this:
// https://partner.steamgames.com/doc/features/workshop/implementation#uploading_a_workshop_item

template <typename... Args>
bool anyHasValue(const Args&... args) {
    return (... || args.has_value());
}

int main(const int argc, const char *argv[])
{
    spdlog::set_default_logger(spdlog::stdout_color_mt(PROJECT_NAME));
    spdlog::set_pattern(LOGGER_PATTERN);
    spdlog::enable_backtrace(BACKTRACE_SIZE);
    spdlog::set_level(spdlog::level::info);

    cxxopts::Options options(PROJECT_NAME, PROJECT_DESCRIPTION);
    options.add_options()
        ("h,help", "Shows help message with program usage.")
        ("verbose", "Enables verbose logging.")
        ("U,update", "Force update SteamUploader to the latest version.");
    options.add_options("UGC")
        ("a,appID", "App ID of the game", cxxopts::value<AppId_t>())
        ("w,workshopID", "Workshop ID of the item. Overwritten by --new", cxxopts::value<PublishedFileId_t>());
    options.add_options("Uploader")
        (
            "d,description",
            "Path to a text file which contains the description for the UGC item. Follows the BBCode markdown format.",
            cxxopts::value<std::filesystem::path>()
        )
        (
            "p,preview",
            "Path to the preview image or gif. Suggest using JPG/PNG/GIF. Possibly no limitations to image dimensions with a maximum file size of 1 MB.",
            cxxopts::value<std::filesystem::path>()
        )
        (
            "c,content",
            "Path to the folder containing the content to upload.",
            cxxopts::value<std::filesystem::path>()
        )
        ("t,title", "Title of the UGC item.")
        (
            "v,visibility",
            "Visibility of the item on the workshop. Defaults to public visibility. [0 = public, 1 = friends-only, 2 = private/hidden, 3 = unlisted]",
            cxxopts::value<int8_t>()->default_value("-1")
        )
        (
            "T,tags",
            "A comma-separated list of tags for the UGC item, or empty to remove all tags. Predefined tags are set by the developers of the game and thus only those should be used.",
            cxxopts::value<std::vector<std::string>>()
        )
        (
            "n,new",
            "Creates a new UGC item (along with a new workshop ID). This will result in an empty workshop item without any details on the workshop page."
        )
        (
            "P,patchNote",
            "[OPTIONAL] Path to a text file which contains the patch notes for the update. Only needed if you upload new content. Follows the BBCode markdown format.",
            cxxopts::value<std::filesystem::path>()
        )
        (
            "L,language",
            "[OPTIONAL] Target language code for the description and title of this specific upload. Defaults to 'english'. See more: https://partner.steamgames.com/doc/store/localization/languages",
            cxxopts::value<std::string>()->default_value("english")
        );
    const auto opts = options.parse(argc, argv);

    if (opts.count("help")) {
        spdlog::info(options.help());
        exit(0);
    }

    if (opts.count("verbose")) {
        spdlog::set_level(spdlog::level::trace);
    }

    if (opts.count("update") > 0) {
        perform_update();
        return 0;
    } else {
        const std::string latest_version = fetch_latest_version();
        if (!latest_version.empty() && latest_version != PROJECT_VERSION) {
            spdlog::warn("A new version ({}) is available!", latest_version);
            spdlog::warn("Run with -U/--update to update.");
        }
    }

    const auto appID = opts["appID"].as<AppId_t>();
    const auto workshopID = opts["workshopID"].as<PublishedFileId_t>();

    const auto descriptionPath = opts["description"].as_optional<std::filesystem::path>();
    const auto previewPath = opts["preview"].as_optional<std::filesystem::path>();
    const auto contentPath = opts["content"].as_optional<std::filesystem::path>();
    const auto title = opts["title"].as_optional<std::string>();
    const auto visibility = opts["visibility"].as_optional<ERemoteStoragePublishedFileVisibility>();
    const auto tags = opts["tags"].as_optional<std::vector<std::string>>();

    const auto isNewUGC = opts.count("new") > 0;
    const auto patchNotePath = opts["patchNote"].as_optional<std::filesystem::path>();
    const auto language = opts["language"].as<std::string>();

    // If at least one required param doesn't exist, raise an error and exit.
    if (!anyHasValue(descriptionPath, previewPath, contentPath, title, visibility, tags)) {
        if (isNewUGC) {
            const Uploader uploader(workshopID, appID, isNewUGC);
            return 0;
        }

        spdlog::error("All required arguments are empty -- there is nothing to upload.");
        return 1;
    }

    // Upload the item.
    Uploader uploader(workshopID, appID, isNewUGC);
    return uploader.UpdateItem(descriptionPath, previewPath, contentPath, title, visibility, tags, patchNotePath, language);
}
