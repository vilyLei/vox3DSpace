#pragma once
#include "DataView.h"

namespace model
{
namespace base
{
class Uint32Array : public DataView
{
public:
    Uint32Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
    Uint32Array(Uint32 length);
    virtual ~Uint32Array();

    Uint32& operator[](Int32 i);

protected:
    void buildData() override;
};
} // namespace base
} // namespace model