#ifndef ALGOMODULE_H
#define ALGOMODULE_H

#include <QByteArray>
#include <QVector>

class AlgoModule
{
public:
    /// <summary>
    /// Ctor
    /// </summary>
    AlgoModule();

    /// <summary>
    /// Compresses int[] packet data, returns it as a byte[]
    /// </summary>
    /// <param name="input">assumed to be point data (x,x,x,x,x,x,x)</param>
    /// <param name="compression">magic byte specifying the compression to use</param>
    /// <returns></returns>
    QByteArray CompressPacketData(QVector<int> input, quint8 compression);

    /// <summary>
    /// DecompressPacketData - given a compressed byte[], uncompress it to the outputBuffer
    /// </summary>
    /// <param name="input">compressed byte from the ISF stream</param>
    /// <param name="outputBuffer">prealloc'd buffer to write to</param>
    /// <returns></returns>
    uint DecompressPacketData(QByteArray input, QVector<int>& outputBuffer);

    /// <summary>
    /// Compresses property data which is already in the form of a byte[]
    /// into a compressed byte[]
    /// </summary>
    /// <param name="input">byte[] data ready to be compressed</param>
    /// <param name="compression">the compression to use</param>
    /// <returns></returns>
    QByteArray CompressPropertyData(QByteArray input, quint8 compression);

    /// <summary>
    /// Decompresses property data (from a compressed byte[] to an uncompressed byte[])
    /// </summary>
    /// <param name="input">The byte[] to decompress</param>
    /// <returns></returns>
    QByteArray DecompressPropertyData(QByteArray input);

private:
    /// <summary>
    /// Privates, lazy initialized, do not reference directly
    /// </summary>
//    HuffModule          _huffModule;
//    MultiByteCodec      _multiByteCodec;
//    DeltaDelta          _deltaDelta;
//    GorillaCodec        _gorillaCodec;
//    LZCodec             _lzCodec;

public:
    /// <summary>
    /// Static members defined in Penimc code
    /// </summary>
    static constexpr quint8 NoCompression = 0x00;
    static constexpr quint8 DefaultCompression = 0xC0;
    static constexpr quint8 IndexedHuffman = 0x80;
    static constexpr quint8 LempelZiv = 0x80;
    static constexpr quint8 DefaultBAACount = 8;
    static constexpr quint8 MaxBAACount = 10;


    static constexpr double DefaultFirstSquareRoot[] = { 1, 1, 1, 4, 9, 16, 36, 49};
};

#endif // ALGOMODULE_H
