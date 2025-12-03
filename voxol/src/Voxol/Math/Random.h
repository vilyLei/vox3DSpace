
#ifndef VOXOL_MATH_RANDOM_H
#define VOXOL_MATH_RANDOM_H
#include <random>
#include <concepts>
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <limits>

namespace Voxol::Math
{
class Random
{
private:
    // 线程安全的随机引擎
    static std::mt19937_64& engine()
    {
        static thread_local std::mt19937_64 eng = []() {
            auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
            return std::mt19937_64(static_cast<uint64_t>(seed));
        }();
        return eng;
    }

public:
    // ========== 浮点数 ==========
    static float Float()
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(engine());
    }

    static float Float(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine());
    }

    static double Double()
    {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(engine());
    }

    static double Double(double min, double max)
    {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(engine());
    }

    // ========== 有符号整数 ==========
    static int8_t Int8()
    {
        return Int8(std::numeric_limits<int8_t>::min(),
                    std::numeric_limits<int8_t>::max());
    }

    static int8_t Int8(int8_t min, int8_t max)
    {
        std::uniform_int_distribution<int16_t> dist(min, max);
        return static_cast<int8_t>(dist(engine()));
    }

    static int32_t Int32()
    {
        return Int32(std::numeric_limits<int32_t>::min(),
                     std::numeric_limits<int32_t>::max());
    }

    static int32_t Int32(int32_t min, int32_t max)
    {
        std::uniform_int_distribution<int32_t> dist(min, max);
        return dist(engine());
    }

    // ========== 无符号整数 ==========
    static uint8_t UInt8()
    {
        return UInt8(std::numeric_limits<uint8_t>::min(),
                     std::numeric_limits<uint8_t>::max());
    }

    static uint8_t UInt8(uint8_t min, uint8_t max)
    {
        std::uniform_int_distribution<uint16_t> dist(min, max);
        return static_cast<uint8_t>(dist(engine()));
    }

    static uint32_t UInt32()
    {
        return UInt32(std::numeric_limits<uint32_t>::min(),
                      std::numeric_limits<uint32_t>::max());
    }

    static uint32_t UInt32(uint32_t min, uint32_t max)
    {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(engine());
    }

    // ========== 高级功能 ==========
    static bool Bool(double true_probability = 0.5)
    {
        std::bernoulli_distribution dist(true_probability);
        return dist(engine());
    }

    // 随机选择枚举值
    template <typename Enum>
        requires std::is_enum_v<Enum>
    static Enum EnumValue()
    {
        using Underlying = std::underlying_type_t<Enum>;
        std::uniform_int_distribution<Underlying> dist(
            static_cast<Underlying>(Enum::First), // 假设定义了 First/Last
            static_cast<Underlying>(Enum::Last));
        return static_cast<Enum>(dist(engine()));
    }

    // 正态分布
    static double Normal(double mean = 0.0, double stddev = 1.0)
    {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(engine());
    }

    // 高斯分布
    static float Gaussian(float mean = 0.0f, float stddev = 1.0f)
    {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(engine());
    }

    // 设置种子（用于测试和复现）
    static void Seed(uint64_t seed)
    {
        engine().seed(seed);
    }

    // 获取当前种子（用于保存状态）
    static uint64_t GetSeed()
    {
        // 注意：mt19937_64 没有直接获取种子的方法
        // 这里返回一个伪种子
        static thread_local uint64_t current_seed =
            std::chrono::steady_clock::now().time_since_epoch().count();
        return current_seed;
    }
};
}
#endif