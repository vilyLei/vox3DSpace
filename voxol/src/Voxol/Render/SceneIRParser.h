#ifndef VOXOL_RENDER_SCENE_IR_PARSER_H
#define VOXOL_RENDER_SCENE_IR_PARSER_H

#include "../Base/BaseDefine.h"

namespace Voxol::Render
{
class SceneIRParser
{
public:
    SceneIRParser()  = default;
    ~SceneIRParser() = default;

public:
    void parse();
};

} // namespace Voxol::Render
#endif