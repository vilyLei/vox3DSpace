#include "FileHeader.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
FileHeader::FileHeader(std::shared_ptr<ICTMRStream> stream) noexcept
{
    auto magic = stream->readInt32(); //magic "OCTM"
    //std::cout << "      FileHeader::FileHeader(), magic: " << magic << std::endl;

    fileFormat        = stream->readInt32();
    compressionMethod = stream->readInt32();
    //std::cout << "      FileHeader::FileHeader(), compressionMethod: " << compressionMethod << std::endl;
    vertexCount   = stream->readInt32();
    triangleCount = stream->readInt32();
    uvMapCount    = stream->readInt32();
    attrMapCount  = stream->readInt32();
    flags         = stream->readInt32();
    //std::cout << "      FileHeader::FileHeader(), flags: " << flags << std::endl;
    comment = stream->readString();
}
FileHeader::~FileHeader()
{
}

Int32 FileHeader::hasNormals()
{
    auto f = flags & static_cast<Int32>(CTMFlags::NORMALS);
    //std::cout << "      FileHeader::hasNormals(), f: " << f << std::endl;

    return flags & static_cast<Int32>(CTMFlags::NORMALS);
}
} // namespace ctm
} // namespace data
} // namespace voxengine