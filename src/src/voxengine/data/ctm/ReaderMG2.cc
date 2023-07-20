#include "ReaderMG2.h"
#include "CTM.h"
#include "LZMA.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
ReaderMG2::ReaderMG2() :
    MG2Header(nullptr)
{
}
ReaderMG2::~ReaderMG2()
{
    MG2Header = nullptr;
    //std::cout << "ReaderMG2::deconstructor()..." << std::endl;
}

void ReaderMG2::read(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<CTMFileBody> body)
{
    //std::cout << "      ReaderMG2::read(), stream->offset: " << stream->offset << std::endl;
    //stream->showInfo();
    auto& ivs = body->indices;
    auto  i32 = std::make_shared<voxengine::data::stream::Int32Array>(ivs->getBuffer(), ivs->getByteOffset(), ivs->getLength());

    MG2Header = std::make_shared<FileMG2Header>(stream);

    readVertices(stream, body->vertices);
    readIndices(stream, i32);

    //showArrayViewPtr(body->vertices, "ReaderMG2::read() vertices\n");
    //showArrayViewPtr(i32, "ReaderMG2::read() indices\n");
    //std::cout << "      ReaderMG2::read(), body->normals!=nullptr: " << (body->normals != nullptr) << std::endl;
    if (body->normals)
    {
        //showArrayViewPtr(body->normals, "ReaderMG2::read() normals\n");
        readNormals(stream, body);
    }
    //showArrayViewPtr(body->normals, "ReaderMG2::read() normals\n");
    //std::cout << "      ReaderMG2::read(), body->uvMaps.size(): " << body->uvMaps.size() << std::endl;
    if (body->uvMaps.size() > 0)
    {
        readUVMaps(stream, body->uvMaps);
        //showArrayViewPtr(body->uvMaps[0].uv, "ReaderMG2::read() uv\n");
    }
    //std::cout << "      ReaderMG2::read(), body->attrMaps.size(): " << body->attrMaps.size() << std::endl;
    if (body->attrMaps.size() > 0)
    {
        readAttrMaps(stream, body->attrMaps);
    }
    //std::cout << "      ReaderMG2::read(), end." << std::endl;
}

std::shared_ptr<voxengine::data::stream::Uint32Array> ReaderMG2::readGridIndices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Float32Array> vertices)
{
    auto f0 = stream->readInt32(); //magic "GIDX"
    auto f1 = stream->readInt32(); //packed size

    //std::cout << "      ReaderMG2::readGridIndices(), ...f0: " << f0 << ", f1: " << f1 << ", offset: " << stream->offset << std::endl;

    auto size = vertices->getLength();
    size /= 3;
    //std::cout << "      ReaderMG2::readGridIndices(), ...step 0, vertices->getLength(): " << vertices->getLength() << ", size: " << size << std::endl;
    auto gridIndices = std::make_shared<voxengine::data::stream::Uint32Array>(size);

    //std::cout << "      ReaderMG2::readGridIndices(), ...step 1, offset: " << stream->offset << ", size: " << size << std::endl;
    auto interleaved = std::make_shared<InterleavedStream>(gridIndices, 1);

    //std::cout << "      ReaderMG2::readGridIndices(), ...step 2, interleaved->data != nullptr: " << (interleaved->data != nullptr) << std::endl;
    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());

    CTM::restoreGridIndices(gridIndices, gridIndices->getLength());

    //std::cout << "Error,  ReaderMG2::readGridIndices() end... " << std::endl;
    return gridIndices;
}
void ReaderMG2::readIndices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Int32Array> indices)
{
    stream->readInt32(); //magic "INDX"
    stream->readInt32(); //packed size

    //auto interleaved = new InterleavedStream(indices, 3);
    auto interleaved = std::make_shared<InterleavedStream>(indices, 3);

    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());

    CTM::restoreIndices(indices, indices->getLength());
}

void ReaderMG2::readVertices(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<voxengine::data::stream::Float32Array> vertices)
{
    //std::cout << "      ###$ A ReaderMG2::readVertices(), stream.offset: " << stream->offset << std::endl;
    //stream->showInfo();
    auto f0 = stream->readInt32(); //magic "VERT"
    auto f1 = stream->readInt32(); //packed size

    //std::cout << "      ###$ B ReaderMG2::readVertices(), ...f0: " << f0 << ", f1: " << f1 << ", stream.offset : " << stream->offset<< std::endl;

    auto interleaved = std::make_shared<InterleavedStream>(vertices, 3);

    //std::cout << "      ReaderMG2::readVertices(), stream.offset: " << stream->offset << std::endl;
    //showArrayViewPtr(interleaved->data, "ReaderMG2::readVertices() interleaved->data\n");
    //std::cout << "      ReaderMG2::readVertices(), ===============================================================, stream->offset: " << stream->offset << std::endl;
    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
    //std::cout << "      ReaderMG2::readVertices(), VVVV stream.offset: " << stream->offset << std::endl;

    auto gridIndices = readGridIndices(stream, vertices);

    //showArrayViewPtr(gridIndices, "ReaderMG2::readVertices() gridIndices\n");
    CTM::restoreVertices(vertices, MG2Header, gridIndices, MG2Header->vertexPrecision);
}

void ReaderMG2::readNormals(std::shared_ptr<ICTMRStream> stream, std::shared_ptr<CTMFileBody> body)
{
    auto f0 = stream->readInt32(); //magic "NORM"
    auto f1 = stream->readInt32(); //packed size

    //std::cout << "      ReaderMG2::readNormals(), ...f0: " << f0 << ", f1: " << f1 << std::endl;
    auto interleaved = std::make_shared<InterleavedStream>(body->normals, 3);
    LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());
    auto& ivs    = body->indices;
    auto  u32    = std::make_shared<voxengine::data::stream::Uint32Array>(ivs->getBuffer(), ivs->getByteOffset(), ivs->getLength());
    auto  smooth = CTM::calcSmoothNormals(u32, body->vertices);

    CTM::restoreNormals(body->normals, smooth, MG2Header->normalPrecision);
}

void ReaderMG2::readUVMaps(std::shared_ptr<ICTMRStream> stream, std::vector<UVMapType>& uvMaps)
{
    auto i = 0;
    for (; i < uvMaps.size(); ++i)
    {
        stream->readInt32(); //magic "TEXC"

        uvMaps[i].name     = stream->readString();
        uvMaps[i].filename = stream->readString();

        auto precision = stream->readFloat32();

        stream->readInt32(); //packed size

        //let interleaved = new InterleavedStream(uvMaps[i].uv, 2);
        auto interleaved = std::make_shared<InterleavedStream>(uvMaps[i].uv, 2);
        LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());

        CTM::restoreMap(uvMaps[i].uv, 2, precision);
    }
}

void ReaderMG2::readAttrMaps(std::shared_ptr<ICTMRStream> stream, std::vector<AttrMapType>& attrMaps)
{
    auto i = 0;
    for (; i < attrMaps.size(); ++i)
    {
        stream->readInt32(); //magic "ATTR"

        attrMaps[i].name = stream->readString();

        auto precision = stream->readFloat32();

        stream->readInt32(); //packed size

        //let interleaved = new InterleavedStream(attrMaps[i].attr, 4);
        auto interleaved = std::make_shared<InterleavedStream>(attrMaps[i].attr, 4);
        LZMA::decompress(stream, stream, interleaved, interleaved->data->getLength());

        CTM::restoreMap(attrMaps[i].attr, 4, precision);
    }
}
} // namespace ctm
} // namespace data
} // namespace voxengine