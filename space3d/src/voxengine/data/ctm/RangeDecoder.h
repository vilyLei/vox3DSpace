#pragma once
#include "ctmDef.h"
#include "IDecoder.h"
#include "ICTMRStream.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
class RangeDecoder : public IDecoder
{
public:
    RangeDecoder() noexcept;
    virtual ~RangeDecoder() noexcept;
    std::shared_ptr<ICTMRStream> _stream;
    Int32                        _code;
    Int32                        _range;

    void   setStream(std::shared_ptr<ICTMRStream> stream);
    void   releaseStream();
    void   init() override;
    Uint32 decodeDirectBits(Uint32 numTotalBits);
    Uint32 decodeBit(Int32* probs, Int32 index);
};
} // namespace ctm
} // namespace data
} // namespace voxengine