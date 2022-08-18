#include "ArrayBuffer.h"
#include "Int16Array.h"

namespace voxengine
{
	namespace data
	{
		namespace stream
		{
			Int16Array::Int16Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
				DataView(arrBuf, byteOffset, length)
			{
				buildData();
			}
			Int16Array::Int16Array(Uint32 length) :
				DataView(length)
			{
				buildData();
			}
			Int16Array::~Int16Array()
			{
				//std::cout << "Int16Array::deconstructor()..." << std::endl;
			}
			void Int16Array::buildData()
			{
				buildBytesData(2);
			}
			Int16& Int16Array::operator[](Int32 i)
			{
				assertIndex(i);
				return (reinterpret_cast<Int16*>(m_bytes))[i];
			}

		} // namespace base
	} // namespace voxengine
}
