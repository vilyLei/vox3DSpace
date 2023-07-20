#pragma once
#include "ctmDef.h"
#include "ICTMWStream.h"
#include "../stream/DataView.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class InterleavedStream : public ICTMWStream
{

public:
    InterleavedStream(std::shared_ptr<voxengine::data::stream::DataView> pdata, Int32 pcount) noexcept;
    virtual ~InterleavedStream() noexcept;

    void writeByte(Uint8 value) override;

    static bool CTM_isLittleEndian;
};
} // namespace ctm
} // namespace data
} // namespace voxengine