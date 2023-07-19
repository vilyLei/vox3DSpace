#pragma once
namespace voxengine
{
namespace math
{

template <typename NumberType>
class IMatrix4
{
public:
    virtual NumberType* getLocalFS() const = 0;
};

} // namespace math
} // namespace voxengine