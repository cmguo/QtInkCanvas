#ifndef EXTENDEDPROPERTYSERIALIZER_H
#define EXTENDEDPROPERTYSERIALIZER_H

#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

#include <QUuid>
#include <QVariant>

class ExtendedProperty;
class ExtendedPropertyCollection;
class QIODevice;
class GuidList;

/// <summary>
/// ExtendedProperty converter for ISF and serialization purposes
/// </summary>
class ExtendedPropertySerializer
{

private:
        // If the ExtendedProperty identifier matches one of the original ISF/Tablet-internal
        //      Guids that did not include embedded type information (e.g. used the OS-internal
        //      property storage API), then it is always stored as byte array and does not
        //      include type information
    static bool UsesEmbeddedTypeInformation(QUuid const &propGuid);

public:
    static void EncodeToStream(ExtendedProperty& attribute, QIODevice& stream);

    /// <summary>
    /// This function returns the Data bytes that accurately describes the object
    /// </summary>
    /// <returns></returns>
    static void EncodeAttribute(QUuid const &guid, QVariant const & value, QIODevice& stream);

#if OLD_ISF
    /// <summary>
    /// Encodes a custom attribute to the ISF stream
    /// </summary>
    /// <SecurityNote>
    ///     Critical - Calls the critical method Compressor.CompressPropertyData
    ///             to compress the attribute data
    ///
    ///     This directly called by ExtendedPropertySerializer.EncodeAsISF
    ///                             DrawingAttributeSerializer.PersistStylusTip
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Encodes a custom attribute to the ISF stream
    /// </summary>
#endif
    static uint EncodeAsISF(QUuid const &id, QByteArray data, QIODevice& strm, GuidList& guidList, quint8 compressionAlgorithm, bool fTag);

#if OLD_ISF
    /// <summary>
    /// Loads a single ExtendedProperty from the stream and add that to the list. Tag may be passed as in
    /// the case of Stroke ExtendedPropertyCollection where tag is stored in the stroke descriptor or 0 when tag
    /// is embeded in the stream
    /// </summary>
    /// <param name="stream">Memory buffer to load from</param>
    /// <param name="cbSize">Maximum length of buffer to read</param>
    /// <param name="guidList">QUuid const &cache to read from</param>
    /// <param name="tag">QUuid const &tag to lookup</param>
    /// <param name="guid">QUuid const &of property</param>
    /// <param name="data">Data of property</param>
    /// <returns>Length of buffer read</returns>
    /// <SecurityNote>
    ///     Critical - calls the Compressor.DecompressPropertyData critical method
    ///
    ///     Called directly by  DrawingAttributeSerializer.DecodeAsISF
    ///                         StrokeCollectionSerializer.DecodeRawISF
    ///                         StrokeSerializer.DecodeISFIntoStroke
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.DecodeRawISF
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Loads a single ExtendedProperty from the stream and add that to the list. Tag may be passed as in
    /// the case of Stroke ExtendedPropertyCollection where tag is stored in the stroke descriptor or 0 when tag
    /// is embeded in the stream
    /// </summary>
    /// <param name="stream">Memory buffer to load from</param>
    /// <param name="cbSize">Maximum length of buffer to read</param>
    /// <param name="guidList">QUuid const &cache to read from</param>
    /// <param name="tag">QUuid const &tag to lookup</param>
    /// <param name="guid">QUuid const &of property</param>
    /// <param name="data">Data of property</param>
    /// <returns>Length of buffer read</returns>
#endif
    static uint DecodeAsISF(QIODevice& stream, uint cbSize, GuidList& guidList, KnownTagCache::KnownTagIndex tag, QUuid &guid, QVariant& data);

    /// <summary>
    /// Decodes a byte array (stored in the memory stream) into an object
    /// If the GUID is one of the internal versions, then the type is assumed
    /// to be byte array.
    /// If however, the guid is of unknown origin or not v1 internal, then the type
    ///     information is assumed to be stored in the first 2 bytes of the stream.
    /// </summary>
    /// <param name="guid">QUuid const &of property - to detect origin</param>
    /// <param name="stream">Buffer of data</param>
    /// <returns>object stored in data buffer</returns>
    //static QVariant DecodeAttribute(QUuid const &guid, QIODevice& stream)
    //{
        //VarEnum type;
    //    return DecodeAttribute(guid, stream);
    //}

    /// <summary>
    /// Decodes a byte array (stored in the memory stream) into an object
    /// If the GUID is one of the internal versions, then the type is assumed
    /// to be byte array.
    /// If however, the guid is of unknown origin or not v1 internal, then the type
    ///     information is assumed to be stored in the first 2 bytes of the stream.
    /// </summary>
    /// <param name="guid">QUuid const &of property - to detect origin</param>
    /// <param name="memStream">Buffer of data</param>
    /// <param name="type">the type info stored in the stream</param>
    /// <returns>object stored in data buffer</returns>
    /// <remarks>The buffer stream passed in to the method will be closed after reading</remarks>
    static QVariant DecodeAttribute(QUuid const &guid, QIODevice& memStream);

#if OLD_ISF
    /// <summary>
    /// Saves all elements in this list in the stream passed with the tags being generated based on the GuidList
    /// by the caller and using compressionAlgorithm as the preferred algorith identifier. For ExtendedPropertyCollection associated
    /// with Ink, drawing attributes and Point properties, we need to write the tag while saving them and hence
    /// fTag param is true. For strokes, the Tag is stored in the stroke descriptor and hence we don't store the
    /// tag
    /// </summary>
    /// <param name="attributes">Custom attributes to encode</param>
    /// <param name="stream">If stream is null, then size is calculated only.</param>
    /// <param name="guidList"></param>
    /// <param name="compressionAlgorithm"></param>
    /// <param name="fTag"></param>
    /// <returns></returns>
    /// <SecurityNote>
    ///     Critical - Calls the crtical method ExtendedPropertySerializer.EncodeAsISF (different overload)
    ///
    ///     This directly called by ExtendedPropertySerializer.EncodeAsISF (different overload)
    ///                         and StrokeCollectionSerializer.EncodeISF
    ///                         and DrawingAttributeSerializer.PersistExtendedProperties
    ///                         and StrokeSerializer.EncodeStroke
    ///
    ///     TreatAsSafe boundary is StrokeCollectionSerializer.EncodeISF
    ///
    /// </SecurityNote>
    [SecurityCritical]
#else
    /// <summary>
    /// Saves all elements in this list in the stream passed with the tags being generated based on the GuidList
    /// by the caller and using compressionAlgorithm as the preferred algorith identifier. For ExtendedPropertyCollection associated
    /// with Ink, drawing attributes and Point properties, we need to write the tag while saving them and hence
    /// fTag param is true. For strokes, the Tag is stored in the stroke descriptor and hence we don't store the
    /// tag
    /// </summary>
    /// <param name="attributes">Custom attributes to encode</param>
    /// <param name="stream">If stream is null, then size is calculated only.</param>
    /// <param name="guidList"></param>
    /// <param name="compressionAlgorithm"></param>
    /// <param name="fTag"></param>
#endif
    static uint EncodeAsISF(ExtendedPropertyCollection const & attributes, QIODevice& stream, GuidList& guidList, quint8 compressionAlgorithm, bool fTag);

    /// <summary>
    /// Retrieve the guids for the custom attributes that are not known by
    /// the v1 ISF decoder
    /// </summary>
    /// <param name="attributes"></param>
    /// <param name="count">count of guids returned (can be less than return.Length</param>
    /// <returns></returns>
    static QVector<QUuid> GetUnknownGuids(ExtendedPropertyCollection& attributes, int& count);

    //#region Key/Value pair validation helpers
    /// <summary>
    /// Validates the data to be associated with a ExtendedProperty id
    /// </summary>
    /// <param name="id">ExtendedProperty identifier</param>
    /// <param name="value">data</param>
    /// <remarks>Ignores Ids that are not known (e.g. ExtendedProperties)</remarks>
    static void Validate(QUuid const &id, QVariant const & value);
    //#endregion // Key/Value pair validation helpers

};

#endif // EXTENDEDPROPERTYSERIALIZER_H
