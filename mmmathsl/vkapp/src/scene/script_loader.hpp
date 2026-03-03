#pragma once

#include <string>
#include <filesystem>

namespace vkapp {

// Utility class for loading mmrsl script files from disk
// and monitoring file modification times for hot-reload.
class ScriptLoader {
public:
    // Read entire file content into string.
    // Throws std::runtime_error if file cannot be opened or read.
    static std::string load(const std::string& path);

    // Return last write time of the file.
    // Throws std::runtime_error if file does not exist.
    static std::filesystem::file_time_type lastModified(const std::string& path);

    // Check if file exists.
    static bool exists(const std::string& path);
};

} // namespace vkapp
