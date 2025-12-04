#include "EntityComponent.h"

namespace Voxol::Scene
{
namespace Component
{
void updateUnitModel(UnitModel& model, const std::string typeName, bool hasImage)
{

    if (hasImage) {

        model.toDrawImage();

        return;
    }
    if (typeName == "rectangle")
    {
        model.toDrawRect();
    }
    else if (typeName == "round-rectangle")
    {
        model.toDrawRoundRect();
    }
    else if (typeName == "circle")
    {
        model.toDrawCircle();
    }
    else if (typeName == "text")
    {
        model.toDrawText();
    };
}
} // namespace Component
} // namespace Voxol::Scene