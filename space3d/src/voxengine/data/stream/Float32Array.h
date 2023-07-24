#pragma once
#include "DataView.h"

namespace voxengine
{
	namespace data
	{
		namespace stream
		{
			class Float32Array : public DataView
			{
			public:
				Float32Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
				Float32Array(Uint32 length);
				virtual ~Float32Array();

				Float32& operator[](Int32 i);

			protected:
				void buildData() override;
			};
		} // namespace base
	} // namespace voxengine
}
