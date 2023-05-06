#pragma once
#include <iostream>

namespace voxengine
{
namespace text
{
// thanks: https://www.foonathan.net/2022/01/compile-time-codegen/
enum class op
{
    ptr_inc,  // >
    ptr_dec,  // <
    data_inc, // +
    data_dec, // -
    write,    // .
    read,     // ,
    jmp_ifz,  // [, jump if zero
    jmp,      // ], unconditional jump
};

template <std::size_t InstructionCapacity>
struct program
{
    std::size_t inst_count;
    op          inst[InstructionCapacity];
    std::size_t inst_jmp[InstructionCapacity];
};
template <std::size_t InstructionCapacity>
void execute(const program<InstructionCapacity>& program,
             unsigned char*                      data_ptr)
{
    auto inst_ptr = std::size_t(0);
    while (inst_ptr < program.inst_count)
    {
        switch (program.inst[inst_ptr])
        {
            case op::ptr_inc:
                ++data_ptr;
                ++inst_ptr;
                break;
            case op::ptr_dec:
                --data_ptr;
                ++inst_ptr;
                break;
            case op::data_inc:
                ++*data_ptr;
                ++inst_ptr;
                break;
            case op::data_dec:
                --*data_ptr;
                ++inst_ptr;
                break;
            case op::write:
                std::putchar(*data_ptr);
                ++inst_ptr;
                break;
            case op::read:
                *data_ptr = static_cast<unsigned char>(std::getchar());
                ++inst_ptr;
                break;
            case op::jmp_ifz:
                if (*data_ptr == 0)
                    inst_ptr = program.inst_jmp[inst_ptr];
                else
                    ++inst_ptr;
                break;
            case op::jmp:
                inst_ptr = program.inst_jmp[inst_ptr];
                break;
        }
    }
}
template <std::size_t N>
constexpr auto parse(const char (&str)[N])
{
    program<N> result{};
    std::size_t jump_stack[N]  = {};
    std::size_t jump_stack_top = 0;

    for (auto ptr = str; *ptr; ++ptr)
    {
        if (*ptr == '>')
            result.inst[result.inst_count++] = op::ptr_inc;
        else if (*ptr == '<')
            result.inst[result.inst_count++] = op::ptr_dec;
        else if (*ptr == '+')
            result.inst[result.inst_count++] = op::data_inc;
        else if (*ptr == '-')
            result.inst[result.inst_count++] = op::data_dec;
        else if (*ptr == '.')
            result.inst[result.inst_count++] = op::write;
        else if (*ptr == ',')
            result.inst[result.inst_count++] = op::read;
        else if (*ptr == '[')
        {
            jump_stack[jump_stack_top++]     = result.inst_count;
            result.inst[result.inst_count++] = op::jmp_ifz;
        }
        else if (*ptr == ']')
        {
            auto open  = jump_stack[--jump_stack_top];
            auto close = result.inst_count++;

            result.inst[close]     = op::jmp;
            result.inst_jmp[close] = open;

            result.inst_jmp[open] = close + 1;
        }
    }

    return result;
}
template <typename T, std::size_t N>
void useFixLengthArr(const T (&arr)[N])
{
    std::cout << "useFixLengthArr N: " << N << std::endl;
}
/*
// some test codes
// `x = std::getchar(); y = x + 3; std::putchar(y);`
static constexpr auto add3 = parse(",>+++<[->+<]>.");
// Use this array for our data_ptr.
unsigned char memory[1024] = {};
execute(add3, memory);
*/
} // namespace text
} // namespace voxengine