#include "Internal/Ink/InkSerializedFormat/extendedpropertyserializer.h"
#include "Internal/Ink/InkSerializedFormat/guidlist.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Internal/Ink/InkSerializedFormat/compress.h"
#include "Windows/Ink/extendedproperty.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Ink/stylustip.h"
#include "Windows/Ink/drawingflags.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/knownids.h"
#include "Internal/debug.h"

#include <QDataStream>
#include <QBuffer>

INKCANVAS_BEGIN_NAMESPACE

// If the ExtendedProperty identifier matches one of the original ISF/Tablet-internal
//      Guids that did not include embedded type information (e.g. used the OS-internal
//      property storage API), then it is always stored as byte array and does not
//      include type information
bool ExtendedPropertySerializer::UsesEmbeddedTypeInformation(Guid const &propGuid)
{
    for (unsigned int i = 0; i < KnownIdCache::OriginalISFIdTableLength; i++)
    {
        if (propGuid == (KnownIdCache::OriginalISFIdTable[i]))
        {
            return false;
        }
    }

    for (unsigned int i = 0; i < KnownIdCache::TabletInternalIdTableLength; i++)
    {
        if (propGuid == (KnownIdCache::TabletInternalIdTable[i]))
        {
            return false;
        }
    }

    return true;
}

void ExtendedPropertySerializer::EncodeToStream(ExtendedProperty& attribute, QIODevice& stream)
{
    //VarEnum interopTypeInfo;
    QVariant const & data = attribute.Value();
    /*
    // NTRAID#T2-23063-2003/12/10-Microsoft: CODEQUALITY: Find better way to discover which properties should be persistable
    if (attribute.Id == KnownIds::DrawingFlags)
    {
        interopTypeInfo = VarEnum.VT_I4;
    }
    else if (attribute.Id == KnownIds::StylusTip)
    {
        interopTypeInfo = VarEnum.VT_I4;
    }
    else
    {
        // Find the type of the object if it's embedded in the stream
        if (UsesEmbeddedTypeInformation(attribute.Id()))
        {
            interopTypeInfo = SerializationHelper::ConvertToVarEnum(attribute.Value.userType(), true);
        }
        else // Otherwise treat this as byte array
        {
            interopTypeInfo = (VarEnum.VT_ARRAY | VarEnum.VT_UI1);
        }
    }
    */
    EncodeAttribute(attribute.Id(), data, stream);
}

/// <summary>
/// This function returns the Data bytes that accurately describes the object
/// </summary>
/// <returns></returns>
void ExtendedPropertySerializer::EncodeAttribute(Guid const &guid, QVariant const & value, QIODevice& stream)
{
    // samgeo - Presharp issue
    // Presharp gives a warning when local IDisposable variables are not closed
    // in this case, we can't call Dispose since it will also close the underlying stream
    // which still needs to be written to
    //#pragma warning disable 1634, 1691
    //#pragma warning disable 6518
    QDataStream bw(&stream);
    bw.setVersion(QDataStream::Qt_4_0);

    // if this guid used the legacy internal attribute persistence APIs,
    //      then it doesn't include embedded type information (it's always a byte array)
    if (UsesEmbeddedTypeInformation(guid))
    {
        //
        ushort datatype = (ushort)value.userType();
        bw << (datatype);
    }
    // We know the type of the object. We must serialize it accordingly.
    bw << value;
    //#pragma warning restore 6518
    //#pragma warning restore 1634, 1691
}

/// <summary>
/// Encodes a custom attribute to the ISF stream
/// </summary>
uint ExtendedPropertySerializer::EncodeAsISF(Guid const &id, QByteArray data, QIODevice& strm, GuidList& guidList, quint8 compressionAlgorithm, bool fTag)
{
    uint cbWrite = 0;
    uint cbSize = guidList.GetDataSizeIfKnownGuid(id);
    //Debug::Assert(strm != null);

    if (fTag)
    {
        uint uTag = guidList.FindTag(id, true);

        cbWrite += SerializationHelper::Encode(strm, uTag);
    }

    // If cbSize is 0, it is either a custom property or a known property with 0
    // size. In either case, we need to write the size of the individual object
    if (0 == cbSize)
    {
        // Now we need to write the actual data for the property
        cbSize = (uint)data.size();

        QByteArray compresseddata = Compressor::CompressPropertyData(data, compressionAlgorithm);

        // write the encoded compressed size minus the algo byte
        cbWrite += SerializationHelper::Encode(strm, (uint)(compresseddata.size() - 1));

        // Write the raw data
        strm.write(compresseddata.data(), compresseddata.size());
        cbWrite += (uint)compresseddata.size();
    }
    else
    {
        //
        // note that we used to write the nocompression byte, but that
        // was incorrect.  We must not write it because loaders do not
        // expect it for known guids
        //
        // write the algo byte
        //strm.WriteByte(Compressor.NoCompression);
        //cbWrite++;

        // write the raw data without compression
        strm.write(data.data(), data.size());
        cbWrite += (uint)data.size();
    }

    return cbWrite;
}
#if OLD_ISF
/// <summary>
/// Loads a single ExtendedProperty from the stream and add that to the list. Tag may be passed as in
/// the case of Stroke ExtendedPropertyCollection where tag is stored in the stroke descriptor or 0 when tag
/// is embeded in the stream
/// </summary>
/// <param name="stream">Memory buffer to load from</param>
/// <param name="cbSize">Maximum length of buffer to read</param>
/// <param name="guidList">Guid const &cache to read from</param>
/// <param name="tag">Guid const &tag to lookup</param>
/// <param name="guid">Guid const &of property</param>
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
/// <param name="guidList">Guid const &cache to read from</param>
/// <param name="tag">Guid const &tag to lookup</param>
/// <param name="guid">Guid const &of property</param>
/// <param name="data">Data of property</param>
/// <returns>Length of buffer read</returns>
#endif
uint ExtendedPropertySerializer::DecodeAsISF(QIODevice& stream, uint cbSize, GuidList& guidList, KnownTagCache::KnownTagIndex tag, Guid &guid, QVariant& data)
{
    uint cb, cbRead = 0;
    uint cbTotal = cbSize;

    if (0 == cbSize)
    {
        throw  std::runtime_error("SR.Get(SRID.EmptyDataToLoad)");
    }

    if (0 == tag) // no tag is passed, it must be embedded in the data
    {
        uint uiTag;
        cb = SerializationHelper::Decode(stream, uiTag);
        tag = (KnownTagCache::KnownTagIndex)uiTag;
        if (cb > cbTotal)
            throw std::runtime_error("cbSize");

        cbTotal -= cb;
        cbRead += cb;
        Debug::Assert(guid == GuidList::Empty);
        guid = guidList.FindGuid(tag);
    }

    if (guid == GuidList::Empty)
    {
        throw std::runtime_error(("Custom Attribute tag embedded in ISF stream does not match guid table"));
    }

    // Try and find the size
    uint size = guidList.GetDataSizeIfKnownGuid(guid);

    if (size > cbTotal)
        throw std::runtime_error("cbSize");

    // if the size is 0
    if (0 == size)
    {
        // Size must be embedded in the stream. Find out the compressed data size
        cb = SerializationHelper::Decode(stream, size);

        uint cbInsize = size + 1;

        cbRead += cb;
        cbTotal -= cb;
        if (cbInsize > cbTotal)
            throw std::runtime_error("");

        QByteArray bytes(cbInsize, 0);

        uint bytesRead = (uint) stream.read(bytes.data(), cbInsize);
        if (cbInsize != bytesRead)
        {
            throw std::runtime_error(("Read different size from stream then expected"));
        }

        cbRead += cbInsize;
        cbTotal -= cbInsize;

        //Find out the Decompressed buffer size
        //using (MemoryStream decompressedStream = new MemoryStream(Compressor.DecompressPropertyData(bytes)))
        QByteArray decData = Compressor::DecompressPropertyData(bytes);
        QBuffer decompressedStream(&decData);
        decompressedStream.open(QIODevice::ReadOnly);
        {
            // Add the property
            data = DecodeAttribute(guid, decompressedStream);
        }
    }
    else
    {
        // For known size data, we just read the data directly from the stream
        QByteArray bytes(size, 0);

        uint bytesRead = (uint) stream.read(bytes.data(), size);
        if (size != bytesRead)
        {
            throw std::runtime_error(("Read different size from stream then expected"));
        }

        QByteArray subData = Compressor::DecompressPropertyData(bytes);
        QBuffer subStream(&subData);
        subStream.open(QIODevice::ReadOnly);
        {
            data = DecodeAttribute(guid, subStream);
        }

        cbTotal -= size;
        cbRead +=  size;
    }

    return cbRead;
}


/// <summary>
/// Decodes a byte array (stored in the memory stream) into an object
/// If the GUID is one of the internal versions, then the type is assumed
/// to be byte array.
/// If however, the guid is of unknown origin or not v1 internal, then the type
///     information is assumed to be stored in the first 2 bytes of the stream.
/// </summary>
/// <param name="guid">Guid const &of property - to detect origin</param>
/// <param name="memStream">Buffer of data</param>
/// <param name="type">the type info stored in the stream</param>
/// <returns>object stored in data buffer</returns>
/// <remarks>The buffer stream passed in to the method will be closed after reading</remarks>
QVariant ExtendedPropertySerializer::DecodeAttribute(Guid const &, QIODevice& memStream)
{
    // First determine the object type
    //using (BinaryReader br = new BinaryReader(memStream))
    QDataStream br(&memStream);
    br.setVersion(QDataStream::Qt_4_0);
    {
        //
        // if usesEmbeddedTypeInfo is true, we do not
        // read the variant type from the ISF stream.  Instead,
        // we assume it to be a byte[]
        //
        //bool usesEmbeddedTypeInfo = UsesEmbeddedTypeInformation(guid);

        // if the Id has embedded type information then retrieve it from the stream
        //if (usesEmbeddedTypeInfo)
        //{
            // We must read the data type from the stream
        //    type = (VarEnum)br.ReadUInt16();
        //}
        //else
        //{
            // The data is stored as byte array
        //    type = (VarEnum.VT_ARRAY | VarEnum.VT_UI1);
        //}
        QVariant value;
        br >> value;
        return value;
    }
}

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
uint ExtendedPropertySerializer::EncodeAsISF(ExtendedPropertyCollection const & attributes, QIODevice& stream, GuidList& guidList, quint8 compressionAlgorithm, bool fTag)
{
    uint cbWrite = 0;

    for (int i = 0; i < attributes.Count(); i++)
    {
        ExtendedProperty prop = attributes[i];

        //using (MemoryStream localStream = new MemoryStream(10)) //reasonable default
        QBuffer localStream;
        localStream.open(QIODevice::ReadWrite);
        {
            EncodeToStream(prop, localStream);

            QByteArray data = localStream.data();

            cbWrite += EncodeAsISF(prop.Id(), data, stream, guidList, compressionAlgorithm, fTag);
        }
    }

    return cbWrite;
}

/// <summary>
/// Retrieve the guids for the custom attributes that are not known by
/// the v1 ISF decoder
/// </summary>
/// <param name="attributes"></param>
/// <param name="count">count of guids returned (can be less than return.Length</param>
/// <returns></returns>
QVector<Guid> ExtendedPropertySerializer::GetUnknownGuids(ExtendedPropertyCollection& attributes, int& count)
{
    QVector<Guid> guids(attributes.Count());
    count = 0;
    for (int x = 0; x < attributes.Count(); x++)
    {
        ExtendedProperty attribute = attributes[x];
        if (0 == GuidList::FindKnownTag(attribute.Id()))
        {
            guids[count++] = attribute.Id();
        }
    }
    return guids;
}

//#region Key/Value pair validation helpers
/// <summary>
/// Validates the data to be associated with a ExtendedProperty id
/// </summary>
/// <param name="id">ExtendedProperty identifier</param>
/// <param name="value">data</param>
/// <remarks>Ignores Ids that are not known (e.g. ExtendedProperties)</remarks>
void ExtendedPropertySerializer::Validate(Guid const &id, QVariant const & value)
{
    if (id == GuidList::Empty)
    {
        throw std::runtime_error("SR.Get(SRID.InvalidGuid)");
    }

    if (id == KnownIds::Color)
    {
        if (!(value.userType() == qMetaTypeId<QColor>()))
        {
            throw std::runtime_error("value");
        }
    }
        // int attributes
    else if (id == KnownIds::CurveFittingError)
    {
        if (!(value.userType() == QVariant::Int))
        {
            throw std::runtime_error("value");
        }
    }
    else if (id == KnownIds::DrawingFlags)
    {
        // ignore validation of flags
        if (value.userType() != qMetaTypeId<DrawingFlags>())
        {
            throw std::runtime_error("value");
        }
    }
    else if (id == KnownIds::StylusTip)
    {
        int valueType = value.userType();
        bool fStylusTipType = ( valueType == qMetaTypeId<StylusTip>() );
        bool fIntType = ( valueType == qMetaTypeId<int>() );

        if ( !fStylusTipType && !fIntType )
        {
            throw std::runtime_error("value");
        }
        else if ( !StylusTipHelper::IsDefined(value.value<StylusTip>()) )
        {
            throw std::runtime_error("value");
        }
    }
    else if (id == KnownIds::StylusTipTransform)
    {
        //
        // StylusTipTransform gets serialized as a String, but at runtime is a Matrix
        //
        int t = value.userType();
        if ( t != qMetaTypeId<QString>() && t != qMetaTypeId<Matrix>() )
        {
            throw std::runtime_error("value");
        }
        else if ( t == qMetaTypeId<Matrix>() )
        {
            Matrix matrix = value.value<Matrix>();
            if ( !matrix.HasInverse() )
            {
                throw std::runtime_error("value");
            }
            //if ( MatrixHelper::ContainsNaN(matrix))
            //{
            //    throw std::runtime_error(SR.Get(SRID.InvalidMatrixContainsNaN), "value");
            //}
            //if ( MatrixHelper.ContainsInfinity(matrix))
            //{
            //    throw std::runtime_error(SR.Get(SRID.InvalidMatrixContainsInfinity), "value");
            //}

        }
    }
    else if (id == KnownIds::IsHighlighter)
    {
        if ( value.userType() != qMetaTypeId<bool>())
        {
            throw std::runtime_error("value");
        }
    }
    else if ( id == KnownIds::StylusHeight || id == KnownIds::StylusWidth )
    {
        if ( value.userType() != qMetaTypeId<double>() )
        {
            throw std::runtime_error("value");
        }

        double dVal = value.toDouble();

        if (id == KnownIds::StylusHeight)
        {
            if ( Double::IsNaN(dVal) || dVal < DrawingAttributes::MinHeight || dVal > DrawingAttributes::MaxHeight)
            {
                throw std::runtime_error("value");
            }
        }
        else
        {
            if (Double::IsNaN(dVal) ||  dVal < DrawingAttributes::MinWidth || dVal > DrawingAttributes::MaxWidth)
            {
                throw std::runtime_error("value");
            }
        }
    }
    else if ( id == KnownIds::Transparency )
    {
        if ( value.userType() != qMetaTypeId<quint8>() )
        {
            throw std::runtime_error("value");
        }

    }
    else
    {
        if ( !UsesEmbeddedTypeInformation(id) )
        {
            // if this guid used the legacy internal attribute persistence APIs,
            //      then it doesn't include embedded type information (it's always a byte array)
            if ( value.userType() != qMetaTypeId<QByteArray>() )
            {
                throw std::runtime_error("value");
            }
        }
        else
        {
                //do nothing in the default case...
        }
        return;
    }
}
//#endregion // Key/Value pair validation helpers

INKCANVAS_END_NAMESPACE
