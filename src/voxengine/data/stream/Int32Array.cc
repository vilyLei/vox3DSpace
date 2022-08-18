#include "ArrayBuffer.h"
#include "Int32Array.h"

namespace model
{
namespace base
{
Int32Array::Int32Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
    DataView(arrBuf, byteOffset, length)
{
    buildData();
}
Int32Array::Int32Array(Uint32 length) :
    DataView(length)
{
    buildData();
}
Int32Array::~Int32Array()
{
    //std::cout << "Int32Array::deconstructor()..." << std::endl;
}
void Int32Array::buildData()
{
    buildBytesData(4);
}
Int32& Int32Array::operator[](Int32 i)
{
    assertIndex(i);
    return (reinterpret_cast<Int32*>(m_bytes))[i];
}

} // namespace base
} // namespace model