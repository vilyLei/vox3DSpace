#pragma once

#include <memory>
#include <vector>
#include "ctmDef.h"
#include "ICTMRStream.h"
#include "ICTMWStream.h"
#include "IDecoder.h"
#include "OutWindow.h"
#include "RangeDecoder.h"
namespace voxengine
{
namespace data
{
namespace ctm
{
class LZMA;
class BitTreeDecoder;

class BitTreeDecoder : public IDecoder
{
public:
    BitTreeDecoder(Uint32 numBitLevels) noexcept;
    virtual ~BitTreeDecoder() noexcept;

    Int32* _models;
    Uint32 _numBitLevels;

    void  init() override;
    Int32 decode(RangeDecoder* rangeDecoder);
    Int32 reverseDecode(RangeDecoder* rangeDecoder);
};

class LenDecoder : public IDecoder
{
public:
    LenDecoder() noexcept;
    virtual ~LenDecoder() noexcept;

    Int32* _choice;

    std::vector<BitTreeDecoder*> _lowCoder;
    std::vector<BitTreeDecoder*> _midCoder;

    BitTreeDecoder* _highCoder;
    Uint32          _numPosStates;

    void  create(Uint32 numPosStates);
    void  init() override;
    Int32 decode(RangeDecoder* rangeDecoder, Uint32 posState);
};

class Decoder2 : public IDecoder
{
public:
    Decoder2() noexcept;
    virtual ~Decoder2() noexcept;

    Int32* _decoders;
    void   init() override;
    Int32  decodeNormal(RangeDecoder* rangeDecoder);
    Int32  decodeWithMatchByte(RangeDecoder* rangeDecoder, Uint32 matchByte);
};

class LiteralDecoder
{
public:
    LiteralDecoder() noexcept;
    virtual ~LiteralDecoder() noexcept;

    std::vector<Decoder2*> _coders;

    Uint32 _numPosBits;
    Uint32 _posMask;
    Uint32 _numPrevBits;

    void      create(Int32 numPosBits, Int32 numPrevBits);
    void      init();
    Decoder2* getDecoder(Int32 pos, Int32 prevByte);
};



class Decoder : public IDecoder
{
public:
    Decoder() noexcept;
    virtual ~Decoder() noexcept;


    OutWindow*      _outWindow;
    RangeDecoder*   _rangeDecoder;
    Int32*          _isMatchDecoders;
    Int32*          _isRepDecoders;
    Int32*          _isRepG0Decoders;
    Int32*          _isRepG1Decoders;
    Int32*          _isRepG2Decoders;
    Int32*          _isRep0LongDecoders;
    Int32*          _posDecoders;
    BitTreeDecoder* _posSlotDecoder[4];

    BitTreeDecoder* _posAlignDecoder;
    LenDecoder*     _lenDecoder;
    LenDecoder*     _repLenDecoder;
    LiteralDecoder* _literalDecoder;

    Int32 _dictionarySize;
    Int32 _dictionarySizeCheck;
    Int32 _posStateMask;

    bool setDictionarySize(Int32 dictionarySize);
    bool setLcLpPb(Uint32 lc, Uint32 lp, Uint32 pb);
    void init() override;
    bool decode(std::shared_ptr<ICTMRStream> inStream,
                std::shared_ptr<ICTMWStream> outStream,
                Int32                        outSize);
    bool setDecoderProperties(std::shared_ptr<ICTMRStream> properties);
};

class LZMA
{
public:
    static void  initBitModels(Int32* probs, Uint32 len);
    static Int32 reverseDecode2(Int32* models, Uint32 startIndex, RangeDecoder* rangeDecoder, Uint32 numBitLevels);
    static bool  decompress(std::shared_ptr<ICTMRStream> properties, std::shared_ptr<ICTMRStream> inStream, std::shared_ptr<ICTMWStream> outStream, Int32 outSize);

private:
    LZMA();
    ~LZMA();
};


} // namespace ctm
} // namespace data
} // namespace voxengine