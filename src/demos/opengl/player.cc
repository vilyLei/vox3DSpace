#include "player.h"
namespace player
{
OGLPlayer::OGLPlayer()
{
    std::cout << "OGLPlayer::constructor()...\n";
}
OGLPlayer::~OGLPlayer()
{
    std::cout << "OGLPlayer::deconstructor()...\n";
}
void OGLPlayer::showInfo()
{
    std::cout << "OGLPlayer::showInfo()...\n";
}
} // namespace player