#pragma once
#include "DataView.h"

namespace voxengine
{
    namespace data
    {
        namespace stream
        {
            class Uint16Array : public DataView
            {
            public:
                Uint16Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
                Uint16Array(Uint32 length);
                virtual ~Uint16Array();

                Uint16& operator[](Int32 i);

            protected:
                void buildData() override;
            };
        } // namespace base
    } // namespace voxengine
}
