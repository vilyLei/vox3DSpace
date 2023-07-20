#pragma once
#include <memory>
#include "ICTMRStream.h"
#include "FileHeader.h"
#include "CTMFileBody.h"
#include "IReaderHeader.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
class CTMFile
{
public:
    CTMFile(std::shared_ptr<ICTMRStream> stream);
    ~CTMFile();


    std::shared_ptr<FileHeader>  header;
    std::shared_ptr<CTMFileBody> body;

    std::shared_ptr<IReaderHeader> getReader();

private:
    std::shared_ptr<IReaderHeader> m_reader;
    void                           load(std::shared_ptr<ICTMRStream> stream);
};
} // namespace ctm
} // namespace data
} // namespace voxengine