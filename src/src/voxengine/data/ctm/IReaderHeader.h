#pragma once
#include <memory>
#include "ICTMRStream.h"
#include "CTMFileBody.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class IReaderHeader
{
public:
    virtual void read(std::shared_ptr<ICTMRStream> pstream, std::shared_ptr<CTMFileBody> pbody) = 0;
};
} // namespace ctm
} // namespace data
} // namespace voxengine