#pragma once
#include <string>
namespace voxengine
{
namespace math
{
template <typename T>
class Vec2
{
public:
    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };
        T data[3];
    };

    //explicit Vec2(T px, T py, T pz = static_cast<T>(1)) noexcept;
    //Vec2() noexcept;
    ~Vec2() = default;

    explicit Vec2(T x, T y, T z = static_cast<T>(1)) noexcept :
        x(x), y(y), z(z)
    {}
    Vec2() noexcept :
        x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0))
    {}

    Vec2(const Vec2& v) :
        x(v.x), y(v.y), z(v.z)
    {
    }
    Vec2(Vec2&& v) :
        x(v.x), y(v.y), z(v.z)
    {}
    Vec2& operator=(const Vec2& v)
    {
        std::memcpy(data, v.data, sizeof(T) * 3);
        return *this;
    }
    Vec2& operator=(Vec2&& v)
    {
        std::memcpy(data, v.data, sizeof(T) * 3);
        return *this;
    }
    T&          operator[](unsigned int i);
    T const&    operator[](unsigned int i) const;
    void        set(T px, T py, T pz);
    void        setXY(T px, T py);

    T dot(const Vec2& v2) const;
    T getLength() const;
    T getLengthSquared() const;

    void       copyFrom(const Vec2& v2);
    void       multBy(const Vec2& v2);
    void       normalize();
    void       normalizeTo(Vec2& v2) const;
    void       scaleVector(const Vec2& v2);
    void       scaleBy(T s);
    void       negate();
    bool       equalsXYZ(const Vec2& v2);
    bool       equalsAll(const Vec2& v2);
    void       project();
    void       addBy(const Vec2& v2);
    void       subtractBy(const Vec2& v2);
    T crossBy(const Vec2& v2) const;
    void       reflectBy(const Vec2& nv);

    void scaleVecTo(const Vec2& va, T scale);
    void subVecsTo(const Vec2& va, const Vec2& vb);

    void addVecsTo(const Vec2& va, const Vec2& vb);
    Vec2 subtract(const Vec2& v2) const;
    Vec2 clone() const;

    void        toArray2(T* arr, unsigned int offset = 0);
    void        toArray3(T* arr, unsigned int offset = 0);
    Vec2*       fromArray2(T* arr, unsigned int offset = 0);
    Vec2*       fromArray3(T* arr, unsigned int offset = 0);
    std::string toString() const;

    const static Vec2 X_AXIS;
    const static Vec2 Y_AXIS;
    const static Vec2 ZERO;
    const static Vec2 ONE;

private:
    const static T s_180OverPi;
    const static T s_minv;
    static Vec2             s_v0;
    static Vec2             s_v1;
};

typedef Vec2<long> UVec2;


#define CREATE_SATATIC_VEC2_INS(T, NAME, v0, v1, v2) \
    template <typename T>                     \
    const Vec2<T> Vec2<T>::NAME{static_cast<T>(v0), static_cast<T>(v1), static_cast<T>(v2)};

} // namespace math
} // namespace voxengine