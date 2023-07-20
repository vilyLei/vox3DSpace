#include <cmath>
#include "LZMA.h"

namespace voxengine
{
namespace data
{
namespace ctm
{

BitTreeDecoder::BitTreeDecoder(Uint32 numBitLevels) noexcept :
    _models(nullptr), _numBitLevels(numBitLevels)
{
    //std::cout << "BitTreeDecoder::constructor()..." << std::endl;
}
BitTreeDecoder::~BitTreeDecoder() noexcept
{
    //std::cout << "BitTreeDecoder::deconstructor()..." << std::endl;
    DELETE_OBJ_ARRAY_NAKED_PTR(_models);
}

void BitTreeDecoder::init()
{
    auto size = 1 << _numBitLevels;
    _models   = new Int32[size];
    LZMA::initBitModels(_models, size);
};

Int32 BitTreeDecoder::decode(RangeDecoder* rangeDecoder)
{
    Int32 m = 1, i = _numBitLevels;

    while (i--)
    {
        m = (m << 1) | rangeDecoder->decodeBit(_models, m);
    }
    return m - (1 << _numBitLevels);
};

Int32 BitTreeDecoder::reverseDecode(RangeDecoder* rangeDecoder)
{
    Int32  m = 1, symbol = 0, bit;
    Uint32 i = 0;

    for (; i < _numBitLevels; ++i)
    {
        bit = rangeDecoder->decodeBit(_models, m);
        m   = (m << 1) | bit;
        symbol |= bit << i;
    }
    return symbol;
}

LenDecoder::LenDecoder() noexcept :
    _choice(nullptr), _numPosStates(0)
{
    _highCoder = new BitTreeDecoder(8);
}
LenDecoder::~LenDecoder() noexcept
{
    //std::cout << "LenDecoder::deconstructor()...\n";
    DELETE_OBJ_ARRAY_NAKED_PTR(_choice);
    DELETE_OBJ_NAKED_PTR(_highCoder);

    for (auto i = 0; i < _lowCoder.size(); ++i)
    {
        DELETE_OBJ_NAKED_PTR(_lowCoder[i]);
        DELETE_OBJ_NAKED_PTR(_midCoder[i]);
        _lowCoder[i] = nullptr;
        _midCoder[i] = nullptr;
    }
    _lowCoder.clear();
    _midCoder.clear();

    std::vector<BitTreeDecoder*>().swap(_lowCoder);
    std::vector<BitTreeDecoder*>().swap(_midCoder);
}

void LenDecoder::create(Uint32 numPosStates)
{
    //_lowCoder = new BitTreeDecoder*[numPosStates]{};
    //_midCoder = new BitTreeDecoder*[numPosStates]{};
    _lowCoder.reserve(numPosStates);
    _midCoder.reserve(numPosStates);
    for (; _numPosStates < numPosStates; ++_numPosStates)
    {
        _lowCoder.push_back(new BitTreeDecoder(3));
        _midCoder.push_back(new BitTreeDecoder(3));
    }
};

void LenDecoder::init()
{
    Uint32 i = _numPosStates;

    _choice = new Int32[2]{};
    LZMA::initBitModels(_choice, 2);
    while (i--)
    {
        _lowCoder[i]->init();
        _midCoder[i]->init();
    }
    _highCoder->init();
};

Int32 LenDecoder::decode(RangeDecoder* rangeDecoder, Uint32 posState)
{

    if (rangeDecoder->decodeBit(_choice, 0) == 0)
    {
        return _lowCoder[posState]->decode(rangeDecoder);
    }
    if (rangeDecoder->decodeBit(_choice, 1) == 0)
    {
        return 8 + _midCoder[posState]->decode(rangeDecoder);
    }
    return 16 + _highCoder->decode(rangeDecoder);
}


Decoder2::Decoder2() noexcept :
    _decoders(nullptr)
{
}
Decoder2::~Decoder2() noexcept
{
    DELETE_OBJ_ARRAY_NAKED_PTR(_decoders);
}

void Decoder2::init()
{
    auto size = 0x300;
    _decoders = new Int32[size]{};
    LZMA::initBitModels(_decoders, size);
};

Int32 Decoder2::decodeNormal(RangeDecoder* rangeDecoder)
{
    Int32 symbol = 1;

    do {
        symbol = (symbol << 1) | rangeDecoder->decodeBit(_decoders, symbol);
    } while (symbol < 0x100);

    return symbol & 0xff;
}

Int32 Decoder2::decodeWithMatchByte(RangeDecoder* rangeDecoder, Uint32 matchByte)
{
    Int32 symbol = 1, matchBit, bit;

    do {
        matchBit = (matchByte >> 7) & 1;
        matchByte <<= 1;
        bit    = rangeDecoder->decodeBit(_decoders, ((1 + matchBit) << 8) + symbol);
        symbol = (symbol << 1) | bit;
        if (matchBit != bit)
        {
            while (symbol < 0x100)
            {
                symbol = (symbol << 1) | rangeDecoder->decodeBit(_decoders, symbol);
            }
            break;
        }
    } while (symbol < 0x100);

    return symbol & 0xff;
}


LiteralDecoder::LiteralDecoder() noexcept :
    _numPosBits(0), _posMask(0), _numPrevBits(0)
{
    //std::cout << "LiteralDecoder::constructor() ..." << std::endl;
}
LiteralDecoder::~LiteralDecoder() noexcept
{

    //std::cout << "LiteralDecoder::deconstructor() ..." << std::endl;

    for (auto i = 0; i < _coders.size(); ++i)
    {
        DELETE_OBJ_NAKED_PTR(_coders[i]);
    }
    _coders.clear();
    std::vector<Decoder2*>().swap(_coders);
    //DELETE_OBJ_ARRAY_NAKED_PTR(_coders);
}

void LiteralDecoder::create(Int32 numPosBits, Int32 numPrevBits)
{
    Int32 i;

    if (_coders.size() > 0 && (_numPrevBits == numPrevBits) && (_numPosBits == numPosBits))
    {
        return;
    }
    _numPosBits  = numPosBits;
    _posMask     = (1 << numPosBits) - 1;
    _numPrevBits = numPrevBits;

    i = 1 << (_numPrevBits + _numPosBits);
    //_coders = new Decoder2* [i] {};
    _coders.reserve(i);

    while (i--)
    {
        _coders.push_back(new Decoder2());
    }
}
void LiteralDecoder::init()
{
    Uint32 i = 1 << (_numPrevBits + _numPosBits);
    while (i--)
    {
        _coders[i]->init();
    }
}
Decoder2* LiteralDecoder::getDecoder(Int32 pos, Int32 prevByte)
{
    //return _coders[((pos & _posMask) << _numPrevBits) + ((prevByte & 0xff) >>> (8 - _numPrevBits))];
    return _coders[((pos & _posMask) << _numPrevBits) + ((prevByte & 0xff) >> (8 - _numPrevBits))];
}

Decoder::Decoder() noexcept :
    //_outWindow(nullptr),
    //_rangeDecoder(nullptr),
    //_isMatchDecoders(nullptr),
    //_isRepDecoders(nullptr),
    //_isRepG0Decoders(nullptr),
    //_isRepG1Decoders(nullptr),
    //_isRepG2Decoders(nullptr),
    //_isRep0LongDecoders(nullptr),
    //_posDecoders(nullptr),
    //_posAlignDecoder(nullptr),

    //_lenDecoder(nullptr),
    //_repLenDecoder(nullptr),
    //_literalDecoder(nullptr),
    _dictionarySize(-1),
    _dictionarySizeCheck(-1),
    _posStateMask(0)
{

    //std::cout << "Decoder::constructor() ..." << std::endl;
    _outWindow      = new OutWindow();
    _rangeDecoder   = new RangeDecoder();
    _lenDecoder     = new LenDecoder();
    _repLenDecoder  = new LenDecoder();
    _literalDecoder = new LiteralDecoder();

    _posAlignDecoder = new BitTreeDecoder(4);

    _posSlotDecoder[0] = new BitTreeDecoder(6);
    _posSlotDecoder[1] = new BitTreeDecoder(6);
    _posSlotDecoder[2] = new BitTreeDecoder(6);
    _posSlotDecoder[3] = new BitTreeDecoder(6);
}
Decoder::~Decoder() noexcept
{
    //std::cout << "Decoder::deconstructor() ..." << std::endl;
    DELETE_OBJ_NAKED_PTR(_outWindow);
    DELETE_OBJ_NAKED_PTR(_rangeDecoder);
    DELETE_OBJ_NAKED_PTR(_lenDecoder)
    DELETE_OBJ_NAKED_PTR(_repLenDecoder);
    DELETE_OBJ_NAKED_PTR(_posAlignDecoder);
    DELETE_OBJ_NAKED_PTR(_literalDecoder);

    DELETE_OBJ_ARRAY_NAKED_PTR(_isMatchDecoders);
    DELETE_OBJ_ARRAY_NAKED_PTR(_isRepDecoders);
    DELETE_OBJ_ARRAY_NAKED_PTR(_isRepG0Decoders);
    DELETE_OBJ_ARRAY_NAKED_PTR(_isRepG1Decoders);
    DELETE_OBJ_ARRAY_NAKED_PTR(_isRepG2Decoders);
    DELETE_OBJ_ARRAY_NAKED_PTR(_isRep0LongDecoders);
    DELETE_OBJ_ARRAY_NAKED_PTR(_posDecoders);


    for (int i = 0; i < 4; ++i)
    {
        DELETE_OBJ_NAKED_PTR(_posSlotDecoder[i]);
    }
}

bool Decoder::setDictionarySize(Int32 dictionarySize)
{
    if (dictionarySize < 0)
    {
        return false;
    }
    if (_dictionarySize != dictionarySize)
    {
        _dictionarySize      = dictionarySize;
        _dictionarySizeCheck = std::max(_dictionarySize, 1);
        _outWindow->create(std::max(_dictionarySizeCheck, 4096));
    }
    return true;
}

bool Decoder::setLcLpPb(Uint32 lc, Uint32 lp, Uint32 pb)
{
    Uint32 numPosStates = 1 << pb;

    if (lc > 8 || lp > 4 || pb > 4)
    {
        return false;
    }

    _literalDecoder->create(lp, lc);

    _lenDecoder->create(numPosStates);
    _repLenDecoder->create(numPosStates);
    _posStateMask = numPosStates - 1;

    return true;
}

void Decoder::init()
{
    _outWindow->init(false);
    _isMatchDecoders = new Int32[192];
    LZMA::initBitModels(_isMatchDecoders, 192);
    _isRep0LongDecoders = new Int32[192];
    LZMA::initBitModels(_isRep0LongDecoders, 192);
    _isRepDecoders = new Int32[12];
    LZMA::initBitModels(_isRepDecoders, 12);

    _isRepG0Decoders = new Int32[12];
    LZMA::initBitModels(_isRepG0Decoders, 12);
    _isRepG1Decoders = new Int32[12];
    LZMA::initBitModels(_isRepG1Decoders, 12);
    _isRepG2Decoders = new Int32[12];
    LZMA::initBitModels(_isRepG2Decoders, 12);
    _posDecoders = new Int32[114];
    LZMA::initBitModels(_posDecoders, 114);

    _literalDecoder->init();

    Uint32 i = 4;

    while (i--)
    {
        _posSlotDecoder[i]->init();
    }

    _lenDecoder->init();
    _repLenDecoder->init();
    _posAlignDecoder->init();
    _rangeDecoder->init();
}

bool Decoder::decode(std::shared_ptr<ICTMRStream> inStream,
                     std::shared_ptr<ICTMWStream> outStream,
                     Int32                        outSize)
{
    Int32 state = 0, rep0 = 0, rep1 = 0, rep2 = 0, rep3 = 0, prevByte = 0,
          len, distance, posSlot, numDirectBits;

    Int32 posState = 0, nowPos64 = 0;

    Decoder2* decoder2 = nullptr;

    _rangeDecoder->setStream(inStream);
    _outWindow->setStream(outStream);

    init();

    //std::cout << "      Decoder::decode(), A1 stream.offset: " << inStream->offset << ",nowPos64: " << nowPos64 << ",outSize: " << outSize << ", _posStateMask: " << _posStateMask << std::endl;
    while (outSize < 0 || nowPos64 < outSize)
    {
        //std::cout << "                  *****Decoder::decode(),state: " << state << ", posState: " << posState << ", nowPos64: " << nowPos64 << std::endl;
        posState = nowPos64 & _posStateMask;

        if (_rangeDecoder->decodeBit(_isMatchDecoders, (state << 4) + posState) == 0)
        {
            decoder2 = _literalDecoder->getDecoder(nowPos64++, prevByte);

            //std::cout << "      Decoder::decode(), F2 A0 stream.offset: " << inStream->offset << std::endl;
            if (state >= 7)
            {
                prevByte = decoder2->decodeWithMatchByte(_rangeDecoder, _outWindow->getByte(rep0));
            }
            else
            {
                prevByte = decoder2->decodeNormal(_rangeDecoder);
            }
            _outWindow->putByte(prevByte);

            state = state < 4 ? 0 : state - (state < 10 ? 3 : 6);
        }
        else
        {

            //std::cout << "      Decoder::decode(), F2 B0 stream.offset: " << inStream->offset << std::endl;
            if (_rangeDecoder->decodeBit(_isRepDecoders, state) == 1)
            {
                len = 0;
                if (_rangeDecoder->decodeBit(_isRepG0Decoders, state) == 0)
                {
                    if (_rangeDecoder->decodeBit(_isRep0LongDecoders, (state << 4) + posState) == 0)
                    {
                        state = state < 7 ? 9 : 11;
                        len   = 1;
                    }
                }
                else
                {
                    if (_rangeDecoder->decodeBit(_isRepG1Decoders, state) == 0)
                    {
                        distance = rep1;
                    }
                    else
                    {
                        if (_rangeDecoder->decodeBit(_isRepG2Decoders, state) == 0)
                        {
                            distance = rep2;
                        }
                        else
                        {
                            distance = rep3;
                            rep3     = rep2;
                        }
                        rep2 = rep1;
                    }
                    rep1 = rep0;
                    rep0 = distance;
                }
                if (len == 0)
                {
                    len = 2 + _repLenDecoder->decode(_rangeDecoder, posState);
                    //std::cout << "      Decoder::decode(), C 08 len: " << len << std::endl;
                    state = state < 7 ? 8 : 11;
                }

                //std::cout << "      Decoder::decode(), C 09 len: " << len << std::endl;
            }
            else
            {
                rep3 = rep2;
                rep2 = rep1;
                rep1 = rep0;

                len   = 2 + _lenDecoder->decode(_rangeDecoder, posState);
                state = state < 7 ? 7 : 10;

                posSlot = _posSlotDecoder[len <= 5 ? len - 2 : 3]->decode(_rangeDecoder);
                if (posSlot >= 4)
                {

                    numDirectBits = (posSlot >> 1) - 1;
                    rep0          = (2 | (posSlot & 1)) << numDirectBits;

                    if (posSlot < 14)
                    {
                        rep0 += LZMA::reverseDecode2(_posDecoders,
                                                     rep0 - posSlot - 1, _rangeDecoder, numDirectBits);
                    }
                    else
                    {
                        rep0 += _rangeDecoder->decodeDirectBits(numDirectBits - 4) << 4;
                        rep0 += _posAlignDecoder->reverseDecode(_rangeDecoder);
                        if (rep0 < 0)
                        {
                            if (rep0 == -1)
                            {
                                break;
                            }
                            return false;
                        }
                    }
                }
                else
                {
                    rep0 = posSlot;
                }
                //std::cout << "      Decoder::decode(), D 09 len: " << len << std::endl;
            }

            if (rep0 >= nowPos64 || rep0 >= _dictionarySizeCheck)
            {
                return false;
            }

            //std::cout << "      Decoder::decode(), F2 E0 stream.offset: " << inStream->offset << ", len: " << len << std::endl;
            _outWindow->copyBlock(rep0, len);
            //std::cout << "      Decoder::decode(), F2 F0 stream.offset: " << inStream->offset << ", len: " << len << std::endl;
            nowPos64 += len;
            prevByte = _outWindow->getByte(0);
            //std::cout << "      ##############    Decoder::decode(), F2 F5 stream.offset: " << inStream->offset << ", prevByte: " << prevByte << std::endl;
        }
        //std::cout << "                  $$$$$ #######    Decoder::decode(), F2 F6 stream.offset: " << inStream->offset << std::endl;
    }
    //std::cout << "      Decoder::decode(), stream.offset: " << inStream->offset << std::endl;
    //std::cout << "      Decoder::decode(), F"<< std::endl;

    _outWindow->flush();
    _outWindow->releaseStream();
    _rangeDecoder->releaseStream();

    return true;
}

bool Decoder::setDecoderProperties(std::shared_ptr<ICTMRStream> properties)
{
    Int32 value, lc, lp, pb, dictionarySize;

    // if (properties.size < 5) {
    //     return false;
    // }

    value = properties->readByte();
    lc    = value % 9;
    value = ~~(value / 9);
    lp    = value % 5;
    pb    = ~~(value / 5);

    if (!setLcLpPb(lc, lp, pb))
    {
        return false;
    }

    dictionarySize = properties->readByte();
    dictionarySize |= properties->readByte() << 8;
    dictionarySize |= properties->readByte() << 16;
    dictionarySize += properties->readByte() * 16777216;

    return setDictionarySize(dictionarySize);
}

LZMA::LZMA()
{
}

LZMA::~LZMA()
{
}
void LZMA::initBitModels(Int32* probs, Uint32 len)
{
    while (len--)
    {
        probs[len] = 1024;
    }
}
Int32 LZMA::reverseDecode2(Int32* models, Uint32 startIndex, RangeDecoder* rangeDecoder, Uint32 numBitLevels)
{
    Int32  m = 1, symbol = 0, bit;
    Uint32 i = 0;

    for (; i < numBitLevels; ++i)
    {
        bit = rangeDecoder->decodeBit(models, startIndex + m);
        m   = (m << 1) | bit;
        symbol |= bit << i;
    }
    return symbol;
}
bool LZMA::decompress(std::shared_ptr<ICTMRStream> properties, std::shared_ptr<ICTMRStream> inStream, std::shared_ptr<ICTMWStream> outStream, Int32 outSize)
{
    std::shared_ptr<Decoder> decoder = std::make_shared<Decoder>();
    if (!decoder->setDecoderProperties(properties))
    {
        throw "Incorrect stream properties";
    }
    if (!decoder->decode(inStream, outStream, outSize))
    {
        throw "Error in data stream";
    }
    return true;
}

} // namespace ctm
} // namespace data
} // namespace voxengine