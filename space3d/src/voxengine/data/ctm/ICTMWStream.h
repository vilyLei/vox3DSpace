#pragma once
#include "ctmDef.h"
#include <memory>
#include "../stream/Uint8Array.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
class ICTMWStream
{
public:
    /**
     * uint8 array
     */
    std::shared_ptr<voxengine::data::stream::Uint8Array> data;
    Int32                                                offset;
    Int32                                                count;
    Int32                                                len;
    virtual void                                         writeByte(Uint8 value) = 0;
};
} // namespace ctm
} // namespace data
} // namespace voxengine