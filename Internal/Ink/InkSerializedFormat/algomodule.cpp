#include "Internal/Ink/InkSerializedFormat/algomodule.h"

AlgoModule::AlgoModule()
{

}

/// <summary>
/// Compresses int[] packet data, returns it as a byte[]
/// </summary>
/// <param name="input">assumed to be point data (x,x,x,x,x,x,x)</param>
/// <param name="compression">magic byte specifying the compression to use</param>
/// <returns></returns>
QByteArray AlgoModule::CompressPacketData(QVector<int> input, quint8 compression)
{
    QByteArray output(input.size() * 4 + 1, 0);
    memcpy(output.data() + 1, input.data(), input.length() * 4);
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
    if (input.size() != outputBuffer.size() * 4 + 1)
    {
        throw std::exception(("Input buffer passed was shorter than expected"));
    }

    quint8 compression = input[0];
    memcpy(outputBuffer.data(), input.data() + 1, outputBuffer.size() * 4);
    return outputBuffer.size();
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
