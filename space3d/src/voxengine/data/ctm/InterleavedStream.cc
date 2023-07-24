#include "InterleavedStream.h"
#include "ctmUtils.h"

namespace voxengine
{
namespace data
{
namespace ctm
{

bool InterleavedStream::CTM_isLittleEndian{};
InterleavedStream::InterleavedStream(std::shared_ptr<voxengine::data::stream::DataView> pdata, Int32 pcount) noexcept
{
    //std::cout << "      InterleavedStream::InterleavedStream(), ###$$$ offset: " << pdata->getByteOffset() << ", byteLength: " << pdata->getByteLength() << std::endl;
    data = std::make_shared<voxengine::data::stream::Uint8Array>(pdata->getBuffer(), pdata->getByteOffset(), pdata->getByteLength());

    offset = isLittleEndian() ? 3 : 0;

    count = pcount << 2;
    //std::cout << "      InterleavedStream::InterleavedStream(), pcount: " << pcount << std::endl;
    len = data->getLength();

    //std::cout << "      InterleavedStream::InterleavedStream(), len: " << len << ",data getByteLength(): " << data->getByteLength() << std::endl;
}
InterleavedStream ::~InterleavedStream() noexcept
{
    data = nullptr;
}

void InterleavedStream ::writeByte(Uint8 value)
{
    if (offset < 0)
    {
        std::cout << "      Error InterleavedStream::writeByte() offset: " << offset << std::endl;
        return;
    }
    (*data)[offset] = value;

    Int32 flag = (isLittleEndian() ? 1 : -1);
    offset += count;
    //std::cout << "      InterleavedStream::writeByte(), flag: " << flag << ", offset: " << offset << ", count: " << count << std::endl;
    if (offset >= len)
    {
        offset -= len - 4;
        if (offset >= count)
        {
            auto poffset = offset;
            offset -= count + flag;
        }
    }
}
} // namespace ctm
} // namespace data
} // namespace voxengine