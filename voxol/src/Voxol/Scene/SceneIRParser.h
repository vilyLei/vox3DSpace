#ifndef VOXOL_SCENE_SCENE_IR_PARSER_H
#define VOXOL_SCENE_SCENE_IR_PARSER_H

#include "../Base/BaseDefine.h"
#include "SceneIRComponent.h"
#include "../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>
#include <algorithm>
#include <variant>

namespace Voxol::Scene
{
using JsonType = nlohmann::json;

class SceneIRParser
{
public:
    SceneIRParser()  = default;
    ~SceneIRParser() = default;

public:
    SceneIR::Shadering::Module shaderingModule;
    SceneIR::Scene::Module     sceneModule;

public:
    void parseFromFile(const std::string& fileName);
    void parse(const JsonType& json);
};

} // namespace Voxol::Scene
#endif