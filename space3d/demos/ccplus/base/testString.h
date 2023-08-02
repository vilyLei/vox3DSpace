#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <random>
namespace base
{
namespace testString
{
namespace test_1
{
void testDistributeRandom()
{
    std::random_device                 rd;
    std::mt19937                  gen(rd());
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
    SExampleA ex0 = {       // start of initializer list for struct example
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

    SExampleA ex2 = {80, 257,0};
    std::cout << "struct SExampleA ex2: \n\t" << std::string(ex2) << std::endl;
    SExampleB ex3 = {80, 515, 769};
    std::cout << "struct SExampleA ex3: \n\t" << std::string(ex3) << std::endl;

}
} // namespace test_1

int testMain()
{
    test_1::testMain();
    test_1::testDistributeRandom();
    return EXIT_SUCCESS;
}
} // namespace testString

} // namespace base