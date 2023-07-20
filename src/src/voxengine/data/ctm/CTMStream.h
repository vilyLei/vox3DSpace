#pragma once
#include <string>
#include <memory>
#include "ICTMRStream.h"
#include "../stream/Uint8Array.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class CTMStream : public ICTMRStream
{
public:
    CTMStream(std::shared_ptr<voxengine::data::stream::Uint8Array> pdata);
    virtual ~CTMStream();

    void                                                   showInfo() override;
    Uint8                                                  readByte() override;
    Int32                                                  readInt32() override;
    Float32                                                readFloat32() override;
    std::string                                            readString() override;
    std::shared_ptr<voxengine::data::stream::Int32Array>   readArrayInt32(std::shared_ptr<voxengine::data::stream::Int32Array> arr) override;
    std::shared_ptr<voxengine::data::stream::Float32Array> readArrayFloat32(std::shared_ptr<voxengine::data::stream::Float32Array> arr) override;

private:
    std::string decodeUint8Arr(std::shared_ptr<voxengine::data::stream::Uint8Array> pu8array);
};
} // namespace ctm
} // namespace data
} // namespace voxengine