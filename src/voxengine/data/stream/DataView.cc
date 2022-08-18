#include <cassert>
#include "DataView.h"
namespace voxengine
{
namespace data
{
namespace stream
{
DataView::DataView(std::shared_ptr<ArrayBuffer> arrBuf, Uint32 byteOffset, Uint32 length) :
    m_buildType(0), m_arrBuf(arrBuf), m_byteLength(arrBuf->getByteLength()), m_byteOffset(byteOffset), m_length(length), m_bytes(arrBuf->getBytes())
{
    //std::cout << "DataView::constructor() type 0 ..." << std::endl;
}
DataView::DataView(Uint32 length) :
    m_buildType(1), m_byteLength(0), m_byteOffset(0), m_length(length), m_bytes(nullptr)
{
    //std::cout << "DataView::constructor() type 1 ..." << std::endl;
}

DataView::~DataView()
{
    // DELETE_OBJ_ARRAY_NAKED_PTR(m_arrBuf);
    m_bytes  = nullptr;
    m_arrBuf = nullptr;
    //std::cout << "DataView::deconstructor() type " << m_buildType << "..." << std::endl;
}

Int32 DataView::getLength()
{
    return m_length;
}
Int32 DataView::getByteLength()
{
    return m_byteLength;
}
Int8* DataView::getBytes()
{
    return m_bytes;
}
std::shared_ptr<ArrayBuffer> DataView::getBuffer()
{
    return m_arrBuf;
}

void DataView::buildBytesData(Int32 step)
{
    auto bitMoveStep = step >> 1;
    if (m_buildType < 1)
    {
        auto bytesLen = m_length > 0 ? ((m_length << bitMoveStep) + m_byteOffset) : m_byteLength;
        assert(m_byteOffset % step == 0 && bytesLen % step == 0 && bytesLen <= m_byteLength);
        if (m_length < 1) m_length = bytesLen >> bitMoveStep;
        m_bytes = m_arrBuf->getBytes() + m_byteOffset;
        m_byteLength = m_length * step;
    }
    else
    {
        assert(m_length > 0);
        m_byteLength = m_length << bitMoveStep;
        m_arrBuf     = std::make_shared<ArrayBuffer>(m_byteLength);
        m_bytes      = m_arrBuf->getBytes();
    }
}

Int32 DataView::getByteOffset()
{
    return m_byteOffset;
}
void DataView::assertIndex(Int32 i)
{
    assert(i >= 0 && i < m_length);
}
} // namespace base
} // namespace voxengine
}
