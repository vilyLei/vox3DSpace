
#include "../def/pch.h"
#include "consoleColor.h"

namespace profile
{
ConsoleTextColor::ConsoleTextColor(ColorCode pCode) :
    code(pCode), ver(1002) {}

//std::ostream& operator<<(std::ostream& os, const ConsoleTextColor& mod)
//{
//    return os << "\033[" << mod.code << "m";
//}

std::string ConsoleTextColor::getColorString()
{
    return "\033[" + std::to_string(code) + "m";
}
int ConsoleTextColor::getVersion()
{
    return 1001;
}
void ConsoleTextColor::showVersion()
{
    auto str = "\033[" + std::to_string(101) + "m";
    std::cout << "version: " << str << std::endl;
    //std::cout << "version: "<< "101" << std::endl;
}
void ConsoleTextColor::showInfo()
{
    std::cout << "ConsoleTextColor::showInfo() v01." << std::endl;
}
} // namespace profile