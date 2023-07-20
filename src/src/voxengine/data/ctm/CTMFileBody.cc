
#include "../stream/ArrayBuffer.h"
#include "../stream/Uint32Array.h"
#include "CTMFileBody.h"

namespace voxengine
{
namespace data
{
namespace ctm
{

CTMFileBody::CTMFileBody(std::shared_ptr<FileHeader> header) noexcept
{
    Int32 i   = header->triangleCount * 3,
          v   = header->vertexCount * 3,
          n   = header->hasNormals() ? header->vertexCount * 3 : 0,
          u   = header->vertexCount * 2,
          a   = header->vertexCount * 4,
          j   = 0;
    auto data = std::make_shared<voxengine::data::stream::ArrayBuffer>((i + v + n + u * header->uvMapCount + a * header->attrMapCount) * 4);
    indices   = std::make_shared<voxengine::data::stream::Uint32Array>(data, 0, i);

    vertices = std::make_shared<voxengine::data::stream::Float32Array>(data, i * 4, v);

    if (header->hasNormals())
    {
        normals = std::make_shared<voxengine::data::stream::Float32Array>(data, (i + v) * 4, n);
    }

    if (header->uvMapCount)
    {
        uvMaps.reserve(header->uvMapCount);
        for (j = 0; j < header->uvMapCount; ++j)
        {
            uvMaps.push_back(UVMapType{
                "",
                "",
                std::make_shared<voxengine::data::stream::Float32Array>(data, (i + v + n + j * u) * 4, u)});
        }
    }

    if (header->attrMapCount)
    {
        attrMaps.reserve(header->uvMapCount);
        for (j = 0; j < header->attrMapCount; ++j)
        {
            attrMaps.push_back(
                AttrMapType{"", std::make_shared<voxengine::data::stream::Float32Array>(data, (i + v + n + u * header->uvMapCount + j * a) * 4, a)});
        }
    }
}
CTMFileBody::~CTMFileBody()
{
    std::cout << "      CTMFileBody::deconstructor() ..." << std::endl;
    uvMaps.clear();
    attrMaps.clear();
    std::vector<UVMapType>().swap(uvMaps);
    std::vector<AttrMapType>().swap(attrMaps);

    indices  = nullptr;
    vertices = nullptr;
    normals  = nullptr;
}
std::shared_ptr<voxengine::data::stream::Float32Array> CTMFileBody::getUVSAt(Int32 i)
{
    return uvMaps[i].uv;
}

} // namespace ctm
} // namespace data
} // namespace voxengine