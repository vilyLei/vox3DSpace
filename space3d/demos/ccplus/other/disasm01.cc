
#include <stdio.h>

int main(int argc, char** argv)
{
    // AT&T assembly format
    // GUN x86 64bit AT&T assembly

    auto sk = argc;             // movl #edi, %ecx                  # 将形参argc数据值存放到 ecx寄存器
    int a = 11 << sk;           // movl $11, %eax                   # 将立即数11存放到 eax寄存器
                                // sall %cl, %eax                   # 用cl寄存器中的值对eax中的值做左移位运算操作，ch|cl -> cx -> ecx -> rcx(see: https://stackoverflow.com/questions/15869950/clarify-subsections-of-32-bit-registers-x86)
    int b = 19 + argc;          // leal	152(%rax, %rdi, 8), %eax    # 8 * rdi + rax + 152(152 = 19 * 8), 结果存放在eax中, 实际上完成的是 a + b * 8 计算过程的操作
    volatile int rv = a + b * 8;
    rv *= sk;                   // imull	%edi, %eax      # eax *= edi
    return rv;
}
// no optimize
// g++ -O0 -Wa,-adhln ./disasm01.cc
// O1 optimize
// g++ -O1 -Wa,-adhln ./disasm01.cc
// O3 optimize
// g++ -O3 -Wa,-adhln ./disasm01.cc

// gcc -S -o disasm01.s disasm01.cc
// g++ -S -o disasm01.s disasm01.cc

// 目标文件反汇编
// gcc -c -o main.o main.c
// objdump -s -d main.o > main.o.txt

// GAS assembly instructions are generally suffixed with the letters "b", "s", "w", "l", "q" or "t" to determine what size operand is being manipulated.
// b = byte (8 bit).
// s = single (32-bit floating point).
// w = word (16 bit).
// l = long (32 bit integer or 64-bit floating point).
// q = quad (64 bit).
// t = ten bytes (80-bit floating point).