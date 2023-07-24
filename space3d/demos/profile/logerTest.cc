
#include <iostream>
#include "profile/log/color/consoleColor.h"
using namespace profile;

//__declspec(dllimport) void myFunc(std::string param);
//__declspec(dllimport) std::ostream& operator<<(std::ostream& os, const ConsoleTextColor& mod);
int main(int argc, char** argv)
{
    std::cout << "logerTest proj init 02...\n";
    ConsoleTextColor red(ColorCode::FG_RED);
    ConsoleTextColor green(ColorCode::FG_GREEN);
    ConsoleTextColor defaultColor(ColorCode::FG_DEFAULT);
    //green.showVersion();
    green.showInfo();
    auto ver = green.getVersion();
    std::cout << "logerTest proj version: " << ver << std::endl;
    auto greenColorCmdStr = green.getColorString();
    auto defaultColorCmdStr = defaultColor.getColorString();
    std::cout << greenColorCmdStr << "rendererTest color text ...\n";
    std::cout << defaultColorCmdStr;
    //std::cout << red << "rendererTest proj init ...\n"
    //          << defaultColor;

    std::cout << "logerTest proj end 01...\n";
    return EXIT_SUCCESS;
}