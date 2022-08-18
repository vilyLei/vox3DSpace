#pragma once
#include "baseDef.h"
namespace model
{
namespace base
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
} // namespace model