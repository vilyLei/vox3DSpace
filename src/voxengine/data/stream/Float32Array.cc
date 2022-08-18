#include "ArrayBuffer.h"
#include "Float32Array.h"

namespace model
{
namespace base
{
Float32Array::Float32Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
    DataView(arrBuf, byteOffset, length)
{
    buildData();
}
Float32Array::Float32Array(Uint32 length) :
    DataView(length)
{
    buildData();
}
Float32Array::~Float32Array()
{
    //std::cout << "Float32Array::deconstructor()..." << std::endl;
}
void Float32Array::buildData()
{
    buildBytesData(4);
}
Float32& Float32Array::operator[](Int32 i)
{
    assertIndex(i);
    return (reinterpret_cast<Float32*>(m_bytes))[i];
}

} // namespace base
} // namespace model