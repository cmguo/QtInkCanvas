#ifndef STROKESERIALIZER_H
#define STROKESERIALIZER_H

#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"

#include <QSharedPointer>
#include <QVector>

#define OLD_ISF 0

class QIODevice;
class GuidList;
class StrokeDescriptor;
class StylusPointDescription;
class DrawingAttributes;
class QMatrix;
class Compressor;
class Stroke;
class StylusPointCollection;
class ExtendedPropertyCollection;

class StrokeSerializer
{
    //#region Decoding

    //#region Public Methods
public:
#if OLD_ISF
    /// <summary>
    /// Loads a stroke from the stream based on Stroke Descriptor, StylusPointDescription, Drawing Attributes, Stroke IDs, transform and GuidList
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="size"></param>
    /// <param name="guidList"></param>
    /// <param name="strokeDescriptor"></param>
    /// <param name="stylusPointDescription"></param>
    /// <param name="drawingAttributes"></param>
    /// <param name="transform"></param>
    /// <param name="compressor">Compression module</param>
    /// <param name="stroke">Newly decoded stroke</param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - calls the StrokeSerializer.DecodeISFIntoStroke critical method
    ///
    ///     Called directly by  StrokeCollectionSerializer.DecodeRawISF
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Loads a stroke from the stream based on Stroke Descriptor, StylusPointDescription, Drawing Attributes, Stroke IDs, transform and GuidList
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="size"></param>
    /// <param name="guidList"></param>
    /// <param name="strokeDescriptor"></param>
    /// <param name="stylusPointDescription"></param>
    /// <param name="drawingAttributes"></param>
    /// <param name="transform"></param>
    /// <param name="stroke">Newly decoded stroke</param>
#endif
    static uint DecodeStroke(QIODevice& stream,
                             uint size,
                             GuidList& guidList,
                             StrokeDescriptor& strokeDescriptor,
                             QSharedPointer<StylusPointDescription> stylusPointDescription,
                             QSharedPointer<DrawingAttributes> drawingAttributes,
                             QMatrix& transform,
#if OLD_ISF
                             Compressor& compressor,
#endif
                             QSharedPointer<Stroke>& stroke);

#if OLD_ISF
    /// <summary>
    /// This functions loads a stroke from a memory stream based on the descriptor and GuidList. It returns
    /// the no of bytes it has read from the stream to correctly load the stream, which should be same as
    /// the value of the size parameter. If they are unequal throws ArgumentException. Stroke descriptor is
    /// used to load the packetproperty as well as ExtendedPropertyCollection on this stroke. Compressor is used
    /// to decompress the data.
    /// </summary>
    /// <param name="compressor"></param>
    /// <param name="stream"></param>
    /// <param name="totalBytesInStrokeBlockOfIsfStream"></param>
    /// <param name="guidList"></param>
    /// <param name="strokeDescriptor"></param>
    /// <param name="stylusPointDescription"></param>
    /// <param name="transform"></param>
    /// <param name="stylusPoints"></param>
    /// <param name="extendedProperties"></param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - calls the ExtendedPropertySerializer.DecodeAsISF
    ///                          StrokeSerializer.LoadPackets
    ///                      and Compressor.DecompressPropertyData critical methods
    ///
    ///     Called directly by  StrokeSerializer.DecodeStroke
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
   [SecurityCritical]
#else
    /// <summary>
    /// This functions loads a stroke from a memory stream based on the descriptor and GuidList. It returns
    /// the no of bytes it has read from the stream to correctly load the stream, which should be same as
    /// the value of the size parameter. If they are unequal throws ArgumentException. Stroke descriptor is
    /// used to load the packetproperty as well as ExtendedPropertyCollection on this stroke. Compressor is used
    /// to decompress the data.
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="totalBytesInStrokeBlockOfIsfStream"></param>
    /// <param name="guidList"></param>
    /// <param name="strokeDescriptor"></param>
    /// <param name="stylusPointDescription"></param>
    /// <param name="transform"></param>
    /// <param name="stylusPoints"></param>
    /// <param name="extendedProperties"></param>
#endif
    static uint DecodeISFIntoStroke(
#if OLD_ISF
        Compressor& compressor,
#endif
        QIODevice& stream,
        uint totalBytesInStrokeBlockOfIsfStream,
        GuidList& guidList,
        StrokeDescriptor& strokeDescriptor,
        QSharedPointer<StylusPointDescription> stylusPointDescription,
        QMatrix& transform,
        QSharedPointer<StylusPointCollection>& stylusPoints,
        ExtendedPropertyCollection*& extendedProperties);

#if OLD_ISF
    /// <summary>
    /// Loads packets from the input stream.  For example, packets are all of the x's in a stroke
    /// </summary>
    /// <SecurityNote>
    ///     Critical - calls the Compressor.DecompressPacketData critical method
    ///
    ///     Called directly by  StrokeSerializer.DecodeISFIntoStroke
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Loads packets from the input stream.  For example, packets are all of the x's in a stroke
    /// </summary>
#endif
    static uint LoadPackets(QIODevice& inputStream,
                            uint totalBytesInStrokeBlockOfIsfStream,
#if OLD_ISF
                            Compressor& compressor,
#endif
                            QSharedPointer<StylusPointDescription> stylusPointDescription,
                            QMatrix& transform,
                            QSharedPointer<StylusPointCollection>& stylusPoints);

private:
    /// <summary>
    /// Fill packet buffer with button data
    /// </summary>
    /// <param name="pointCount">how many points in the stroke</param>
    /// <param name="buttonCount">how many buttons</param>
    /// <param name="buttonIndex">the first index of the buttons in packets</param>
    /// <param name="packets">packet data</param>
    /// <param name="buttonData">button data to fill</param>
    static void FillButtonData(
        int pointCount,
        int buttonCount,
        int buttonIndex,
        QVector<int> packets,
        QByteArray buttonData);



    //#endregion

    //#endregion // Decoding

    //#region Encoding

    //#region Public Methods
public:
#if OLD_ISF
    /// <summary>
    /// Returns an array of bytes of the saved stroke
    /// </summary>
    /// <param name="stroke">Stroke to save</param>
    /// <param name="stream">null to calculate only the size</param>
    /// <param name="compressor"></param>
    /// <param name="compressionAlgorithm"></param>
    /// <param name="guidList"></param>
    /// <param name="strokeLookupEntry"></param>
    /// <SecurityNote>
    ///     Critical - Calls the critical methods
    ///                     StrokeSerializer.SavePackets
    ///                     ExtendedPropertySerializer.EncodeAsISF
    ///
    ///     This directly called by StrokeCollectionSerializer.StoreStrokeData
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Returns an array of bytes of the saved stroke
    /// </summary>
    /// <param name="stroke">Stroke to save</param>
    /// <param name="stream">null to calculate only the size</param>
    /// <param name="compressionAlgorithm"></param>
    /// <param name="guidList"></param>
    /// <param name="strokeLookupEntry"></param>
#endif
    static uint EncodeStroke(
        Stroke& stroke,
        QIODevice& stream,
#if OLD_ISF
        Compressor compressor,
#endif
        quint8 compressionAlgorithm,
        GuidList& guidList,
        StrokeCollectionSerializer::StrokeLookupEntry& strokeLookupEntry);

    /// <summary>
    /// Builds the Stroke Descriptor for this stroke based on Packet Layout and Extended Properties
    /// For details on how this is strored please refer to the spec.
    /// </summary>
    static void BuildStrokeDescriptor(
        Stroke const& stroke,
        GuidList& guidList,
        StrokeCollectionSerializer::StrokeLookupEntry& strokeLookupEntry,
        StrokeDescriptor*& strokeDescriptor,
        MetricBlock*& metricBlock);

    //#endregion // Private Methods

    //#region Private methods
#if OLD_ISF
    /// <summary>
    /// Saves the packets into a stream of bytes
    /// </summary>
    /// <param name="stroke">Stroke to save</param>
    /// <param name="stream">null to calculate size only</param>
    /// <param name="compressor"></param>
    /// <param name="strokeLookupEntry"></param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - Calls the critical method StrokeSerializer.SavePacketPropertyData
    ///
    ///     This directly called by StrokeSerializer.EncodeStroke
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Saves the packets into a stream of bytes
    /// </summary>
    /// <param name="stroke">Stroke to save</param>
    /// <param name="stream">null to calculate size only</param>
    /// <param name="strokeLookupEntry"></param>
#endif
    static uint SavePackets(
        Stroke& stroke,
        QIODevice& stream,
#if OLD_ISF
        Compressor& compressor,
#endif
        StrokeCollectionSerializer::StrokeLookupEntry& strokeLookupEntry);

#if OLD_ISF
    /// <summary>
    /// Saves the packets data corresponding to a packet property (identified by the guid) into the stream
    /// based on the Compression algorithm and compress header
    /// </summary>
    /// <param name="packetdata">packet data to save</param>
    /// <param name="stream">null to calculate only the size</param>
    /// <param name="compressor"></param>
    /// <param name="guid"></param>
    /// <param name="algo"></param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - Calls unmanaged code in Compressor.CompressPacketData to compress
    ///         an int[] representing packet data
    ///
    ///     This directly called by StrokeSerializer.SavePackets
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Saves the packets data corresponding to a packet property (identified by the guid) into the stream
    /// based on the Compression algorithm and compress header
    /// </summary>
    /// <param name="packetdata">packet data to save</param>
    /// <param name="stream">null to calculate only the size</param>
    /// <param name="guid"></param>
    /// <param name="algo"></param>
#endif
    static uint SavePacketPropertyData(
        QVector<int> packetdata,
        QIODevice& stream,
#if OLD_ISF
        Compressor& compressor,
#endif
        QUuid const & guid,
        quint8& algo);

    //#endregion

    //#endregion // Encoding
};

#endif // STROKESERIALIZER_H
