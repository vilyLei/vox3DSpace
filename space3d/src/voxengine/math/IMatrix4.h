#pragma once
namespace voxengine
{
namespace math
{

template <typename T>
class IMatrix4
{
public:
    virtual T* getLocalFS() const = 0;
};

} // namespace math
} // namespace voxengine