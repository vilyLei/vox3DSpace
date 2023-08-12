#pragma once
#include <memory>
#include "streamDef.h"
#include "ArrayBuffer.h"
namespace voxengine
{
namespace data
{
namespace stream
{
//class DataView : public std::enable_shared_from_this<DataView>
class DataView
{
public:
    DataView(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset = 0, Uint32 length = 0);
    DataView(Uint32 length);
    virtual ~DataView();


    Int32 getLength();
    Int32 getByteLength();
    Int8* getBytes();
    Int32 getByteStride();
    Int32 getByteOffset();

    std::shared_ptr<ArrayBuffer> getBuffer();
    //std::shared_ptr<DataView>    getSharedPtr()
    //{
    //    return shared_from_this();
    //}

protected:
    virtual void buildData() = 0;
    void         buildBytesData(Int32 step);
    void         assertIndex(Int32 i);

    Int32                        m_buildType;
    Int32                        m_byteOffset;
    Int32                        m_length;
    Int32                        m_byteStride;
    Int32                        m_byteLength;
    Int8*                        m_bytes;
    std::shared_ptr<ArrayBuffer> m_arrBuf;

private:
};

} // namespace base
} // namespace voxengine
}