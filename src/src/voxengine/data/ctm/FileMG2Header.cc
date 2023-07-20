#include "FileMG2Header.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
FileMG2Header::FileMG2Header(std::shared_ptr<ICTMRStream> stream) noexcept
{
    stream->readInt32(); //magic "MG2H"

    vertexPrecision = stream->readFloat32();
    normalPrecision = stream->readFloat32();
    lowerBoundx     = stream->readFloat32();
    lowerBoundy     = stream->readFloat32();
    lowerBoundz     = stream->readFloat32();
    higherBoundx    = stream->readFloat32();
    higherBoundy    = stream->readFloat32();
    higherBoundz    = stream->readFloat32();

    divx = stream->readInt32();
    divy = stream->readInt32();
    divz = stream->readInt32();

    sizex = (higherBoundx - lowerBoundx) / divx;
    sizey = (higherBoundy - lowerBoundy) / divy;
    sizez = (higherBoundz - lowerBoundz) / divz;
}
} // namespace ctm
} // namespace data
} // namespace voxengine