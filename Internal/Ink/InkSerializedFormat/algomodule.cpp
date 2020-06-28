#include "Internal/Ink/InkSerializedFormat/algomodule.h"

INKCANVAS_BEGIN_NAMESPACE

AlgoModule::AlgoModule()
{

}

/// <summary>
/// Compresses int[] packet data, returns it as a byte[]
/// </summary>
/// <param name="input">assumed to be point data (x,x,x,x,x,x,x)</param>
/// <param name="compression">magic byte specifying the compression to use</param>
/// <returns></returns>
QByteArray AlgoModule::CompressPacketData(Array<int> input, quint8 compression)
{
    QByteArray output(input.Length() * 4 + 1, 0);
    compression = (quint8)(input.Length() * 4);
    output[0] = compression;
    memcpy(output.data() + 1, &input[0], input.Length() * 4);
    return output;
}

/// <summary>
/// DecompressPacketData - given a compressed byte[], uncompress it to the outputBuffer
/// </summary>
/// <param name="input">compressed byte from the ISF stream</param>
/// <param name="outputBuffer">prealloc'd buffer to write to</param>
/// <returns></returns>
uint AlgoModule::DecompressPacketData(QByteArray input, QVector<int>& outputBuffer)
{
    if (input.size() < 2)
    {
        throw std::runtime_error(("Input buffer passed was shorter than expected"));
    }

    quint8 compression = (quint8)(outputBuffer.size() * 4);

    if (compression != (quint8)input[0])
    {
        throw std::runtime_error(("Input buffer passed was shorter than expected"));
    }

    memcpy(outputBuffer.data(), input.data() + 1, outputBuffer.size() * 4);
    return outputBuffer.size() * 4 + 1;
}

/// <summary>
/// Compresses property data which is already in the form of a byte[]
/// into a compressed byte[]
/// </summary>
/// <param name="input">byte[] data ready to be compressed</param>
/// <param name="compression">the compression to use</param>
/// <returns></returns>
QByteArray AlgoModule::CompressPropertyData(QByteArray input, quint8 compression)
{
    QByteArray output(input.size() + 1, 0);
    memcpy(output.data() + 1, input.data(), input.length());
    return output;
}

/// <summary>
/// Decompresses property data (from a compressed byte[] to an uncompressed byte[])
/// </summary>
/// <param name="input">The byte[] to decompress</param>
/// <returns></returns>
QByteArray AlgoModule::DecompressPropertyData(QByteArray input)
{
    QByteArray output(input.size() - 1, 0);
    memcpy(output.data(), input.data() + 1, input.size() - 1);
    return output;
}

INKCANVAS_END_NAMESPACE
