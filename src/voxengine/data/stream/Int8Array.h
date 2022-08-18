#pragma once
#include "DataView.h"

namespace model
{
namespace base
{
class Int8Array : public DataView
{
public:
    Int8Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
    Int8Array(Uint32 length);
    virtual ~Int8Array();

    Int8& operator[](Int32 i);

protected:
    void buildData() override;
};
} // namespace base
} // namespace model