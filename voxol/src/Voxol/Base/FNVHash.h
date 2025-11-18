#pragma once

#include <cstdint>
#include <type_traits>

namespace Voxol::Base {
namespace Hash {
    
template<size_t Bits>
struct FNVConfig;

template<>
struct FNVConfig<32> {
    using ValueType = uint32_t;
    static constexpr ValueType PRIME = 16777619u;
    static constexpr ValueType OFFSET_BASIS = 2166136261u;
};

template<>
struct FNVConfig<64> {
    using ValueType = uint64_t;
    static constexpr ValueType PRIME = 1099511628211ull;
    static constexpr ValueType OFFSET_BASIS = 14695981039346656037ull;
};

// common FNV Hash
template<size_t Bits = 64>
class FNVHasher {
private:
    using Config = FNVConfig<Bits>;
    typename Config::ValueType hash_;

public:
    FNVHasher() : hash_(Config::OFFSET_BASIS) {}
    
    explicit FNVHasher(typename Config::ValueType seed) : hash_(seed) {}
    
    // update hash state
    template<typename T>
    void update(const T* data, size_t count = 1) {
        static_assert(std::is_trivial_v<T>, "T must be trivial type for hashing");
        
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
        size_t totalBytes = count * sizeof(T);
        
        for (size_t i = 0; i < totalBytes; ++i) {
            hash_ ^= bytes[i];
            hash_ *= Config::PRIME;
        }
    }
    
    // update a hash value
    template<typename T>
    void update(const T& value) {
        update(&value, 1);
    }
    
    // string specialization
    void update(const char* str) {
        while (*str) {
            hash_ ^= static_cast<uint8_t>(*str);
            hash_ *= Config::PRIME;
            ++str;
        }
    }
    
    void update(const std::string& str) {
        update(str.c_str());
    }
    
    // get current hash value
    auto getCurrentHash() const { return hash_; }
    
    auto finalize() {
        auto result = hash_;
        reset();
        return result;
    }
    
    void reset() {
        hash_ = Config::OFFSET_BASIS;
    }
    
    void reset(typename Config::ValueType seed) {
        hash_ = seed;
    }
};

// get an independent hash value
template<size_t Bits = 64, typename... Args>
auto fnvHash(const Args&... args) {
    FNVHasher<Bits> hasher;
    (hasher.update(args), ...);
    return hasher.finalize();
}

}
} // namespace Core::Hash