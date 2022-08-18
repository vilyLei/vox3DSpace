#include "ArrayBuffer.h"
#include "Int8Array.h"

namespace voxengine
{
	namespace data
	{
		namespace stream
		{
			Int8Array::Int8Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
				DataView(arrBuf, byteOffset, length)
			{
				buildData();
			}
			Int8Array::Int8Array(Uint32 length) :
				DataView(length)
			{
				buildData();
			}
			Int8Array::~Int8Array()
			{
				//std::cout << "Int8Array::deconstructor()..." << std::endl;
			}
			void Int8Array::buildData()
			{
				buildBytesData(1);
			}
			Int8& Int8Array::operator[](Int32 i)
			{
				assertIndex(i);
				return m_bytes[i];
			}

		} // namespace base
	} // namespace voxengine
}
