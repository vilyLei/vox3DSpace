#pragma once
#include "ctmDef.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class IDecoder
{
public:
    virtual void init() = 0;
};
} // namespace ctm
} // namespace data
} // namespace voxengine