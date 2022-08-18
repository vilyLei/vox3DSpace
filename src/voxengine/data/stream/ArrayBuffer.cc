#include "ArrayBuffer.h"

namespace model
{
namespace base
{
ArrayBuffer::ArrayBuffer(Int32 byteLength) :
    m_byteLength(byteLength)
{
    m_bytes = new Int8[byteLength]{};
    //std::cout << "ArrayBuffer::constructor()..." << byteLength << std::endl;
}

ArrayBuffer::~ArrayBuffer()
{
    DELETE_OBJ_ARRAY_NAKED_PTR(m_bytes);
    //std::cout << "ArrayBuffer::deconstructor()..." << std::endl;
}

Int32 ArrayBuffer::getByteLength()
{
    return m_byteLength;
}
Int8* ArrayBuffer::getBytes()
{
    return m_bytes;
}

} // namespace base
} // namespace model