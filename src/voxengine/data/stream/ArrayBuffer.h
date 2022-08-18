#pragma once
#include "streamDef.h"
namespace voxengine
{
namespace data
{
namespace stream
{
class ArrayBuffer
{
public:
    ArrayBuffer(Int32 byteLength);
    virtual ~ArrayBuffer();

    Int32 getByteLength();
    Int8*  getBytes();

private:
    Int32 m_byteLength;
    Int8*  m_bytes;
};

} // namespace base
} // namespace voxengine
}
