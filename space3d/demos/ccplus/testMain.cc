#include <iostream>
#include <variant>

class ValueUnit
{

public:
    std::string unitName = "";
    void setValue(const std::string&                                 puniform_name,
                    const std::variant<bool, int, const std::string, const char *,  float>& value) const
    {
        const int         location     = 1001;
        const std::string uniform_name = puniform_name;

        std::cout << "uniform_name: " << uniform_name << std::endl;

        struct Visitor
        {
            int program  = 0;
            int location = 1;
            Visitor(int program, int location) :
                program(program), location(location) {}

            void operator()(bool value)
            {
                std::cout << "set bool value: " << value << std::endl;
            }
            void operator()(int value)
            {
                std::cout << "set unsigned int value: " << value << std::endl;
            }
            void operator()(float value)
            {
                std::cout << "set float value: " << value << std::endl;
            }
            void operator()(const std::string& value)
            {
                std::cout << "set string value: " << value << std::endl;
            }
            void operator()(const char* value)
            {
                std::cout << "set const char* value: " << value << std::endl;
            }
        };
        int program = 2001;
        std::visit(Visitor{program, location}, value);
    }
};
int main()
{
    std::variant<int, float, std::string> v;
    v = 10;
    std::visit([](auto&& arg) { std::cout << arg << '\n'; }, v);
    v = 3.14f;
    std::visit([](auto&& arg) { std::cout << arg << '\n'; }, v);
    v = "test-string";
    std::visit([](auto&& arg) { std::cout << arg << '\n'; }, v);
    std::cout << "\n--- --- --- --- --- ---\n" << std::endl;

    ValueUnit valueUnit{};
    valueUnit.setValue("info01", 11.3f);
    //valueUnit.setValue("info02", std::string("tex_shd"));
    valueUnit.setValue("info03", "ps_shd");
    return 0;
}