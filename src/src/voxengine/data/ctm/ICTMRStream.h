#pragma once
#include <string>
#include <memory>
#include "ctmDef.h"
#include "../stream/Uint8Array.h"
#include "../stream/Int32Array.h"
#include "../stream/Float32Array.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
class ICTMRStream
{
public:
    ICTMRStream() :
        data(nullptr), offset(0) {}

    virtual ~ICTMRStream()
    {
        data   = nullptr;
        offset = 0;
        //std::cout << "ICTMRStream::deconstructor()..." << std::endl;
    }
    /**
     * uint8 array
     */
    std::shared_ptr<voxengine::data::stream::Uint8Array> data;
    Int32                                                offset;


    static const double TWO_POW_MINUS23;
    static const double TWO_POW_MINUS126;

    virtual void                                                   showInfo()                                                                   = 0;
    virtual Uint8                                                  readByte()                                                                   = 0;
    virtual Int32                                                  readInt32()                                                                  = 0;
    virtual Float32                                                readFloat32()                                                                = 0;
    virtual std::string                                            readString()                                                                 = 0;
    virtual std::shared_ptr<voxengine::data::stream::Int32Array>   readArrayInt32(std::shared_ptr<voxengine::data::stream::Int32Array> arr)     = 0;
    virtual std::shared_ptr<voxengine::data::stream::Float32Array> readArrayFloat32(std::shared_ptr<voxengine::data::stream::Float32Array> arr) = 0;
};
} // namespace ctm
} // namespace data
} // namespace voxengine