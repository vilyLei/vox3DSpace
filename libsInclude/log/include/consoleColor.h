#pragma once


#ifdef LogerDLL_EXPORTS
#    define LOGER_API _declspec(dllexport)
#else
#    define LOGER_API _declspec(dllimport)
#endif


#include <iostream>
#include <string>

namespace profile
{


enum ColorCode
{
    FG_RED     = 31,
    FG_GREEN   = 32,
    FG_BLUE    = 34,
    FG_DEFAULT = 39,
    BG_RED     = 41,
    BG_GREEN   = 42,
    BG_BLUE    = 44,
    BG_DEFAULT = 49
};
class LOGER_API ConsoleTextColor
{
    ColorCode code;
    int       ver;

public:
    ConsoleTextColor(ColorCode pCode);
    ~ConsoleTextColor() = default;
    void        showInfo();
    std::string getColorString();
    void        showVersion();
    int         getVersion();
    //friend std::ostream& operator<<(std::ostream& os, const ConsoleTextColor& mod);
};
} // namespace profile