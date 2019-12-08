#ifndef STROKECOLLECTIONSERIALIZER_H
#define STROKECOLLECTIONSERIALIZER_H

#include "Internal/Ink/serializationflags.h"
#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

#include <QList>
#include <QSharedPointer>

class StylusPointDescription;
class Stroke;
class StrokeDescriptor;
class StrokeCollection;
class TransformDescriptor;
class GuidList;
class MetricBlock;
class StylusPointPropertyInfo;
class DrawingAttributes;

// namespace MS.Internal.Ink.InkSerializedFormat

class StrokeCollectionSerializer
{
public:
    //#region Constants (Static Fields)
    static constexpr double AvalonToHimetricMultiplier = 2540.0 / 96.0;
    static constexpr double HimetricToAvalonMultiplier = 96.0 / 2540.0;
    static TransformDescriptor IdentityTransformDescriptor;

    static TransformDescriptor InitTransformDescriptor();
    //#endregion

    //#region Constructors

    // disable default constructor
private:
    //StrokeCollectionSerializer() { }

public:
    /// <summary>
    /// Initialize the Ink serializer
    /// </summary>
    /// <param name="coreStrokes">Pointer to the core stroke collection - avoids recreation of collections</param>
    StrokeCollectionSerializer(StrokeCollection& coreStrokes);

    //#endregion

    //#region Public Fields

    PersistenceFormat CurrentPersistenceFormat = PersistenceFormat::InkSerializedFormat;
    CompressionMode CurrentCompressionMode = CompressionMode::Compressed;
    QList<int> StrokeIds;
    //#endregion

    //#region Decoding

    //#region Public Methods


    /// <summary>
    /// Loads a Ink object from a spcified byte array in the form of Ink Serialzied Format
    /// This method checks for the 'base64:' prefix in the quint8[] because that is how V1
    /// saved ISF
    /// </summary>
    /// <param name="inkData"></param>
    void DecodeISF(QIODevice& inkData);

    //#endregion

    //#region Methods

    /// <summary>
    /// Loads the strokeIds from the stream, we need to do this to decrement the count of bytes
    /// </summary>
    uint LoadStrokeIds(QIODevice& isfStream, uint cbSize);


private:
    bool IsGIFData(QIODevice& inkdata);

    void ExamineStreamHeader(QIODevice& inkdata, bool& fBase64, bool& fGif, quint32& cbData);

    static constexpr quint8 Base64HeaderBytes[] {'b','a','s','e','6','4',':'};

    #if OLD_ISF
        /// <summary>
        /// Takes an ISF quint8[] and populates the StrokeCollection
        ///  attached to this
        /// </summary>
        /// <param name="inkdata">a quint8[] of the raw isf to decode</param>
        /// <SecurityNote>
        ///     Critical - Calls critical methods in Compressor
        ///     TreatAsSafe - inkdata is a disconnected copy from the one passed in
        ///         underlying unmanaged code has been security reviewed and fuzzed
        /// </SecurityNote>
        //[SecurityCritical, SecurityTreatAsSafe]
    #else
        /// <summary>
        /// Takes an ISF Stream and populates the StrokeCollection
        ///  attached to this
        /// </summary>
        /// <param name="inputStream">a Stream the raw isf to decode</param>
    #endif
    void DecodeRawISF(QIODevice& inputStream);

    #if OLD_ISF
    /// <summary>
    /// Loads a DrawingAttributes Table from the stream and adds individual drawing attributes to the drawattr
    /// list passed
    /// </summary>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - Calls the DrawingAttributeSerializer.DecodeAsISF critical method
    /// </SecurityNote>
    //[SecurityCritical]
    #else
    /// <summary>
    /// Loads a DrawingAttributes Table from the stream and adds individual drawing attributes to the drawattr
    /// list passed
    /// </summary>
    #endif
    uint LoadDrawAttrsTable(QIODevice& strm, GuidList& guidList, uint cbSize);

    /// <summary>
    /// Reads and Decodes a stroke descriptor information from the stream. For details on how it is stored
    /// please refer the spec
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <param name="descr"></param>
    /// <returns></returns>
    uint DecodeStrokeDescriptor(QIODevice& strm, uint cbSize, StrokeDescriptor*& descr);


    /// <summary>
    /// Reads and Decodes a stroke descriptor information from the stream. For details on how it is stored
    /// please refer the spec
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <returns></returns>
    uint DecodeStrokeDescriptorBlock(QIODevice& strm, uint cbSize);


    /// <summary>
    /// Reads and Decodes a number of stroke descriptor information from the stream. For details on how they are stored
    /// please refer the spec
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <returns></returns>
    uint DecodeStrokeDescriptorTable(QIODevice& strm, uint cbSize);


    /// <summary>
    /// Decodes metric table from the stream. For information on how they are stored in the stream, please refer to the spec.
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <returns></returns>
    uint DecodeMetricTable(QIODevice& strm, uint cbSize);


    /// <summary>
    /// Decodes a Metric Block from the stream. For information on how they are stored in the stream, please refer to the spec.
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <param name="block"></param>
    /// <returns></returns>
    uint DecodeMetricBlock(QIODevice& strm, uint cbSize, MetricBlock*& block);

    /// <summary>
    /// Reads and Decodes a Table of Transform Descriptors from the stream. For information on how they are stored
    /// in the stream, please refer to the spec.
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <param name="useDoubles"></param>
    /// <returns></returns>
    uint DecodeTransformTable(QIODevice& strm, uint cbSize, bool useDoubles);

public:
    /// <summary>
    /// ReliableRead
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="buffer"></param>
    /// <param name="requestedCount"></param>
    /// <returns></returns>
    static uint ReliableRead(QIODevice& stream, quint8* buffer, uint requestedCount);

private:
    /// <summary>
    /// Reads and Decodes a Transfrom Descriptor Block from the stream. For information on how it is stored in the stream,
    /// please refer to the spec.
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="tag"></param>
    /// <param name="cbSize"></param>
    /// <param name="useDoubles"></param>
    /// <param name="xform"></param>
    /// <returns></returns>
    uint DecodeTransformBlock(QIODevice& strm, KnownTagCache::KnownTagIndex tag, uint cbSize, bool useDoubles, TransformDescriptor*& xform);

    /// <summary>
    /// Decodes Ink Space Rectangle information from the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="cbSize"></param>
    /// <returns></returns>
    uint DecodeInkSpaceRectangle(QIODevice& strm, uint cbSize);


    /// <summary>
    /// Creates a Matrix Information structure based on the transform descriptor
    /// </summary>
    /// <param name="tdrd"></param>
    /// <returns></returns>
    QMatrix LoadTransform(TransformDescriptor* tdrd);


    /// <summary>
    /// Sets the Property Metrics for a property based on Tag and metric descriptor block
    /// </summary>
    /// <param name="guid"></param>
    /// <param name="tag"></param>
    /// <param name="block"></param>
    /// <returns></returns>
    StylusPointPropertyInfo GetStylusPointPropertyInfo(QUuid const & guid, KnownTagCache::KnownTagIndex tag, MetricBlock* block);


    /// <summary>
    /// Builds StylusPointDescription based on StrokeDescriptor and Metric Descriptor Block. Sometime Metric Descriptor block may contain
    /// metric information for properties which are not part of the stroke descriptor. They are simply ignored.
    /// </summary>
    /// <param name="strd"></param>
    /// <param name="block"></param>
    /// <param name="guidList"></param>
    /// <returns></returns>
    QSharedPointer<StylusPointDescription> BuildStylusPointDescription(StrokeDescriptor* strd, MetricBlock* block, GuidList& guidList);
    //#endregion

    //#endregion // Decoding

    //#region Encoding

    //#region Public Methods
#if OLD_ISF
    /// <summary>
    /// This functions Saves the Ink as Ink Serialized Format based on the Compression code
    /// </summary>
    /// <returns>A quint8[] with the encoded ISF</returns>
    /// <SecurityNote>
    ///     Critical - Calls critical methods:
    ///             SaveStrokeIds
    ///             ExtendedPropertySerializer.EncodeAsISF
    ///             StoreStrokeData
    ///
    ///
    ///     TreatAsSafe - We're saving a StrokeCollection and we control and verify
    ///             all of the data the StrokeCollection directly and indirectly contains
    ///
    ///             This codepath calls into unmanaged code in Compressor.CompressPacketData
    ///             and Compressor.CompressPropertyData.  The underlying unmanaged code has been
    ///              security reviewed and fuzzed
    ///
    /// </SecurityNote>
    //[SecurityCritical, SecurityTreatAsSafe]
#else
    /// <summary>
    /// This functions Saves the Ink as Ink Serialized Format based on the Compression code
    /// </summary>
    /// <returns>A quint8[] with the encoded ISF</returns>
#endif
    void EncodeISF(QIODevice& outputStream);

#if OLD_ISF
    /// <Summary>
    /// Encodes all of the strokes in a strokecollection to ISF
    /// </Summary>
    /// <SecurityNote>
    ///     Critical - Calls the critical method StrokeSerializer.EncodeStroke
    ///
    ///     This directly called by EncodeISF
    ///
    ///     TreatAsSafe boundary is EncodeISF
    ///
    /// </SecurityNote>
    //[SecurityCritical]
#else
    /// <Summary>
    /// Encodes all of the strokes in a strokecollection to ISF
    /// </Summary>
#endif
    void StoreStrokeData(QIODevice& localStream, GuidList& guidList, quint32& cumulativeEncodedSize, quint32& localEncodedSize);
#if OLD_ISF
    /// <summary>
    /// Saves the stroke Ids in the stream.
    /// </summary>
    /// <param name="strokes"></param>
    /// <param name="strm"></param>
    /// <param name="forceSave">save ids even if they are contiguous</param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - Calls the critical method Compressor.CompressPacketData
    ///
    ///     This directly called by EncodeISF
    ///
    ///     TreatAsSafe boundary is EncodeISF
    ///
    /// </SecurityNote>
    //[SecurityCritical]
#else
    /// <summary>
    /// Saves the stroke Ids in the stream.
    /// </summary>
    /// <param name="strokes"></param>
    /// <param name="strm"></param>
    /// <param name="forceSave">save ids even if they are contiguous</param>
#endif
    static uint SaveStrokeIds(StrokeCollection& strokes, QIODevice& strm, bool forceSave);


    //#endregion

    //#region Methods

    /// <summary>
    /// Simple helper method to examine the first 7 members (if they exist)
    /// of the quint8[] and see if they have the ascii characters 'base64:' in them.
    /// </summary>
    /// <param name="data"></param>
    /// <returns></returns>
    bool IsBase64Data(QIODevice& data);

    /// <summary>
    /// Builds the GuidList& based on ExtendedPropeties and StrokeCollection
    /// </summary>
    /// <returns></returns>
    GuidList BuildGuidList();

    /// <summary>
    /// Builds the list of Custom Guids that were used by this particular stroke, either in the packet layout
    /// or in the drawing attributes, or in the buttons or in Extended properties or in the point properties
    /// and updates the GuidList& with that information
    /// </summary>
    /// <param name="stroke"></param>
    /// <param name="guidList"></param>
    void BuildStrokeGuidList(QSharedPointer<Stroke> stroke, GuidList& guidList);


    quint8 GetCompressionAlgorithm();


    /// <summary>
    /// This function serializes Stroke Descriptor Table in the stream. For information on how they are serialized, please refer to the spec.
    ///
    /// </summary>
    /// <param name="strm"></param>
    /// <returns></returns>
    uint SerializePacketDescrTable(QIODevice& strm);


    /// <summary>
    /// This function serializes Metric Descriptor Table in the stream. For information on how they are serialized, please refer to the spec.
    /// </summary>
    /// <param name="strm"></param>
    /// <returns></returns>
    uint SerializeMetricTable(QIODevice& strm);


    /// <summary>
    /// Multibyte Encodes a Stroke Descroptor
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="strd"></param>
    /// <returns></returns>
    uint EncodeStrokeDescriptor(QIODevice& strm, StrokeDescriptor& strd);


    /// <summary>
    /// This function serializes Transform Descriptor Table in the stream. For information on how they are serialized, please refer to the spec.
    /// </summary>
    /// <param name="strm"></param>
    /// <returns></returns>
    uint SerializeTransformTable(QIODevice& strm);


    /// <summary>
    /// Multibyte Encode if necessary a Transform Descriptor into the stream
    /// </summary>
    /// <param name="strm"></param>
    /// <param name="xform"></param>
    /// <param name="useDoubles"></param>
    /// <returns></returns>
    uint EncodeTransformDescriptor(QIODevice& strm, TransformDescriptor& xform, bool useDoubles);

#if OLD_ISF
    /// <summary>
    /// This function serializes Drawing Attributes Table in the stream. For information on how they are serialized, please refer to the spec.
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="guidList"></param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - Calls the critical method
    ///         DrawingAttributeSerializer.EncodeAsISF
    ///
    ///     This directly called by EncodeISF
    ///
    ///     TreatAsSafe boundary is EncodeISF
    ///
    /// </SecurityNote>
    //[SecurityCritical]
#else
    /// <summary>
    /// This function serializes Drawing Attributes Table in the stream. For information on how they are serialized, please refer to the spec.
    /// </summary>
    /// <param name="stream"></param>
    /// <param name="guidList"></param>
#endif
    uint SerializeDrawingAttrsTable(QIODevice& stream, GuidList& guidList);

    /// <summary>
    /// This function builds list of all unique Tables, ie Stroke Descriptor Table, Metric Descriptor Table, Transform Descriptor Table
    /// and Drawing Attributes Table based on all the strokes. Each entry in the Table is unique with respect to the table.
    /// </summary>
    /// <param name="guidList"></param>
    void BuildTables(GuidList& guidList);

    //#endregion // Methods

public:
    class StrokeLookupEntry
    {
    public:
        uint MetricDescriptorTableIndex = 0;
        uint StrokeDescriptorTableIndex = 0;
        uint TransformTableIndex = 0;
        uint DrawingAttributesTableIndex = 0;

        // Compression algorithm data
        quint8 CompressionData = 0;

        QVector<QVector<int>> ISFReadyStrokeData;
        bool StorePressure = false;
    };

    //#endregion // Encoding

    //#region Debugging Methods
private:
    //[System.Diagnostics.Conditional("DEBUG_ISF")]
    static void ISFDebugTrace(QString message)
    {
        //Debug::WriteLine(message);
    }
    //#endregion

    // [System.Diagnostics.Conditional("DEBUG_ISF")]
    static QString const & ISFDebugMessage(QString const & debugMessage)
    {
//#if DEBUG
        return debugMessage;
//#else
//        return SR.Get(SRID.IsfOperationFailed);
//#endif
    }

    //#region Fields

    StrokeCollection& _coreStrokes;
private:
    QList<StrokeDescriptor*> _strokeDescriptorTable;
    QList<TransformDescriptor*> _transformTable;
    QList<QSharedPointer<DrawingAttributes>> _drawingAttributesTable;
    QList<MetricBlock*> _metricTable;
    QPointF _himetricSize;


        // The ink space rectangle (e.g. bounding box for GIF) is stored
        //      with the serialization info so that load/save roundtrip the
        //      rectangle
    QRectF _inkSpaceRectangle;

    QMap<QSharedPointer<Stroke>, StrokeLookupEntry*> _strokeLookupTable;

    //#endregion
};

/// <summary>
/// Simple static method for generating StrokeIds
/// </summary>
class StrokeIdGenerator
{
public:
    /// <summary>
    /// Generates backwards compatible StrokeID's for the strokes
    /// </summary>
    /// <param name="strokes">strokes</param>
    /// <returns></returns>
    static QVector<int> GetStrokeIds(StrokeCollection& strokes);
};

#endif // STROKECOLLECTIONSERIALIZER_H
