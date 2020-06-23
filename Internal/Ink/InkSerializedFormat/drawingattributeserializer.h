#ifndef DRAWINGATTRIBUTESERIALIZER_H
#define DRAWINGATTRIBUTESERIALIZER_H

#define OLD_ISF 0

#include "InkCanvas_global.h"

class QIODevice;
class QDataStream;

INKCANVAS_BEGIN_NAMESPACE

class DrawingAttributes;
class GuidList;

// namespace MS.Internal.Ink.InkSerializedFormat

class DrawingAttributeSerializer
{
private:
    static constexpr double      V1PenWidthWhenWidthIsMissing = 25.0;
    static constexpr double      V1PenHeightWhenHeightIsMissing = 25.0;
    static constexpr int         TransparencyDefaultV1 = 0;
public:
    static constexpr quint32        RasterOperationMaskPen = 9;
    static constexpr quint32        RasterOperationDefaultV1 = 13;

    /// <summary>The v1 ISF version of the pen tip shape. For v2, this is represented as StylusShape</summary>
    enum PenTip
    {
        Circle = 0,
        Rectangle = 1,
        Default = Circle
    };

    class PenTipHelper
    {
        static bool IsDefined(PenTip penTip)
        {
            if (penTip < PenTip::Circle || penTip > PenTip::Rectangle)
            {
                return false;
            }
            return true;
        }
    };

    /// <summary>The v1 ISF version of the pen style. For v2, this is represented as StylusShape</summary>
    enum PenStyle
    {
        Cosmetic = 0x00000000,     // no shape
        Geometric = 0x00010000,     // has shape
        PenStyleDefault = Geometric
    };

#if OLD_ISF
    /// <summary>
    /// Loads drawing attributes from a memory buffer.
    /// </summary>
    /// <param name="stream">Memory buffer to read from</param>
    /// <param name="guidList">Guid tags if extended properties are used</param>
    /// <param name="maximumStreamSize">Maximum size of buffer to read through</param>
    /// <param name="da">The drawing attributes collection to decode into</param>
    /// <returns>Number of bytes read</returns>
    /// <SecurityNote>
    ///     Critical - calls the ExtendedPropertySerializer.DecodeAsISF
    ///                      and Compressor.DecompressPropertyData critical methods
    ///
    ///     Called directly by  StrokeCollectionSerializer.DecodeRawISF
    ///                         StrokeCollectionSerializer.LoadDrawAttrsTable
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Loads drawing attributes from a memory buffer.
    /// </summary>
    /// <param name="stream">Memory buffer to read from</param>
    /// <param name="guidList">Guid tags if extended properties are used</param>
    /// <param name="maximumStreamSize">Maximum size of buffer to read through</param>
    /// <param name="da">The drawing attributes collection to decode into</param>
    /// <returns>Number of bytes read</returns>
#endif

    static quint32 DecodeAsISF(QIODevice& stream, GuidList& guidList, quint32 maximumStreamSize, DrawingAttributes & da);

    /// <summary>
    /// helper to limit what we set for width or height on deserialization
    /// </summary>
    static double GetCappedHeightOrWidth(double heightOrWidth);
    //#endregion // Decoding

    //#region Encoding

#if OLD_ISF
    /// <Summary>
    /// Encodes a DrawingAttriubtesin the ISF stream.
    /// </Summary>
    /// <SecurityNote>
    ///     Critical - Calls the critical methods
    ///         DrawingAttributeSerializer.PersistExtendedProperties
    ///         DrawingAttributeSerializer.PersistStylusTip
    ///
    ///     This directly called by StrokeCollectionSerializer.SerializeDrawingAttrsTable
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <Summary>
    /// Encodes a DrawingAttriubtesin the ISF stream.
    /// </Summary>
#endif
    static quint32 EncodeAsISF(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, unsigned char compressionAlgorithm, bool fTag);


    static void PersistDrawingFlags(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw);

    static void PersistColorAndTransparency(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw);

    static void PersistRasterOperation(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw);
#if OLD_ISF
    /// <Summary>
    /// Encodes the ExtendedProperties in the ISF stream.
    /// </Summary>
    /// <SecurityNote>
    ///     Critical - Calls the critical method ExtendedPropertySerializer.EncodeAsISF
    ///
    ///     This directly called by DrawingAttributeSerializer.EncodeAsISF
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <Summary>
    /// Encodes the ExtendedProperties in the ISF stream.
    /// </Summary>
#endif
    static void PersistExtendedProperties(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw, unsigned char compressionAlgorithm, bool fTag);
#if OLD_ISF
    /// <Summary>
    /// Encodes the StylusTip in the ISF stream.
    /// </Summary>
    /// <SecurityNote>
    ///     Critical - Calls the critical method ExtendedPropertySerializer.EncodeAsISF
    ///
    ///     This directly called by DrawingAttributeSerializer.EncodeAsISF
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <Summary>
    /// Encodes the StylusTip in the ISF stream.
    /// </Summary>
#endif
    static void PersistStylusTip(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw);

    static void PersistWidthHeight(DrawingAttributes& da, QIODevice&  stream, GuidList& guidList, quint32& cbData, QDataStream& bw);


    //#endregion // Encoding

    class PersistenceTypes
    {
    public:
        //static constexpr Type StylusTip = typeof(Int32);
        //static constexpr Type IsHollow = typeof(bool);
        //static constexpr Type StylusTipTransform = typeof(string);
    };
};

INKCANVAS_END_NAMESPACE

#endif // DRAWINGATTRIBUTESERIALIZER_H
