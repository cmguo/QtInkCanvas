#ifndef COMPRESS_H
#define COMPRESS_H

#include <QByteArray>
#include <QVector>

#define OLD_ISF 0

class AlgoModule;

class Compressor
#if OLD_ISF
    : IDisposable
#endif
{
public:
#if OLD_ISF
    /// <summary>
    /// Non-static members
    /// </summary>
    /// <SecurityNote>
    ///     Critical - This is plutonium, marked critical to
    ///         prevent it from being used from transparent code
    /// </SecurityNote>
    [SecurityCritical]
    private MS.Win32.Penimc.CompressorSafeHandle _compressorHandle;

    /// <summary>
    /// Compressor constructor.  This is called by our ISF decompression
    /// after reading the ISF header that indicates which type of compression
    /// was performed on the ISF when it was being compressed.
    /// </summary>
    /// <param name="data">a byte[] specifying the compressor used to compress the ISF being decompressed</param>
    /// <param name="size">expected initially to be the length of data, it IsfLoadCompressor sets it to the
    ///                    length of the header that is read.  They should always match, but in cases where they
    ///                    don't, we immediately fail</param>
    /// <SecurityNote>
    ///     Critical - Calls unmanaged code through MS.Win32.Penimc.UnsafeNativeMethods to instance a compressor
    ///             that is used to decompress packet or property data.  The compressor instance is wrapped in a
    ///             safehandle.
    ///
    ///             This ctor is called by StrokeCollectionSerializer.EncodeRawISF, which is marked TreatAsSafe
    /// </SecurityNote>
    [SecurityCritical]
    internal Compressor(byte[] data, ref uint size)
    {
        if (data == null || data.Length != size)
        {
            //we don't raise any information that could be used to attack our ISF code
            //a simple 'ISF Operation Failed' is sufficient since the user can't do
            //anything to fix bogus ISF
            throw new InvalidOperationException(StrokeCollectionSerializer.ISFDebugMessage(SR.Get(SRID.InitializingCompressorFailed)));
        }

        _compressorHandle = MS.Win32.Penimc.UnsafeNativeMethods.IsfLoadCompressor(data, ref size);
        if (_compressorHandle.IsInvalid)
        {
            //we don't raise any information that could be used to attack our ISF code
            //a simple 'ISF Operation Failed' is sufficient since the user can't do
            //anything to fix bogus ISF
            throw new InvalidOperationException(StrokeCollectionSerializer.ISFDebugMessage(SR.Get(SRID.InitializingCompressorFailed)));
        }
    }
    /// <summary>
    /// DecompressPacketData - take a byte[] or a subset of a byte[] and decompresses it into
    ///     an int[] of packet data (for example, x's in a Stroke)
    /// </summary>
    /// <param name="compressor">
    ///     The compressor used to decompress this byte[] of packet data (for example, x's in a Stroke)
    ///     Can be null
    /// </param>
    /// <param name="compressedInput">The byte[] to decompress</param>
    /// <param name="size">In: the max size of the subset of compressedInput to read, out: size read</param>
    /// <param name="decompressedPackets">The int[] to write the packet data to</param>
    /// <SecurityNote>
    ///     Critical - Calls unmanaged code in MS.Win32.Penimc.UnsafeNativeMethods to decompress
    ///         a byte[] into an int[] of packet data (for example, x's or y's in a Stroke)
    ///
    ///     This is called by StrokeSerializer.LoadPackets directly.  The TreatAsSafe boundary of this call
    ///     is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// DecompressPacketData - take a byte[] or a subset of a byte[] and decompresses it into
    ///     an int[] of packet data (for example, x's in a Stroke)
    /// </summary>
    /// <param name="compressedInput">The byte[] to decompress</param>
    /// <param name="size">In: the max size of the subset of compressedInput to read, out: size read</param>
    /// <param name="decompressedPackets">The int[] to write the packet data to</param>
#endif
    static void DecompressPacketData(
#if OLD_ISF
        Compressor& compressor,
#endif
        QByteArray compressedInput,
        uint& size,
        QVector<int>& decompressedPackets);

#if OLD_ISF
    /// <summary>
    /// DecompressPropertyData - decompresses a byte[] representing property data (such as DrawingAttributes.Color)
    /// </summary>
    /// <param name="input">The byte[] to decompress</param>
    /// <SecurityNote>
    ///     Critical - Calls unmanaged code in MS.Win32.Penimc.UnsafeNativeMethods to decompress
    ///         a byte[] representing property data
    ///
    ///     This directly called by ExtendedPropertySerializer.DecodeAsISF.
    ///                             DrawingAttributesSerializer.DecodeAsISF.
    ///                             StrokeSerializer.DecodeISFIntoStroke.
    ///
    ///     The TreatAsSafe boundary of this call
    ///     is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// DecompressPropertyData - decompresses a byte[] representing property data (such as DrawingAttributes.Color)
    /// </summary>
    /// <param name="input">The byte[] to decompress</param>
#endif
    static QByteArray DecompressPropertyData(QByteArray input);

#if OLD_ISF
    /// <summary>
    /// CompressPropertyData - compresses property data
    /// </summary>
    /// <param name="data">The property data to compress</param>
    /// <param name="algorithm">In: the desired algorithm to use.  Out: the algorithm used</param>
    /// <param name="outputSize">In: if output is not null, the size of output.  Out: the size required if output is null</param>
    /// <param name="output">The byte[] to writ the compressed data to</param>
    /// <SecurityNote>
    ///     Critical - Calls unmanaged code in MS.Win32.Penimc.UnsafeNativeMethods to compress
    ///         a byte[] representing property data
    ///
    ///     This directly called by ExtendedPropertySerializer.EncodeAsISF
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
    internal static void CompressPropertyData(byte[] data, ref byte algorithm, ref uint outputSize, byte[] output)
    {
        //
        // lock to prevent multi-threaded vulnerabilities
        //
        lock (_compressSync)
        {
            //
            // it is valid to pass is null for output to check to see what the
            // required buffer size is.  We want to guard against when output is not null
            // and outputSize doesn't match, as this is passed directly to unmanaged code
            // and could result in bytes being written past the end of output buffer
            //
            if (output != null && outputSize != output.Length)
            {
                //we don't raise any information that could be used to attack our ISF code
                //a simple 'ISF Operation Failed' is sufficient since the user can't do
                //anything to fix bogus ISF
                throw new InvalidOperationException(SR.Get(SRID.IsfOperationFailed));
            }
            int hr = MS.Win32.Penimc.UnsafeNativeMethods.IsfCompressPropertyData(data, (uint)data.Length, ref algorithm, ref outputSize, output);
            if (0 != hr)
            {
                //we don't raise any information that could be used to attack our ISF code
                //a simple 'ISF Operation Failed' is sufficient since the user can't do
                //anything to fix bogus ISF
                throw new InvalidOperationException(StrokeCollectionSerializer.ISFDebugMessage("IsfCompressPropertyData returned: " + hr.ToString(CultureInfo.InvariantCulture)));
            }
        }
    }
#endif

    /// <summary>
    /// CompressPropertyData
    /// </summary>
    /// <param name="data"></param>
    /// <param name="algorithm"></param>
    /// <returns></returns>
    static QByteArray CompressPropertyData(QByteArray data, quint8 algorithm);

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
    static QByteArray CompressPacketData(
#if OLD_ISF
            Compressor& compressor,
#endif
            QVector<int> input,
            quint8& algorithm);

private:
    /// <summary>
    /// Thread static algo module
    /// </summary>
    //[ThreadStatic]
    static AlgoModule* _algoModule;

    /// <summary>
    /// Private AlgoModule, one per thread, lazy init'd
    /// </summary>
    static AlgoModule& GetAlgoModule();

#if OLD_ISF
    private static object _compressSync = new object();
    private bool _disposed = false;

    /// <summary>
    /// Dispose.
    /// </summary>
    /// <SecurityNote>
    ///     Critical - Handles plutonium: _compressorHandle
    ///
    ///     This is directly called by StrokeCollectionSerializer.DecodeRawISF
    ///     The TreatAsSafe boundary of this call is StrokeCollectionSerializer.DecodeRawISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }

        _compressorHandle.Dispose();
        _disposed = true;
        _compressorHandle = null;
    }
#endif
};

#endif // COMPRESS_H
