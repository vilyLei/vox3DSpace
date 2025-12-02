#include "ActionSystem.h"
namespace Voxol::Intent
{

ActionSystem::SP ActionSystem::make()
{
    auto sp = std::make_shared<ActionSystem>();
    return sp;
}

void ActionSystem::initialize() {

}
void ActionSystem::updateAction(const Scene::Component::UnitLocation& location){

}
void ActionSystem::update(){

}

} // namespace Voxol::Intent