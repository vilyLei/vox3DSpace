#include "RangeDecoder.h"
#include "ctmUtils.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
RangeDecoder::RangeDecoder() noexcept :
    _stream(nullptr), _code(0), _range(-1)
{
    //std::cout << "RangeDecoder::deconstructor() ..." << std::endl;
}
RangeDecoder::~RangeDecoder() noexcept
{

    //std::cout << "RangeDecoder::deconstructor() ..." << std::endl;
    _code   = 0;
    _range  = -1;
    _stream = nullptr;
}

void RangeDecoder::setStream(std::shared_ptr<ICTMRStream> stream)
{
    _stream = stream;
}

void RangeDecoder::releaseStream()
{
    _stream = nullptr;
}

void RangeDecoder::init()
{
    auto i = 5;

    _code  = 0;
    _range = -1;

    while (i--)
    {
        _code = (_code << 8) | _stream->readByte();
    }
    //std::cout << "      >>>>>> RangeDecoder::init(), _code: " << _code << std::endl;
}

Uint32 RangeDecoder::decodeDirectBits(Uint32 numTotalBits)
{
    Int32 result = 0, i = numTotalBits, t;

    while (i--)
    {
        // >>> ÁãÌî³äÓÒÎ»ÒÆ
        //  _range >>>= 1;
        _range = rightMoveBitFillZero<Int32, Uint32>(_range, 1);

        // t = (_code - _range) >>> 31;
        t = rightMoveBitFillZero<Int32, Uint32>((_code - _range), 31);

        _code -= _range & (t - 1);
        result = (result << 1) | (1 - t);

        if ((_range & 0xff000000) == 0)
        {
            _code = (_code << 8) | _stream->readByte();
            _range <<= 8;
        }
    }

    //std::cout << "      >>>>&&&>> RangeDecoder::decodeDirectBits(), _code: " << _code << std::endl;
    return result;
};

Uint32 RangeDecoder::decodeBit(Int32* probs, Int32 index)
{
    auto prob = probs[index];

    //auto newBound = (_range >>> 11) * prob;
    auto newBound = rightMoveBitFillZero<Int32, Uint32>(_range, 11) * prob;

    //std::cout << "      RangeDecoder::decodeBit(), newBound: " << newBound << ", _code: " << static_cast<Uint32>( _code) << std::endl;
    Int32 f_code     = _code ^ 0x80000000;
    Int32 f_newBound = newBound ^ 0x80000000;
    //std::cout << "      RangeDecoder::decodeBit(), newBound: " << newBound << ", _code: " << _code << ",value: " << (f_code < f_newBound) << std::endl;
    //
    //(this._code ^ 0x80000000) < (newBound ^ 0x80000000)
    if (f_code < f_newBound)
    {
        _range = newBound;
        //probs[index] += (2048 - prob) >>> 5;

        probs[index] += rightMoveBitFillZero<Int32, Uint32>((2048 - prob), 5);

        f_code = _range & 0xff000000;

        if (f_code == 0)
        {
            //auto t = static_cast<Uint8>(_stream->readByte());
            //std::cout << "      RangeDecoder::decodeBit(), A3 0 _code: " << _code << ", t: " << t << std::endl;
            //_code = (_code << 8) | static_cast<Uint8>(_stream->readByte());
            _code = (_code << 8) | _stream->readByte();
            //std::cout << "      RangeDecoder::decodeBit(), A3 1 _code: " << _code << std::endl;
            _range <<= 8;
        }
        //std::cout << "      RangeDecoder::decodeBit(), A3 _code: " << _code << ", _range: " << _range << std::endl;
        return 0;
    }

    _range -= newBound;
    _code -= newBound;

    //probs[index] -= prob >>> 5;
    probs[index] -= rightMoveBitFillZero<Int32, Uint32>(prob, 5);
    f_code = _range & 0xff000000;
    if (f_code == 0)
    {
        //_code = (_code << 8) | static_cast<Uint8>(_stream->readByte());
        _code = (_code << 8) | _stream->readByte();
        _range <<= 8;
    }
    //std::cout << "      RangeDecoder::decodeBit(), B3 _code: " << _code << std::endl;
    return 1;
}

} // namespace ctm
} // namespace data
} // namespace voxengine