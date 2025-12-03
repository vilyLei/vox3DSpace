
#ifndef VOXOL_MATH_RANDOM_FUNCS_H
#define VOXOL_MATH_RANDOM_FUNCS_H

#include <random>
#include <concepts>
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <limits>
#include <array>

namespace Voxol::Math
{

class SimpleRandom
{
private:
    static std::mt19937_64& get_engine()
    {
        static thread_local std::mt19937_64 engine = []() {
            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            return std::mt19937_64(static_cast<uint64_t>(seed));
        }();
        return engine;
    }

public:
    // 浮点数：[0, 1)
    static float get_float()
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(get_engine());
    }

    static double get_double()
    {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(get_engine());
    }

    // 浮点数指定范围
    static float get_float(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(get_engine());
    }

    static double get_double(double min, double max)
    {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(get_engine());
    }

    // 整数：完整范围
    static int8_t get_int8()
    {
        std::uniform_int_distribution<int16_t> dist(
            std::numeric_limits<int8_t>::min(),
            std::numeric_limits<int8_t>::max());
        return static_cast<int8_t>(dist(get_engine()));
    }

    static uint8_t get_uint8()
    {
        std::uniform_int_distribution<uint16_t> dist(
            std::numeric_limits<uint8_t>::min(),
            std::numeric_limits<uint8_t>::max());
        return static_cast<uint8_t>(dist(get_engine()));
    }

    static int32_t get_int32()
    {
        std::uniform_int_distribution<int32_t> dist(
            std::numeric_limits<int32_t>::min(),
            std::numeric_limits<int32_t>::max());
        return dist(get_engine());
    }

    static uint32_t get_uint32()
    {
        std::uniform_int_distribution<uint32_t> dist(
            std::numeric_limits<uint32_t>::min(),
            std::numeric_limits<uint32_t>::max());
        return dist(get_engine());
    }

    // 整数指定范围
    template <typename T>
        requires std::is_integral_v<T>
    static T get_int(T min, T max)
    {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(get_engine());
    }

    // 随机布尔值
    static bool get_bool(double probability = 0.5)
    {
        std::bernoulli_distribution dist(probability);
        return dist(get_engine());
    }
};

template <size_t CacheSize = 1024>
class FastRandom
{
private:
    std::mt19937_64 engine;

    // 类型特定的缓存
    template <typename T>
    struct Cache
    {
        std::array<T, CacheSize> data;
        size_t                   index = CacheSize; // 初始化为无效值

        void refill(std::mt19937_64& eng)
        {
            if constexpr (std::is_floating_point_v<T>)
            {
                std::uniform_real_distribution<T> dist(0.0, 1.0);
                for (auto& val : data) val = dist(eng);
            }
            else if constexpr (std::is_integral_v<T>)
            {
                std::uniform_int_distribution<T> dist(
                    std::numeric_limits<T>::min(),
                    std::numeric_limits<T>::max());
                for (auto& val : data) val = dist(eng);
            }
            index = 0;
        }

        T next(std::mt19937_64& eng)
        {
            if (index >= data.size())
            {
                refill(eng);
            }
            return data[index++];
        }
    };

    Cache<float>    float_cache;
    Cache<double>   double_cache;
    Cache<int8_t>   int8_cache;
    Cache<uint8_t>  uint8_cache;
    Cache<int32_t>  int32_cache;
    Cache<uint32_t> uint32_cache;

public:
    FastRandom(uint64_t seed = std::random_device{}()) :
        engine(seed) {}

    float    get_float() { return float_cache.next(engine); }
    double   get_double() { return double_cache.next(engine); }
    int8_t   get_int8() { return int8_cache.next(engine); }
    uint8_t  get_uint8() { return uint8_cache.next(engine); }
    int32_t  get_int32() { return int32_cache.next(engine); }
    uint32_t get_uint32() { return uint32_cache.next(engine); }

    // 指定范围
    template <typename T>
    T get_range(T min, T max)
    {
        T value;
        if constexpr (std::is_same_v<T, float>)
        {
            value = get_float();
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            value = get_double();
        }
        else if constexpr (std::is_same_v<T, int8_t>)
        {
            value = get_int8();
        }
        else if constexpr (std::is_same_v<T, uint8_t>)
        {
            value = get_uint8();
        }
        else if constexpr (std::is_same_v<T, int32_t>)
        {
            value = get_int32();
        }
        else if constexpr (std::is_same_v<T, uint32_t>)
        {
            value = get_uint32();
        }

        // 映射到指定范围
        if constexpr (std::is_floating_point_v<T>)
        {
            return min + value * (max - min);
        }
        else
        {
            auto range = static_cast<double>(max - min);
            return static_cast<T>(min + static_cast<T>(value * range));
        }
    }
};
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