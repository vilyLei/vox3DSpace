#pragma once
#include "DataView.h"

namespace model
{
namespace base
{
class Uint8Array : public DataView
{
public:
    Uint8Array(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
    Uint8Array(Uint32 length);
    virtual ~Uint8Array();

    Uint8& operator[](Int32 i);

protected:
    void buildData() override;
};

} // namespace base
} // namespace model