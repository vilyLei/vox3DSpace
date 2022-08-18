#pragma once
#include "DataView.h"

namespace model
{
namespace base
{
class Int32Array : public DataView
{
public:
    Int32Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
    Int32Array(Uint32 length);
    virtual ~Int32Array();

    Int32& operator[](Int32 i);

protected:
    void buildData() override;
};
} // namespace base
} // namespace model