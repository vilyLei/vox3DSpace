#include "SceneIRParser.h"
#include <fstream>
#include <filesystem>
#include <cctype>

namespace Voxol::Scene
{

void SceneIRParser::parseFromFile(const std::string& fileName)
{

    auto           pngPath = std::filesystem::path(SRC_DIR) / "assets/";
    auto           pathStr = pngPath.string() + fileName;
    std::ifstream  fs(pathStr);
    JsonType jo;
    fs >> jo;
    parse(jo);
}
void SceneIRParser::parse(const JsonType& json)
{
    if (json.contains("shadering"))
    {
        shaderingModule.parse(json["shadering"]);
    }
    if (json.contains("scene"))
    {
        sceneModule.parse(json["scene"]);
    }
}
} // namespace Voxol::Scene