#include "ReaderRAW.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
void ReaderRAW::readAttrMaps(std::shared_ptr<ICTMRStream> stream, std::vector<AttrMapType>& attrMaps)
{
    auto i = 0;
    for (; i < attrMaps.size(); ++i)
    {
        stream->readInt32(); //magic "ATTR"

        attrMaps[i].name = stream->readString();
        auto& attr       = attrMaps[i].attr;
        auto  f32        = std::make_shared<voxengine::data::stream::Float32Array>(attr->getBuffer(), attr->getByteOffset(), attr->getLength());
        stream->readArrayFloat32(f32);
    }
}
void ReaderRAW::readUVMaps(std::shared_ptr<ICTMRStream> stream, std::vector<UVMapType>& uvMaps)
{
    auto i = 0;
    for (; i < uvMaps.size(); ++i)
    {
        stream->readInt32(); //magic "TEXC"

        uvMaps[i].name     = stream->readString();
        uvMaps[i].filename = stream->readString();
        stream->readArrayFloat32(uvMaps[i].uv);
    }
}

void ReaderRAW::readNormals(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Float32Array> normals)
{
    stream->readInt32(); //magic "NORM"
    stream->readArrayFloat32(normals);
}
void ReaderRAW::readVertices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Float32Array> vertices)
{
    stream->readInt32(); //magic "VERT"
    stream->readArrayFloat32(vertices);
}
void ReaderRAW::readIndices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Int32Array> indices)
{
    stream->readInt32(); //magic "INDX"
    //auto i32 = std::make_shared<voxengine::data::stream::Int32Array>(indices->getBuffer(), indices->getByteOffset(), indices->getLength());
    stream->readArrayInt32(indices);
}


void ReaderRAW::read(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<CTMFileBody> body)
{
    auto& ivs = body->indices;
    auto  i32 = std::make_shared<voxengine::data::stream::Int32Array>(ivs->getBuffer(), ivs->getByteOffset(), ivs->getLength());
    readIndices(stream, i32);
    readVertices(stream, body->vertices);

    if (body->normals)
    {
        readNormals(stream, body->normals);
    }
    if (body->uvMaps.size() > 0)
    {
        readUVMaps(stream, body->uvMaps);
    }
    if (body->attrMaps.size() > 0)
    {
        readAttrMaps(stream, body->attrMaps);
    }
}
//class ReaderRAW

} // namespace ctm
} // namespace data
} // namespace voxengine
