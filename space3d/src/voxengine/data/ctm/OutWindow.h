#pragma once
#include "ctmDef.h"
#include "ICTMWStream.h"
#include "../stream/Uint32Array.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class OutWindow
{

public:
    OutWindow() noexcept;
    virtual ~OutWindow() noexcept;


    void create(Uint32 windowSize);
    void flush();
    void releaseStream();
    void setStream(std::shared_ptr<ICTMWStream> stream);
    void init(bool solid);
    void copyBlock(Int32 distance, Int32 len);
    void putByte(Uint8 b);

    Uint8 getByte(Int32 distance);

private:
    Int32                        _pos;
    Int32                        _streamPos;
    Int32                        _windowSize;
    std::shared_ptr<ICTMWStream> _stream;
    Uint32*                      _buffer;
};
} // namespace ctm
} // namespace data
} // namespace voxengine
