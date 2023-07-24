#pragma once
#include "DataView.h"

namespace voxengine
{
	namespace data
	{
		namespace stream
		{
			class Int16Array : public DataView
			{
			public:
				Int16Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
				Int16Array(Uint32 length);
				virtual ~Int16Array();

				Int16& operator[](Int32 i);

			protected:
				void buildData() override;
			};
		} // namespace base
	} // namespace voxengine
}
