#pragma once
#include <string>
#include <memory>
#include "ctmDef.h"
#include "ctmUtils.h"
#include "ICTMRStream.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class FileHeader
{
public:
    FileHeader(std::shared_ptr<ICTMRStream> stream) noexcept;
    virtual ~FileHeader() noexcept;

    Int32       fileFormat;
    Int32       compressionMethod;
    Int32       vertexCount;
    Int32       triangleCount;
    Int32       uvMapCount;
    Int32       attrMapCount;
    Int32       flags;
    std::string comment;

    Int32 hasNormals();
};
} // namespace ctm
} // namespace data
} // namespace voxengine