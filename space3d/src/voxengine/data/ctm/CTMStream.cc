#include <limits>
#include "ctmUtils.h"
#include "CTMStream.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
const double CTMStream::TWO_POW_MINUS23{std::pow(2, -23)};
const double CTMStream::TWO_POW_MINUS126{std::pow(2, -126)};

CTMStream::CTMStream(std::shared_ptr<voxengine::data::stream::Uint8Array> pdata)
{
    data   = pdata;
    offset = 0;
    //std::cout << "CTMStream::constructor()... offset: " << offset << std::endl;
}
CTMStream::~CTMStream()
{
    data = nullptr;
    //std::cout << "CTMStream::deconstructor()..." << std::endl;
}
void CTMStream::showInfo()
{
    //std::cout << "      XXXX CTMStream->showInfo(), offset: " << offset << std::endl;
}
std::string CTMStream::decodeUint8Arr(std::shared_ptr<voxengine::data::stream::Uint8Array> pu8array)
{
    //return new TextDecoder("utf-8").decode(u8array);
    std::string str;
    return str;
}
Uint8 CTMStream::readByte()
{
    return (*data)[offset++] & 0xff;
}

Int32 CTMStream::readInt32()
{
    auto& pd = (*data);

    Int32 i = pd[offset++];
    i |= pd[offset++] << 8;
    i |= pd[offset++] << 16;
    return static_cast<Int32>(i | (pd[offset++] << 24));
}

Float32 CTMStream::readFloat32()
{
    auto& pd = (*data);
    Int32 m  = pd[offset++];
    m += pd[offset++] << 8;

    auto b1 = pd[offset++];
    auto b2 = pd[offset++];

    m += (b1 & 0x7f) << 16;
    auto e = ((b2 & 0x7f) << 1) | ((b1 & 0x80) >> 7);
    //auto e = ((b2 & 0x7f) << 1) | ((b1 & 0x80) >>> 7);
    //auto e = ((b2 & 0x7f) << 1) | rightMoveBitFillZero<Uint8, Uint8>(b1 & 0x80, 7);
    auto s = b2 & 0x80 ? -1 : 1;

    if (e == 255)
    {
        return m != 0 ? NAN : s * std::numeric_limits<Float32>::infinity();
    }
    if (e > 0)
    {
        return s * (1.0 + m * TWO_POW_MINUS23) * std::pow(2, e - 127);
    }
    if (m != 0)
    {
        return s * m * TWO_POW_MINUS126;
    }
    return s * 0.0f;
}

std::string CTMStream::readString()
{
    auto len = readInt32();
    //std::cout << "      %%% CTMStream->readString(), len: " << len << ", offset: " << offset << std::endl;
    offset += len;

    //auto bytes = data.subarray(offset - len, offset);
    //return decodeUint8Arr(bytes);
    std::string str;
    return str;
}

std::shared_ptr<voxengine::data::stream::Int32Array> CTMStream::readArrayInt32(std::shared_ptr<voxengine::data::stream::Int32Array> array)
{
    Int32 i = 0, len = array->getLength();

    while (i < len)
    {
        (*array)[i++] = readInt32();
    }

    return array;
}

std::shared_ptr<voxengine::data::stream::Float32Array> CTMStream::readArrayFloat32(std::shared_ptr<voxengine::data::stream::Float32Array> array)
{
    Int32 i   = 0,
          len = array->getLength();

    while (i < len)
    {
        (*array)[i++] = readFloat32();
    }

    return array;
}

} // namespace ctm
} // namespace data
}
  