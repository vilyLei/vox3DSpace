#pragma once

#include <memory>
#include <vector>
#include "ctmDef.h"
#include "ctmUtils.h"
#include "../stream/DataView.h"
#include "../stream/Uint32Array.h"
#include "../stream/Float32Array.h"
#include "./FileHeader.h"
#include "./FileMG2Header.h"
#include "./InterleavedStream.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class CTM
{
public:
    CTM(std::shared_ptr<FileHeader> header)
    noexcept;
    virtual ~CTM() noexcept;

    static std::shared_ptr<InterleavedStream>                     InterleavedStream(std::shared_ptr<voxengine::data::stream::DataView> pdata, Int32 count);
    static void                                                   restoreIndices(std::shared_ptr<voxengine::data::stream::Int32Array> pindices, Int32 len);
    static std::shared_ptr<voxengine::data::stream::Float32Array> calcSmoothNormals(std::shared_ptr<voxengine::data::stream::Uint32Array> pindices, std::shared_ptr<voxengine::data::stream::Float32Array> pvertices);
    static void                                                   restoreVertices(std::shared_ptr<voxengine::data::stream::Float32Array> pvertices, std::shared_ptr<FileMG2Header> grid, std::shared_ptr<voxengine::data::stream::Uint32Array> gridIndices, Float32 precision);
    static void                                                   restoreGridIndices(std::shared_ptr<voxengine::data::stream::Uint32Array> pgridIndices, Int32 len);
    static void                                                   restoreNormals(std::shared_ptr<voxengine::data::stream::Float32Array> pnormals, std::shared_ptr<voxengine::data::stream::Float32Array> psmooth, Float32 precision);
    static void                                                   restoreMap(std::shared_ptr<voxengine::data::stream::DataView> pmap, Int32 count, Float32 precision);
};
} // namespace ctm
} // namespace data
} // namespace voxengine