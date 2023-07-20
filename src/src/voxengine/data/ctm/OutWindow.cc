#include "OutWindow.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
OutWindow::OutWindow() noexcept :
    _pos(0), _streamPos(0), _windowSize(0), _stream(nullptr), _buffer(nullptr)
{
    //std::cout << "OutWindow::constructor() ..." << std::endl;
}
OutWindow::~OutWindow() noexcept
{
    //std::cout << "OutWindow::deconstructor() XX..." << std::endl;
    _pos       = 0;
    _streamPos = 0;
    _stream    = nullptr;
    DELETE_OBJ_ARRAY_NAKED_PTR(_buffer);
}
void OutWindow::create(Uint32 windowSize)
{
    if ((!_buffer) || (_windowSize != windowSize))
    {
        DELETE_OBJ_ARRAY_NAKED_PTR(_buffer);
        _buffer = new Uint32[windowSize]{0};
    }
    _windowSize = windowSize;
    //std::cout << "OutWindow::create() XXXXXX B, _windowSize: " << _windowSize << std::endl;
    _pos       = 0;
    _streamPos = 0;
}

void OutWindow::flush()
{
    Uint32 size = _pos - _streamPos;
    if (size != 0)
    {
        while (size--)
        {
            _stream->writeByte(_buffer[_streamPos++]);
        }
        if (_pos >= _windowSize)
        {
            _pos = 0;
        }
        _streamPos = _pos;
    }
}

void OutWindow::releaseStream()
{
    flush();
    _stream = nullptr;
}

void OutWindow::setStream(std::shared_ptr<ICTMWStream> stream)
{
    releaseStream();
    _stream = stream;
}

void OutWindow::init(bool solid)
{
    if (!solid)
    {
        _streamPos = 0;
        _pos       = 0;
    }
}

void OutWindow::copyBlock(Int32 distance, Int32 len)
{
    auto pos = _pos - distance - 1;
    if (pos < 0)
    {
        pos += _windowSize;
    }
    while (len--)
    {
        if (pos >= _windowSize)
        {
            pos = 0;
        }
        _buffer[_pos++] = _buffer[pos++];
        if (_pos >= _windowSize)
        {
            flush();
        }
    }
}

void OutWindow::putByte(Uint8 b)
{
    _buffer[_pos++] = b;
    if (_pos >= _windowSize)
    {
        flush();
    }
}

Uint8 OutWindow::getByte(Int32 distance)
{
    auto pos = _pos - distance - 1;

    if (pos < 0)
    {
        pos += _windowSize;
    }
    return _buffer[pos];
}

} // namespace ctm
} // namespace data
} // namespace voxengine
