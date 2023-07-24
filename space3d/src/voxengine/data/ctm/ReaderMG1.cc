#include "ReaderMG1.h"
#include "CTM.h"
#include "LZMA.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
// ReaderMG1
void ReaderMG1::read(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<CTMFileBody> body)
{
    //std::cout << "      ReaderMG1::read(), stream->offset: " << stream->offset << std::endl;
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

void ReaderMG1::readIndices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Int32Array> indices)
{
    auto f0 = stream->readInt32(); //magic "INDX"
    auto f1 = stream->readInt32(); //packed size

    //std::cout << "      ReaderMG1::readIndices(), A0, stream->offset: " << stream->offset << ", f0: " << f0 << ", f1: "<< f1 << std::endl;

    auto interleaved = CTM::InterleavedStream(indices, 3);
    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
    //std::cout << "      ReaderMG1::readIndices(), A1, stream->offset: " << stream->offset << ", indices[0]: " << (*indices)[0] << ", indices[1]: " << (*indices)[1] << std::endl;

    CTM::restoreIndices(indices, indices->getLength());
}

void ReaderMG1::readVertices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Float32Array> vertices)
{
    stream->readInt32(); //magic "VERT"
    stream->readInt32(); //packed size

    auto interleaved = CTM::InterleavedStream(vertices, 1);
    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
}

void ReaderMG1::readNormals(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Float32Array> normals)
{
    stream->readInt32(); //magic "NORM"
    stream->readInt32(); //packed size

    auto interleaved = CTM::InterleavedStream(normals, 3);
    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
}

void ReaderMG1::readUVMaps(std::shared_ptr<ICTMRStream> stream, std::vector<UVMapType>& uvMaps)
{
    auto i = 0;
    for (; i < uvMaps.size(); ++i)
    {
        stream->readInt32(); //magic "TEXC"

        uvMaps[i].name     = stream->readString();
        uvMaps[i].filename = stream->readString();

        stream->readInt32(); //packed size

        auto interleaved = CTM::InterleavedStream(uvMaps[i].uv, 2);
        LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
    }
}

void ReaderMG1::readAttrMaps(std::shared_ptr<ICTMRStream> stream, std::vector<AttrMapType>& attrMaps)
{
    auto i = 0;
    for (; i < attrMaps.size(); ++i)
    {
        stream->readInt32(); //magic "ATTR"

        attrMaps[i].name = stream->readString();

        stream->readInt32(); //packed size

        auto interleaved = CTM::InterleavedStream(attrMaps[i].attr, 4);
        LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
    }
}
} // namespace ctm
} // namespace data
} // namespace voxengine