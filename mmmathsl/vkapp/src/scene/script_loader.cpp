#include "script_loader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace vkapp {

std::string ScriptLoader::load(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open script file: " + path);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::filesystem::file_time_type ScriptLoader::lastModified(const std::string& path) {
    if (!exists(path)) {
        throw std::runtime_error("Script file does not exist: " + path);
    }
    return std::filesystem::last_write_time(path);
}

bool ScriptLoader::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

} // namespace vkapp
