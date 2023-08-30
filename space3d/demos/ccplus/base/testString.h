#include <iostream>
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <random>
#include <regex>
#include <compare>
//#include <cctype>
//#include <locale>
namespace base
{
namespace testString
{
namespace teststr_3
{
void testMain()
{
    std::cout << "teststr_3::testMain() begin.\n";
    char  input[100] = "A bird came down the walk";
    char* token      = std::strtok(input, " ");
    while (token != NULL)
    {
        std::cout << token << '\n';
        token = std::strtok(NULL, " ");
    }
    std::cout << "teststr_3::testMain() end.\n";
}
} // namespace teststr_3
namespace teststr_2
{
namespace str_utils
{
// trim from start (in place)
static inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
            s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s)
{
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s)
{
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s)
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s)
{
    trim(s);
    return s;
}
std::vector<std::string> split(const std::string& str, const std::string& delims = " ")
{
    std::vector<std::string> output;
    auto                     first = std::cbegin(str);

    while (first != std::cend(str))
    {
        const auto second = std::find_first_of(first, std::cend(str),
                                               std::cbegin(delims), std::cend(delims));

        if (first != second)
            output.emplace_back(first, second);

        if (second == std::cend(str))
            break;

        first = std::next(second);
    }

    return output;
}
// thanks: https://www.cppstories.com/2018/07/string-view-perf-followup/
std::vector<std::string_view> splitSV(std::string_view strv, std::string_view delims = " ")
{
    std::vector<std::string_view> output;
    size_t                        first = 0;

    while (first < strv.size())
    {
        const auto second = strv.find_first_of(delims, first);

        if (first != second)
            output.emplace_back(strv.substr(first, second - first));

        if (second == std::string_view::npos)
            break;

        first = second + 1;
    }

    return output;
}
} // namespace str_utils
void main02()
{
    std::string                text = "aaa bbb ccc"s;
    std::regex                 rgx("\\s+");
    std::sregex_token_iterator iter(text.begin(),
                                    text.end(),
                                    rgx,
                                    -1);
    std::sregex_token_iterator end;
    for (; iter != end; ++iter)
        std::cout << "text segment: " << *iter << '\n';

    std::cout << "------------- --------------------- ---------------\n";
    std::string str_line{"String to split here, and here, and here yes,...,end"};
    //std::regex                 regex{R"([\s,]+)"}; // split on space and comma
    std::regex                 regex{R"([\,]+)"}; // split comma
    std::sregex_token_iterator it{str_line.begin(), str_line.end(), regex, -1};
    std::vector<std::string>   words{it, {}};

    for (auto word : words)
        std::cout << "A word: " << word << '\n';

    auto words2 = str_utils::splitSV(str_line, ",");
    for (auto word : words2)
        std::cout << "B word: " << word << '\n';

    std::cout << "------------- ----------- vv ---------- ---------------\n";
    auto trimStr = "  abc "s;
    auto strs    = str_utils::splitSV(trimStr, " ");
    for (auto str : strs)
        std::cout << "str: " << str << "\n";
    trimStr = "  ab c "s;
    str_utils::trim(trimStr);
    std::cout << "trimStr: (" << trimStr << ")\n";
}
std::vector<std::string> parseParams(std::string text)
{
    auto                     temp_params = str_utils::splitSV(text, ",");
    std::vector<std::string> params;
    for (auto temp_param : temp_params)
    {
        std::cout << "temp_param: " << temp_param << "\n";
        params.emplace_back(str_utils::trim_copy(std::string(temp_param)));
    }
    return params;
}

std::vector<std::vector<std::string>> parseParamText(const std::string& datastr)
{
    std::vector<std::vector<std::string>> blocks;
    size_t                                index = 0;
    for (auto t = 0; t < 100; ++t)
    {
        auto i = datastr.find("[", index);
        if (std::string::npos == i)
        {
            break;
        }
        auto j = datastr.find("]", i + 1);
        std::cout << ">> i: " << i << ", j: " << j << "\n";
        if (j > 0 && j > index)
        {
            index = j + 1;
            std::cout << "next pos index: " << index << "\n";
            auto substr = datastr.substr(i + 1, j - i - 1);
            std::cout << "substr: " << substr << "\n";
            auto params = parseParams(substr);
            //for (auto param : params)
            //{
            //	std::cout << ">>> param: " << param << "\n";
            //}
            blocks.emplace_back(params);
        }
        else
        {
            break;
        }
    }
    return blocks;
}

void testParseStr02()
{
    const char* strdatasrc = "[module01, code/01/src, 101][module02, code/02/src, 201][module03, code/03/src, 301]";
    //strdatasrc             = "[module01, code/01/src, 101]";
    auto paramBlocks = parseParamText(strdatasrc);
    std::cout << "paramBlocks.size(): " << paramBlocks.size() << "\n";
}
void testParseStr01()
{
    const char* strdatasrc = "[module01, code/01/src, 101][module02, code/02/src, 201][module03, code/03/src, 301]";
    std::string datastr(strdatasrc);
    size_t      index = 0;
    for (auto t = 0; t < 100; ++t)
    {
        auto i = datastr.find("[", index);
        if (std::string::npos == i)
        {
            break;
        }
        auto j = datastr.find("]", i + 1);
        std::cout << ">> i: " << i << ", j: " << j << "\n";
        if (j > 0 && j > index)
        {
            index = j + 1;
            std::cout << "next pos index: " << index << "\n";
            auto substr = datastr.substr(i + 1, j - i - 1);
            std::cout << "substr: " << substr << "\n";
        }
        else
        {
            break;
        }
    }
}
void testMain()
{
    std::cout << "base::testString::teststr_2::testMain() begin.\n";

    testParseStr02();
    //testParseStr01();
    //main02();
    return;


    std::cout << "base::testString::teststr_2::testMain() end.\n";
}
} // namespace teststr_2
namespace teststr_1
{
void testDistributeRandom()
{
    std::random_device rd;
    std::mt19937       gen(rd());
    //std::uniform_int_distribution<> distribute(1, 6);
    std::uniform_real_distribution<double> distribute(0.5, 1.5);
    std::cout << "distribute(gen): ";
    for (int i = 0; i < 10; ++i)
    {
        std::cout << distribute(gen) << " ";
    }
    std::cout << std::endl;
}

class Singal
{
    std::string& idns;

public:
    ~Singal()
    {
        std::cout << "Singal::deconstrutor(), idns: " << idns << std::endl;
    }
    Singal(std::string& idns) :
        idns(idns)
    {
        std::cout << "Singal::construtor(), idns: " << idns << std::endl;
    }
};
struct SExampleA
{
    struct addr_t
    {
        uint32_t port;
    } addr;
    union
    {
        uint8_t  a8[4];
        uint16_t a16[2];
    } in_u;
    operator std::string() const
    {
        std::string s("SExampleA(\naddr.port=" + std::to_string(addr.port));
        s += "\n\t,in_u.a8=[" + std::to_string(in_u.a8[0]) + "," + std::to_string(in_u.a8[1]) + "," + std::to_string(in_u.a8[2]) + "," + std::to_string(in_u.a8[3]) + "]";
        s += "\n\t,in_u.a16=[" + std::to_string(in_u.a16[0]) + "," + std::to_string(in_u.a16[1]) + "]";
        s += "\n\t)";
        return std::move(s);
    }
};

struct SExampleB
{
    struct addr_t
    {
        uint32_t port;
    } addr;
    union
    {
        uint16_t a16[2];
        uint8_t  a8[4];
    } in_u;
    operator std::string() const
    {
        std::string s("SExampleB(\naddr.port=" + std::to_string(addr.port));
        s += "\n\t,in_u.a16=[" + std::to_string(in_u.a16[0]) + "," + std::to_string(in_u.a16[1]) + "]";
        s += "\n\t,in_u.a8=[" + std::to_string(in_u.a8[0]) + "," + std::to_string(in_u.a8[1]) + "," + std::to_string(in_u.a8[2]) + "," + std::to_string(in_u.a8[3]) + "]";
        s += "\n\t)";
        return std::move(s);
    }
};
struct VPoint
{
    double vx, vy, vz;
           operator std::string() const
    {
        std::string s("VPoint(vx=" + std::to_string(vx));
        s += ",vy=" + std::to_string(vy);
        s += ",vz=" + std::to_string(vz) + ")";
        return std::move(s);
    }
};

void testMain()
{
    std::string samephore_001 = "samephore-001";
    Singal      sg(samephore_001);
    VPoint      v0  = {.vx = 0.5};
    std::string str = "str";
    str.insert(0, "First");
    std::cout << "str: " << str << std::endl;
    std::string vo_str = v0;
    std::cout << "struct VPoint object: \n\t" << vo_str << std::endl;

    //struct SExampleA ex = {// start of initializer list for struct example
    SExampleA ex0 = {// start of initializer list for struct example
                     {
                         // start of initializer list for ex.addr
                         80 // initialized struct's only member
                     },     // end of initializer list for ex.addr
                     {
                         // start of initializer-list for ex.in_u
                         {127, 0, 0, 1} // initializes first element of the union
                     }};

    std::cout << "struct SExampleA ex0: \n\t" << std::string(ex0) << std::endl;
    SExampleA ex1 = {80, 127, 0, 0, 1}; // 80 initializes ex.addr.port
                                        // 127 initializes ex.in_u.a8[0]
                                        // 0 initializes ex.in_u.a8[1]
                                        // 0 initializes ex.in_u.a8[2]
                                        // 1 initializes ex.in_u.a8[3]
    std::cout << "struct SExampleA ex1: \n\t" << std::string(ex1) << std::endl;

#pragma warning(disable : 4838)
    SExampleA ex2 = {80, 257, 0};
    std::cout << "struct SExampleA ex2: \n\t" << std::string(ex2) << std::endl;
    SExampleB ex3 = {80, 515, 769};
    std::cout << "struct SExampleB ex3: \n\t" << std::string(ex3) << std::endl;

#pragma warning(default : 4838)
    //SExampleA ex4 = {8101, (char)257, 0};
    SExampleA ex4 = {8101, 257, 0};
    std::cout << "struct SExampleA ex4: \n\t" << std::string(ex4) << std::endl;
}
struct Struct_S
{
    int         n = 7;
    std::string s{'a', 'b', 'c'};
    std::string s2{"astringtestdo"};
    Struct_S() {} // default member initializer will copy-initialize n, list-initialize s
};
class MyClassA01
{
public:
    MyClassA01()  = default;
    ~MyClassA01() = default;
    int         n = 7;
    std::string s{'a', 'b', 'c'};
    std::string s2{"astringtestdo"};


private:
};
void print(std::weak_ordering value)
{
    value<0 ? std::cout << "less\n" :
              value> 0 ?
        std::cout << "greater\n" :
        std::cout << "equal\n";
}
} // namespace teststr_1

int testMain()
{

    std::cout << "\nbase::testString::testMain() begin.\n";
    std::string str        = "afasfasf*123+9"s;
    auto        fa         = std::find(str.begin(), str.end(), '*');
    auto        fb         = std::find(str.begin(), str.end(), '+');
    auto        beginValue = std::stoul(std::string{fa + 1, fb});
    std::string subStr{fa + 1, fb};
    //std::cout << "beginValue: " << beginValue << "\n";
    std::cout << "subStr: " << subStr << "\n";
    int        value0 = 10;
    int        value1 = 11;
    const int* p      = &value0;
    int* const q(&value0);
    // error ...
    // *p = 11;// p 变量不可修改值,可修改指针
    *q = 11;
    p  = &value1;
    // error ...
    //q  = &value1;// q 变量可修改值， 不可修改指针
    //teststr_1::testMain();
    //teststr_1::testDistributeRandom();
    teststr_2::testMain();
    teststr_3::testMain();
    std::cout << "base::testString::testMain() end.\n";
    return EXIT_SUCCESS;
}
} // namespace testString

} // namespace base