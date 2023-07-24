#pragma once
#include <memory>
#include <vector>
#include "ctmDef.h"
#include "ctmUtils.h"
#include "../stream/DataView.h"
#include "../stream/Float32Array.h"
#include "../stream/Uint32Array.h"
#include "./FileHeader.h"

namespace voxengine
{
namespace data
{
namespace ctm
{
class CTMFileBody
{
public:
    CTMFileBody(std::shared_ptr<FileHeader> header) noexcept;
    virtual ~CTMFileBody();

    std::shared_ptr<voxengine::data::stream::Uint32Array>  indices;
    std::shared_ptr<voxengine::data::stream::Float32Array> vertices;
    std::shared_ptr<voxengine::data::stream::Float32Array> normals;
    std::vector<UVMapType>                                 uvMaps;
    std::vector<AttrMapType>                               attrMaps;

    std::shared_ptr<voxengine::data::stream::Float32Array> getUVSAt(Int32 i);
};
} // namespace ctm
} // namespace data
} // namespace voxengine