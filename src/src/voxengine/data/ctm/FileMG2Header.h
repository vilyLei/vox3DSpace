#pragma once
#include <memory>
#include "ctmDef.h"
#include "./ICTMRStream.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class FileMG2Header
{
public:
    FileMG2Header(std::shared_ptr<ICTMRStream> stream) noexcept;
    ~FileMG2Header() = default;

    Float32 vertexPrecision;
    Float32 normalPrecision;
    Float32 lowerBoundx;
    Float32 lowerBoundy;
    Float32 lowerBoundz;
    Float32 higherBoundx;
    Float32 higherBoundy;
    Float32 higherBoundz;

    Int32 divx;
    Int32 divy;
    Int32 divz;

    Float32 sizex;
    Float32 sizey;
    Float32 sizez;
};
} // namespace ctm
} // namespace data
} // namespace voxengine