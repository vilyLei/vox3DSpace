#pragma once
#include <memory>
#include "DataView.h"

namespace voxengine
{
	namespace data
	{
		namespace stream
		{
			//template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value>::type>	// correct format def
			template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
            class BaseTypeArray : public DataView
			{
			public:
                using _Esft_type = BaseTypeArray<T>;
                BaseTypeArray(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0) :
                    DataView(arrBuf, byteOffset, length)
                {
                    buildData();
				}
				BaseTypeArray(Uint32 length) :
					DataView(length)
                {
                    buildData();
                }
				~BaseTypeArray()
				{
				}
				T& operator[](Int32 i)
                {
                    assertIndex(i);
                    return (reinterpret_cast<T*>(m_bytes))[i];
                }
                [[nodiscard]] std::shared_ptr<BaseTypeArray> getSharedPtr()
                {
                    return std::shared_ptr<BaseTypeArray>(_Wptr);
                }

			protected:
                void buildData() override
                {
                    buildBytesData(sizeof(T));
                }

            private:
                template <class _Yty>
                friend class std::shared_ptr;
                mutable std::weak_ptr<BaseTypeArray> _Wptr;
			};
		} // namespace base
	} // namespace voxengine
}
