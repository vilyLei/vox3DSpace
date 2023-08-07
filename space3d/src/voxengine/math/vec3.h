#pragma once
#include <string>
namespace voxengine
{
namespace math
{
template <typename NumberType>
class Vec3
{
public:
    explicit Vec3(NumberType px, NumberType py, NumberType pz, NumberType pw = static_cast<NumberType>(1)) noexcept;
    Vec3() noexcept;
    ~Vec3() = default;
    //virtual ~Vec3();

    NumberType x;
    NumberType y;
    NumberType z;
    NumberType w;

    NumberType& operator[](unsigned int i);
    void        set(NumberType px, NumberType py, NumberType pz, NumberType pw);
    void        setXYZ(NumberType px, NumberType py, NumberType pz);

    NumberType dot(const Vec3& v3) const;
    NumberType getLength() const;
    NumberType getLengthSquared() const;

    void copyFrom(const Vec3& v3);
    void multBy(const Vec3& v3);
    void normalize();
    void normalizeTo(Vec3& v3) const;
    void scaleVector(const Vec3& v3);
    void scaleBy(NumberType s);
    void negate();
    bool equalsXYZ(const Vec3& v3);
    bool equalsAll(const Vec3& v3);
    void project();
    void addBy(const Vec3& v3);
    void subtractBy(const Vec3& v3);
    void crossBy(const Vec3& v3);
    void reflectBy(const Vec3& nv);

    void scaleVecTo(const Vec3& va, NumberType scale);
    void subVecsTo(const Vec3& va, const Vec3& vb);

    void addVecsTo(const Vec3& va, const Vec3& vb);
    void crossVecsTo(const Vec3& va, const Vec3& vb);
    Vec3 subtract(const Vec3& v3) const;
    Vec3 crossProduct(const Vec3& v3) const;
    Vec3 clone() const;

    void        toArray3(NumberType* arr, unsigned int offset = 0);
    void        toArray4(NumberType* arr, unsigned int offset = 0);
    Vec3*       fromArray3(NumberType* arr, unsigned int offset = 0);
    Vec3*       fromArray4(NumberType* arr, unsigned int offset = 0);
    std::string toString() const;


    /**
     * ���ַ���(Ϊ��)
     */
    static void cross(const Vec3& a, const Vec3& b, Vec3& result);
    // (va1 - va0) ��� (vb1 - vb0), ���ַ���(Ϊ��)
    static void       crossSubtract(const Vec3& va0, const Vec3& va1, const Vec3& vb0, const Vec3& vb1, Vec3& result);
    static void       subtract(const Vec3& a, const Vec3& b, Vec3& result);
    static NumberType distanceSquared(const Vec3& a, const Vec3& b);
    static NumberType distanceXYZ(NumberType x0, NumberType y0, NumberType z0, NumberType x1, NumberType y1, NumberType z1);
    static NumberType distance(const Vec3& v0, const Vec3& v1);

    /**
     * get angle degree between two Vec3 objects
     * @param v0 src Vec3 object
     * @param v1 dst Vec3 object
     * @returns angle degree
     */
    static NumberType angleBetween(const Vec3& v0, const Vec3& v1);
    /**
     * get angle radian between two Vec3 objects
     * @param v0 src Vec3 object
     * @param v1 dst Vec3 object
     * @returns angle radian
     */
    static NumberType radianBetween(const Vec3& v0, const Vec3& v1);
    static NumberType radianBetween2(const Vec3& v0, const Vec3& v1);
    static void       reflect(const Vec3& iv, const Vec3& nv, Vec3& rv);

    const static Vec3 X_AXIS;
    const static Vec3 Y_AXIS;
    const static Vec3 Z_AXIS;
    const static Vec3 ZERO;
    const static Vec3 ONE;

private:
    const static NumberType s_180OverPi;
    const static NumberType s_minv;
    static Vec3             s_v0;
    static Vec3             s_v1;
};

typedef Vec3<long> UVec3;

#define CREATE_SATATIC_VEC3_INS(T, NAME, v0, v1, v2, v3) \
    template <typename T>                                \
    const Vec3<T> Vec3<T>::NAME{static_cast<T>(v0), static_cast<T>(v1), static_cast<T>(v2), static_cast<T>(v3)};
} // namespace math
} // namespace voxengine