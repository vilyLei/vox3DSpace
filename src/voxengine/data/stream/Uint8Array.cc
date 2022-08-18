#include "ArrayBuffer.h"
#include "Uint8Array.h"

namespace model
{
namespace base
{
Uint8Array::Uint8Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
    DataView(arrBuf, byteOffset, length)
{
    //std::cout << "Uint8Array::constructor() type 0 ..." << std::endl;
    buildData();
}
Uint8Array::Uint8Array(Uint32 length) :
    DataView(length)
{
    //std::cout << "Uint8Array::constructor() type 1 ..." << std::endl;
    buildData();
}
Uint8Array::~Uint8Array()
{
    //std::cout << "Uint8Array::deconstructor()..." << std::endl;
}
void Uint8Array::buildData()
{
    buildBytesData(1);
}
Uint8& Uint8Array::operator[](Int32 i)
{
    assertIndex(i);
    return (reinterpret_cast<Uint8*>(m_bytes))[i];
}

} // namespace base
} // namespace model