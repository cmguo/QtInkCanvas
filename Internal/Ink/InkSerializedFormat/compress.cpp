#include "Internal/Ink/InkSerializedFormat/compress.h"
#include "Internal/Ink/InkSerializedFormat/algomodule.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// DecompressPacketData - take a byte[] or a subset of a byte[] and decompresses it into
///     an int[] of packet data (for example, x's in a Stroke)
/// </summary>
/// <param name="compressedInput">The byte[] to decompress</param>
/// <param name="size">In: the max size of the subset of compressedInput to read, out: size read</param>
/// <param name="decompressedPackets">The int[] to write the packet data to</param>
void Compressor::DecompressPacketData(
#if OLD_ISF
    Compressor& compressor,
#endif
    QByteArray compressedInput,
    uint& size,
    QVector<int>& decompressedPackets)
{
#if OLD_ISF
    //
    // lock to prevent multi-threaded vulnerabilities
    //
    lock (_compressSync)
    {
#endif
        if (/*compressedInput == null ||*/
            size > compressedInput.size()/* ||
            decompressedPackets == null*/)
        {
            //we don't raise any information that could be used to attack our ISF code
            //a simple 'ISF Operation Failed' is sufficient since the user can't do
            //anything to fix bogus ISF
            throw std::runtime_error("StrokeCollectionSerializer.ISFDebugMessage(SR.Get(SRID.DecompressPacketDataFailed))");
        }

        size = GetAlgoModule().DecompressPacketData(compressedInput, decompressedPackets);
}

/// <summary>
/// DecompressPropertyData - decompresses a byte[] representing property data (such as DrawingAttributes.Color)
/// </summary>
/// <param name="input">The byte[] to decompress</param>
QByteArray Compressor::DecompressPropertyData(QByteArray input)
{
    //if (input == null)
    {
        //we don't raise any information that could be used to attack our ISF code
        //a simple 'ISF Operation Failed' is sufficient since the user can't do
        //anything to fix bogus ISF
        //throw std::runtime_error();
    }

    QByteArray data = GetAlgoModule().DecompressPropertyData(input);
    return data;
}

/// <summary>
/// CompressPropertyData
/// </summary>
/// <param name="data"></param>
/// <param name="algorithm"></param>
/// <returns></returns>
QByteArray Compressor::CompressPropertyData(QByteArray data, quint8 algorithm)
{
    //if (data == null)
    {
        //we don't raise any information that could be used to attack our ISF code
        //a simple 'ISF Operation Failed' is sufficient since the user can't do
        //anything to fix bogus ISF
        //throw std::runtime_error("data");
    }

    return GetAlgoModule().CompressPropertyData(data, algorithm);
}

#if OLD_ISF
/// <summary>
/// CompressPropertyData - compresses property data using the compression defined by 'compressor'
/// </summary>
/// <param name="compressor">The compressor to use.  This can be null for default compression</param>
/// <param name="input">The int[] of packet data to compress</param>
/// <param name="algorithm">In: the desired algorithm to use.  Out: the algorithm used</param>
/// <returns>the compressed data in a byte[]</returns>
/// <SecurityNote>
///     Critical - Calls unmanaged code in MS.Win32.Penimc.UnsafeNativeMethods to compress
///         an int[] representing packet data
///
///     This directly called by StrokeCollectionSerializer.SaveStrokeIds
///                             StrokeSerializer.SavePacketPropertyData
///
///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
///
/// </SecurityNote>
[SecurityCritical]
#else
/// <summary>
/// CompressPropertyData - compresses property data using the compression defined by 'compressor'
/// </summary>
/// <param name="input">The int[] of packet data to compress</param>
/// <param name="algorithm">In: the desired algorithm to use.  Out: the algorithm used</param>
/// <returns>the compressed data in a byte[]</returns>
#endif
QByteArray Compressor::CompressPacketData(
#if OLD_ISF
        Compressor& compressor,
#endif
        Array<int> input,
        quint8& algorithm)
{
    //if (input == null)
    {
        //we don't raise any information that could be used to attack our ISF code
        //a simple 'ISF Operation Failed' is sufficient since the user can't do
        //anything to fix bogus ISF
    //    throw std::runtime_error(SR.Get(SRID.IsfOperationFailed));
    }

    QByteArray data = GetAlgoModule().CompressPacketData(input, algorithm);
    return data;
}

/// <summary>
/// Thread static algo module
/// </summary>
//[ThreadStatic]
AlgoModule* Compressor::_algoModule = nullptr;

/// <summary>
/// Private AlgoModule, one per thread, lazy init'd
/// </summary>
AlgoModule& Compressor::GetAlgoModule()
{
    if (_algoModule == nullptr)
    {
        _algoModule = new AlgoModule();
    }
    return *_algoModule;
}

INKCANVAS_END_NAMESPACE
