#ifndef VOXOL_SCENE_DESCRIPTION_PARSER_H
#define VOXOL_SCENE_DESCRIPTION_PARSER_H

#include "../Base/BaseDefine.h"
#include "EntityComponent.h"
#include "../Math/Vec2.h"
#include <nlohmann/json.hpp>
#include <string>

namespace Voxol::Scene
{
namespace Desc
{
using JsonType = nlohmann::json;

struct SceneNode
{
    std::string            type;
    std::string            name;
    int                    index = 0;
    std::vector<SceneNode> children;

    int childrenTotal = 0;

    bool hasChild  = false;
    bool hasParent = true;

    void parse(const JsonType& jsonNode);
    void print();
};

class FileParser
{
public:
    FileParser()  = default;
    ~FileParser() = default;

public:
    SceneNode rootNode;

public:
    void initFromFile(const std::string& fileName);
    void parseHeriNodes(const JsonType& jsonNode);
    void parseSceneNode(SceneNode& parentNode, const JsonType& jsonNode);
};
} // namespace Desc

class DescriptionParser
{
public:
    DescriptionParser()  = default;
    ~DescriptionParser() = default;

public:
    Desc::FileParser fileParser;

public:
    void initialize();
};

} // namespace Voxol::Scene
#endif