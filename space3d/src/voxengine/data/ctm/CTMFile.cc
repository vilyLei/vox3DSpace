#include <cassert>
#include "ctmUtils.h"
#include "CTMFile.h"
#include "ReaderRAW.h"
#include "ReaderMG1.h"
#include "ReaderMG2.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
CTMFile::CTMFile(std::shared_ptr<ICTMRStream> stream) :
    m_reader(nullptr)
{
    //std::cout << "CTMFile::constructor()..." << std::endl;
    load(stream);
}
CTMFile::~CTMFile()
{
    //std::cout << "CTMFile::deconstructor()..." << std::endl;
    header   = nullptr;
    body     = nullptr;
    m_reader = nullptr;
}


void CTMFile::load(std::shared_ptr<ICTMRStream> stream)
{
    header = std::make_shared<FileHeader>(stream);

    body = std::make_shared<CTMFileBody>(header);

    m_reader = getReader();
    m_reader->read(stream, body);
}

std::shared_ptr<IReaderHeader> CTMFile::getReader()
{
    std::shared_ptr<IReaderHeader> reader = nullptr;

    //std::cout << "      CTMFile::getReader(), header->compressionMethod: " << header->compressionMethod << std::endl;
    switch (header->compressionMethod)
    {
        case CompressionMethod::RAW:
            reader = std::make_shared<ReaderRAW>();
            break;
        case CompressionMethod::MG1:
            reader = std::make_shared<ReaderMG1>();
            break;
        case CompressionMethod::MG2:
            reader = std::make_shared<ReaderMG2>();
            break;
        default:
            break;
    }
    assert(reader != nullptr);
    return reader;
}
} // namespace ctm
} // namespace data
} // namespace voxengine