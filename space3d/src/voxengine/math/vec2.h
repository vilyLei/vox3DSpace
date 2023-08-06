#pragma once
#include <string>
namespace voxengine
{
namespace math
{
template <typename NumberType>
class Vec2
{
public:
    explicit Vec2(NumberType px, NumberType py, NumberType pw = static_cast<NumberType>(1)) noexcept;
    Vec2() noexcept;
    ~Vec2() = default;
    //virtual ~Vec2();

    NumberType x;
    NumberType y;
    NumberType w;

    NumberType& operator[](unsigned int i);
    void        set(NumberType px, NumberType py, NumberType pw);
    void        setXY(NumberType px, NumberType py);

    NumberType dot(const Vec2& v2) const;
    NumberType getLength() const;
    NumberType getLengthSquared() const;

    void       copyFrom(const Vec2& v2);
    void       multBy(const Vec2& v2);
    void       normalize();
    void       normalizeTo(Vec2& v2) const;
    void       scaleVector(const Vec2& v2);
    void       scaleBy(NumberType s);
    void       negate();
    bool       equalsXYZ(const Vec2& v2);
    bool       equalsAll(const Vec2& v2);
    void       project();
    void       addBy(const Vec2& v2);
    void       subtractBy(const Vec2& v2);
    NumberType crossBy(const Vec2& v2) const;
    void       reflectBy(const Vec2& nv);

    void scaleVecTo(const Vec2& va, NumberType scale);
    void subVecsTo(const Vec2& va, const Vec2& vb);

    void addVecsTo(const Vec2& va, const Vec2& vb);
    Vec2 subtract(const Vec2& v2) const;
    Vec2 clone() const;

    void        toArray2(NumberType* arr, unsigned int offset = 0);
    void        toArray3(NumberType* arr, unsigned int offset = 0);
    Vec2*       fromArray2(NumberType* arr, unsigned int offset = 0);
    Vec2*       fromArray3(NumberType* arr, unsigned int offset = 0);
    std::string toString();

    const static Vec2 X_AXIS;
    const static Vec2 Y_AXIS;
    const static Vec2 ZERO;
    const static Vec2 ONE;

private:
    const static NumberType s_180OverPi;
    const static NumberType s_minv;
    static Vec2             s_v0;
    static Vec2             s_v1;
};

typedef Vec2<long> UVec2;


#define CREATE_SATATIC_VEC2_INS(T, NAME, v0, v1, v2) \
    template <typename T>                     \
    const Vec2<T> Vec2<T>::NAME{static_cast<T>(v0), static_cast<T>(v1), static_cast<T>(v2)};

} // namespace math
} // namespace voxengine