#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <iomanip>
#include <codecvt>
#include <cstdint>
namespace coding::tcodecvr
{
namespace tcodecvr_t01
{
// 工具包装器，用于为 wstring/wbuffer 适配绑定到本地环境的平面
template <class Facet> struct deletable_facet : Facet
{
    template <class... Args>
    deletable_facet(Args&&... args) :
        Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

void testMain()
{
    std::cout << "tcodecvr_t01::testMain(), begin ...\n";
    // UTF-8 窄多字节编码
    std::string data = reinterpret_cast<const char*>(+u8"z\u00df\u6c34\U0001f34c");
    // 或 reinterpret_cast<const char*>(+u8"zß水🍌")
    // 或 "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9f\x8d\x8c"

    std::ofstream("text.txt") << data;

    // 使用系统提供的本地环境的 codecvt 平面
    std::wifstream fin("text.txt");
    // 从 wifstream 的读取将使用 codecvt<wchar_t, char, std::mbstate_t>
    // 此本地环境的 codecvt 从 UTF-8 转换到 UCS4（在如 Linux 的系统上）
    fin.imbue(std::locale("en_US.UTF-8"));
    std::cout << "此 UTF-8 文件包含以下 UCS4 编码单元：\n";
    for (wchar_t c; fin >> c;)
        std::cout << "U+" << std::hex << std::setw(4) << std::setfill('0')
                  << static_cast<uint32_t>(c) << ' ';

    // 使用标准（本地环境无关）codecvt 平面
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t>
                   conv16;
    std::u16string str16 = conv16.from_bytes(data);

    std::cout << "\n\n此 UTF-8 文件包含以下 UTF-16 编码单元：\n";
    for (char16_t c : str16)
        std::cout << "U+" << std::hex << std::setw(4) << std::setfill('0')
                  << static_cast<uint16_t>(c) << ' ';
    std::cout << "tcodecvr_t01::testMain(), end ...\n";
}
//void testMain()
//{
//    std::cout << "tcodecvr_t01::testMain(), begin ...\n";
//    std::cout << "tcodecvr_t01::testMain(), end ...\n";
//}
}
void testMain()
{
    std::cout << "coding::tcodecvr::testMain(), begin ...\n";
    tcodecvr_t01::testMain();
    std::cout << "coding::tcodecvr::testMain(), end ...\n";
}
}