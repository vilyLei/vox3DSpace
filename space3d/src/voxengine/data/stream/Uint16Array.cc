#include "ArrayBuffer.h"
#include "Uint16Array.h"

namespace voxengine
{
    namespace data
    {
        namespace stream
        {
            Uint16Array::Uint16Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
                DataView(arrBuf, byteOffset, length)
            {
                //std::cout << "Uint16Array::constructor(), type 0 ... length: " << length << ", m_byteOffset: " << m_byteOffset << std::endl;
                buildData();
            }
            Uint16Array::Uint16Array(Uint32 length) :
                DataView(length)
            {
                //std::cout << "Uint16Array::constructor() type 1 ..." << std::endl;
                buildData();
            }
            Uint16Array::~Uint16Array()
            {
                //std::cout << "Uint16Array::deconstructor()..." << std::endl;
            }
            void Uint16Array::buildData()
            {
                buildBytesData(2);
            }
            Uint16& Uint16Array::operator[](Int32 i)
            {
                assertIndex(i);
                return (reinterpret_cast<Uint16*>(m_bytes))[i];
            }

        } // namespace base
    } // namespace voxengine
}
