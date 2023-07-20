#include "ArrayBuffer.h"
#include "Uint32Array.h"

namespace voxengine
{
    namespace data
    {
        namespace stream
        {
            Uint32Array::Uint32Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
                DataView(arrBuf, byteOffset, length)
            {
                buildData();
            }
            Uint32Array::Uint32Array(Uint32 length) :
                DataView(length)
            {
                buildData();
            }
            Uint32Array::~Uint32Array()
            {
                //std::cout << "Uint32Array::deconstructor()..." << std::endl;
            }
            void Uint32Array::buildData()
            {
                buildBytesData(4);
            }
            Uint32& Uint32Array::operator[](Int32 i)
            {
                assertIndex(i);
                return (reinterpret_cast<Uint32*>(m_bytes))[i];
            }

        } // namespace base
    } // namespace voxengine
}