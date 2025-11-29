#include "EntityComponent.h"

namespace Voxol::Scene
{
namespace Component
{
void updateUunitModel(UnitModel& model, const std::string typeName)
{
    if (typeName == "rectangle")
    {
        model.drawUnitId = 0;
        model.type       = Component::UnitModelType::Mesh;
    }
    else if (typeName == "round-rectangle")
    {
        model.drawUnitId = 5;
        model.type       = Component::UnitModelType::Mesh;
    }
    else if (typeName == "circle")
    {
        model.drawUnitId = 1;
        model.type       = Component::UnitModelType::Mesh;
    }
    else if (typeName == "text")
    {
        model.drawUnitId = 8;
        model.type       = Component::UnitModelType::Text;
    };
}
} // namespace Component
} // namespace Voxol::Scene