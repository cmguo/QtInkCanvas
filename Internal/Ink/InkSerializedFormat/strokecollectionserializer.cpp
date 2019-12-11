#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"
#include "Internal/Ink/InkSerializedFormat/strokeserializer.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Internal/Ink/InkSerializedFormat/guidlist.h"
#include "Internal/Ink/InkSerializedFormat/drawingattributeserializer.h"
#include "Internal/Ink/InkSerializedFormat/strokedescriptor.h"
#include "Internal/Ink/InkSerializedFormat/metricblock.h"
#include "Internal/Ink/InkSerializedFormat/metricentry.h"
#include "Internal/Ink/InkSerializedFormat/transformdescriptor.h"
#include "Internal/Ink/InkSerializedFormat/extendedpropertyserializer.h"
#include "Internal/Ink/InkSerializedFormat/compress.h"
#include "Internal/Ink/InkSerializedFormat/algomodule.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/knownids.h"
#include "Windows/Ink/strokecollection.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Input/styluspointdescription.h"
#include "Internal/finallyhelper.h"
#include "Internal/debug.h"

#include <QSharedPointer>
#include <QIODevice>
#include <QBuffer>
#include <QtMath>

TransformDescriptor StrokeCollectionSerializer::IdentityTransformDescriptor = {
    {1.0},
    1,
    KnownTagCache::KnownTagIndex::TransformIsotropicScale,
};


/// <summary>
/// Initialize the Ink serializer
/// </summary>
/// <param name="coreStrokes">Pointer to the core stroke collection - avoids recreation of collections</param>
StrokeCollectionSerializer::StrokeCollectionSerializer(StrokeCollection& coreStrokes)
    : _coreStrokes(coreStrokes)
{
}

//#endregion


//#region Decoding

//#region Public Methods


/// <summary>
/// Loads a Ink object from a spcified quint8 array in the form of Ink Serialzied Format
/// This method checks for the 'base64:' prefix in the quint8[] because that is how V1
/// saved ISF
/// </summary>
/// <param name="inkData"></param>
void StrokeCollectionSerializer::DecodeISF(QIODevice& inkData)
{
    try
    {
        // First examine the input data header
        bool isBase64;
        bool isGif;
        quint32 cbData;

        ExamineStreamHeader(inkData, isBase64, isGif, cbData);
        if (isBase64)
        {
            //
            // this is a funky tablet v1 based quint8[] that is base64 encoded...
            // each 4 bytes in this array corresponds to 3 bytes of ISF data.
            // EXCEPT the first 7 bytes which are saved with the value
            // 'base64:' and must not be base64 decoded.
            // and the last null terminator (if present)
            //
            //  The following code does two things:
            //  1) Convert each quint8 to a char so it can be base64 decoded
            //  2) Strips the first 7 resulting characters
            //
            int isfBase64PrefixLength = sizeof(Base64HeaderBytes);
            // the previous call to ExamineStreamHeader guarantees that inkData.Length > isfBase64PrefixLength
            Debug::Assert(inkData.bytesAvailable() > isfBase64PrefixLength);

            inkData.seek((long)isfBase64PrefixLength);
            QList<char> charData;
            charData.reserve((int)inkData.size());
            char intByte;
            while (inkData.getChar(&intByte))
            {
                charData.append(intByte);
            }

            if (0 == (quint8)(charData[charData.size() - 1]))
            {
                //strip the null terminator
                charData.pop_back();
            }

            //char[] chars = charData.ToArray();
            //quint8[] isfData = Convert.FromBase64CharArray(chars, 0, chars.Length);
            //MemoryStream ms = new MemoryStream(isfData);
            //if (IsGIFData(ms))
            //{
            //    DecodeRawISF(SystemDrawingHelper.GetCommentFromGifStream(ms));
            //}
            //else
            //{
            //    DecodeRawISF(ms);
            //}
        }
        //else if (true == isGif)
        //{
        //    DecodeRawISF(SystemDrawingHelper.GetCommentFromGifStream(inkData));
        //}
        else
        {
            DecodeRawISF(inkData);
        }
    }
#if DEBUG
    catch (ArgumentException ex)
    {
        //only include an inner exception in debug builds
        throw std::exception(SR.Get(SRID.IsfOperationFailed), ex);
    }
    catch (InvalidOperationException ex)
    {
        //only include an inner exception in debug builds
        throw std::exception(SR.Get(SRID.IsfOperationFailed), ex);
    }
    catch (IndexOutOfRangeException ex)
    {
        //only include an inner exception in debug builds
        throw std::exception(SR.Get(SRID.IsfOperationFailed), ex);
    }
    catch (NullReferenceException ex)
    {
        //only include an inner exception in debug builds
        throw std::exception(SR.Get(SRID.IsfOperationFailed), ex);
    }
    catch (EndOfStreamException ex)
    {
        //only include an inner exception in debug builds
        throw std::exception(SR.Get(SRID.IsfOperationFailed), ex);
    }
    catch (OverflowException ex)
    {
        //only include an inner exception in debug builds
        throw std::exception(SR.Get(SRID.IsfOperationFailed), ex);
    }
#else
    //catch (ArgumentException)
    //{
    //    throw std::exception(SR.Get(SRID.IsfOperationFailed), "stream");//stream comes from StrokeCollection.ctor()
    //}
    //catch (InvalidOperationException)
    //{
    //    throw std::exception(SR.Get(SRID.IsfOperationFailed), "stream");//stream comes from StrokeCollection.ctor()
    //}
    //catch (IndexOutOfRangeException)
    //{
    //    throw std::exception(SR.Get(SRID.IsfOperationFailed), "stream");//stream comes from StrokeCollection.ctor()
    //}
    //catch (NullReferenceException)
    //{
    //    throw std::exception(SR.Get(SRID.IsfOperationFailed), "stream");//stream comes from StrokeCollection.ctor()
    //}
    //catch (EndOfStreamException)
    //{
    //    throw std::exception(SR.Get(SRID.IsfOperationFailed), "stream");//stream comes from StrokeCollection.ctor()
    //}
    //catch (OverflowException)
    //{
    //    throw std::exception(SR.Get(SRID.IsfOperationFailed), "stream");//stream comes from StrokeCollection.ctor()
    //}
    catch (std::exception)
    {

    }

#endif
}

//#endregion
static int ReadByte(QIODevice& s)
{
    char b;
    return s.getChar(&b) ? b : -1;
}

//#region Methods

/// <summary>
/// Loads the strokeIds from the stream, we need to do this to decrement the count of bytes
/// </summary>
quint32 StrokeCollectionSerializer::LoadStrokeIds(QIODevice& isfStream, quint32 cbSize)
{
    if (0 == cbSize)
        return 0;

    quint32 cb;
    quint32 cbTotal = cbSize;

    // First decode the no of ids
    quint32 count;

    cb = SerializationHelper::Decode(isfStream, count);
    if (cb > cbTotal)
        throw std::exception("isfStream");

    cbTotal -= cb;
    if (0 == count)
        return (cbSize - cbTotal);

    cb = cbTotal;

    quint8* inputdata = new quint8[cb];

    // read the stream
    quint32 bytesRead = ReliableRead(isfStream, inputdata, cb);
    if (cb != bytesRead)
    {
        throw std::exception("isfStream");
    }
    cbTotal -= cb;

    if (0 != cbTotal)
        throw std::exception("isfStream");

    return cbSize;
}


bool StrokeCollectionSerializer::IsGIFData(QIODevice& inkdata)
{
    //Debug::Assert(inkdata != null);
    long currentPosition = inkdata.pos();
    //try
    {
        FinallyHelper final([currentPosition, &inkdata](){
            inkdata.seek(currentPosition);
        });
        return ((quint8)ReadByte(inkdata) == 'G' &&
                (quint8)ReadByte(inkdata) == 'I' &&
                (quint8)ReadByte(inkdata) == 'F');
    }
    //finally
    //{
        //reset position
    //    inkdata.Position = currentPosition;
    //}
}

void StrokeCollectionSerializer::ExamineStreamHeader(QIODevice& inkdata, bool& fBase64, bool& fGif, quint32& cbData)
{
    fGif = false;
    cbData = 0;
    fBase64 = false;

    if (inkdata.bytesAvailable() >= 7)
    {
        fBase64 = IsBase64Data(inkdata);
    }

    // Check for RAW gif
    if (!fBase64 && inkdata.bytesAvailable() >= 3)
    {
        fGif = IsGIFData(inkdata);
    }

    return;
}


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
[SecurityCritical, SecurityTreatAsSafe]
#else
/// <summary>
/// Takes an ISF Stream and populates the StrokeCollection
///  attached to this
/// </summary>
/// <param name="inputStream">a Stream the raw isf to decode</param>
#endif
void StrokeCollectionSerializer::DecodeRawISF(QIODevice& inputStream)
{
    //Debug::Assert(inputStream != null);

    KnownTagCache::KnownTagIndex isfTag;
    quint32 remainingBytesInStream;
    quint32 bytesDecodedInCurrentTag = 0;
    bool strokeDescriptorBlockDecoded = false;
    bool drawingAttributesBlockDecoded = false;
    bool metricBlockDecoded = false;
    bool transformDecoded = false;
    quint32 strokeDescriptorTableIndex = 0;
    quint32 oldStrokeDescriptorTableIndex = 0xFFFFFFFF;
    quint32 drawingAttributesTableIndex = 0;
    quint32 oldDrawingAttributesTableIndex = 0xFFFFFFFF;
    quint32 metricDescriptorTableIndex = 0;
    quint32 oldMetricDescriptorTableIndex = 0xFFFFFFFF;
    quint32 transformTableIndex = 0;
    quint32 oldTransformTableIndex = 0xFFFFFFFF;
    GuidList guidList;
    int strokeIndex = 0;

    QSharedPointer<StylusPointDescription> currentStylusPointDescription;
    QMatrix currentTabletToInkTransform;

    _strokeDescriptorTable.clear();
    _drawingAttributesTable.clear();
    _transformTable.clear();
    _metricTable.clear();

    // First make sure this ink is empty
    if (0 != _coreStrokes.size() || _coreStrokes.ExtendedProperties().Count() != 0)
    {
        throw std::exception(("ISF decoder cannot operate on non-empty ink container"));
    }
#if OLD_ISF
    //
    // store a compressor reference at this scope, if it is needed (if there is a compresson header) and
    // therefore instanced during this routine, we will dispose of it
    // in the finally block
    //
    Compressor compressor = null;

    try
    {
#endif

    // First read the isfTag
    quint32 uiTag;
    quint32 localBytesDecoded = SerializationHelper::Decode(inputStream, uiTag);
    if (0x00 != uiTag)
        throw std::exception("SR.Get(SRID.InvalidStream)");

    // Now read the size of the stream
    localBytesDecoded = SerializationHelper::Decode(inputStream, remainingBytesInStream);
    ISFDebugTrace("Decoded Stream Size in Bytes: ");
    if (0 == remainingBytesInStream)
        return;

    while (0 < remainingBytesInStream)
    {
        bytesDecodedInCurrentTag = 0;

        // First read the isfTag
        localBytesDecoded = SerializationHelper::Decode(inputStream, uiTag);
        isfTag = (KnownTagCache::KnownTagIndex)uiTag;
        if (remainingBytesInStream >= localBytesDecoded)
            remainingBytesInStream -= localBytesDecoded;
        else
        {
            throw std::exception(("Invalid ISF data"));
        }

        ISFDebugTrace("Decoding Tag: ");
        switch (isfTag)
        {
            case KnownTagCache::KnownTagIndex::GuidTable:
            case KnownTagCache::KnownTagIndex::DrawingAttributesTable:
            case KnownTagCache::KnownTagIndex::DrawingAttributesBlock:
            case KnownTagCache::KnownTagIndex::StrokeDescriptorTable:
            case KnownTagCache::KnownTagIndex::StrokeDescriptorBlock:
            case KnownTagCache::KnownTagIndex::MetricTable:
            case KnownTagCache::KnownTagIndex::MetricBlock:
            case KnownTagCache::KnownTagIndex::TransformTable:
            case KnownTagCache::KnownTagIndex::ExtendedTransformTable:
            case KnownTagCache::KnownTagIndex::Stroke:
            case KnownTagCache::KnownTagIndex::CompressionHeader:
            case KnownTagCache::KnownTagIndex::PersistenceFormat:
            case KnownTagCache::KnownTagIndex::HimetricSize:
            case KnownTagCache::KnownTagIndex::StrokeIds:
                {
                    localBytesDecoded = SerializationHelper::Decode(inputStream, bytesDecodedInCurrentTag);
                    if (remainingBytesInStream < (localBytesDecoded + bytesDecodedInCurrentTag))
                    {
                        throw std::exception("inputStream");
                    }

                    remainingBytesInStream -= localBytesDecoded;

                    // Based on the isfTag figure what information we're loading
                    switch (isfTag)
                    {
                        case KnownTagCache::KnownTagIndex::GuidTable:
                            {
                                // Load guid Table
                                localBytesDecoded = guidList.Load(inputStream, bytesDecodedInCurrentTag);
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::DrawingAttributesTable:
                            {
                                // Load drawing attributes table
                                localBytesDecoded = LoadDrawAttrsTable(inputStream, guidList, bytesDecodedInCurrentTag);
                                drawingAttributesBlockDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::DrawingAttributesBlock:
                            {
                                //initialize to V1 defaults, we do it this way as opposed
                                //to dr.DrawingFlags = 0 because this was a perf hot spot
                                //and instancing the epc first mitigates it
                                ExtendedPropertyCollection* epc = new ExtendedPropertyCollection();
                                epc->Add(KnownIds::DrawingFlags, QVariant::fromValue(DrawingFlag::Polyline));
                                QSharedPointer<DrawingAttributes> dr(new DrawingAttributes(epc));
                                localBytesDecoded = DrawingAttributeSerializer::DecodeAsISF(inputStream, guidList, bytesDecodedInCurrentTag, *dr);

                                _drawingAttributesTable.append(dr);
                                drawingAttributesBlockDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::StrokeDescriptorTable:
                            {
                                // Load stroke descriptor table
                                localBytesDecoded = DecodeStrokeDescriptorTable(inputStream, bytesDecodedInCurrentTag);
                                strokeDescriptorBlockDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::StrokeDescriptorBlock:
                            {
                                // Load a single stroke descriptor
                                localBytesDecoded = DecodeStrokeDescriptorBlock(inputStream, bytesDecodedInCurrentTag);
                                strokeDescriptorBlockDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::MetricTable:
                            {
                                // Load Metric Table
                                localBytesDecoded = DecodeMetricTable(inputStream, bytesDecodedInCurrentTag);
                                metricBlockDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::MetricBlock:
                            {
                                // Load a single Metric Block
                                MetricBlock* blk;

                                localBytesDecoded = DecodeMetricBlock(inputStream, bytesDecodedInCurrentTag, blk);
                                _metricTable.clear();
                                _metricTable.append(blk);
                                metricBlockDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::TransformTable:
                            {
                                // Load Transform Table
                                localBytesDecoded = DecodeTransformTable(inputStream, bytesDecodedInCurrentTag, false);
                                transformDecoded = true;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::ExtendedTransformTable:
                            {
                                // non-double transform table should have already been loaded
                                if (!transformDecoded)
                                {
                                    throw std::exception(("Invalid ISF data"));
                                }

                                // Load double-sized Transform Table
                                localBytesDecoded = DecodeTransformTable(inputStream, bytesDecodedInCurrentTag, true);
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::PersistenceFormat:
                            {
                                quint32 fmt;

                                localBytesDecoded = SerializationHelper::Decode(inputStream, fmt);
                                // Set the appropriate persistence information
                                if (0 == fmt)
                                {
                                    CurrentPersistenceFormat = PersistenceFormat::InkSerializedFormat;
                                }
                                else if (0x00000001 == fmt)
                                {
                                    CurrentPersistenceFormat = PersistenceFormat::Gif;
                                }


                                break;
                            }

                        case KnownTagCache::KnownTagIndex::HimetricSize:
                            {
                                // Loads the Hi Metric Size for Fortified GIFs
                                int sz;

                                localBytesDecoded = SerializationHelper::SignDecode(inputStream, sz);
                                if (localBytesDecoded > remainingBytesInStream)
                                    throw std::exception(("Invalid ISF data"));

                                _himetricSize.setX((double)sz);
                                localBytesDecoded += SerializationHelper::SignDecode(inputStream, sz);

                                _himetricSize.setY((double)sz);
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::CompressionHeader:
                            {
#if OLD_ISF
                                quint8[] data = new byte[bytesDecodedInCurrentTag];

                                // read the header from the stream
                                quint32 bytesRead = ReliableRead(inputStream, data, bytesDecodedInCurrentTag);
                                if (bytesDecodedInCurrentTag != bytesRead)
                                {
                                    throw std::exception(("Read different size from stream then expected"), "isfStream");
                                }

                                quint32 size = bytesDecodedInCurrentTag;
                                compressor = new Compressor(data, ref size);
                                // in case the actual number of bytes read by the compressor
                                //      is less than the encoder had expected (e.g. compression
                                //      header was encoded as 10 bytes, but only 7 bytes were read)
                                //      then we don't want to adjust the stream position because
                                //      there are likely other following tags that are encoded
                                //      after the compression tag. This should never happen,
                                //      so just fail if the compressor is broken or the ISF is
                                //      corrupted.
                                if (size != bytesDecodedInCurrentTag)
                                {
                                    throw std::exception(("Compressor intialization reported inconsistent size"));
                                }
#else
                                //just advance the inputstream position, we don't need
                                //no compression header in the new isf decoding
                                inputStream.seek(bytesDecodedInCurrentTag + inputStream.pos());
#endif
                                localBytesDecoded = bytesDecodedInCurrentTag;
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::StrokeIds:
                            {
                                localBytesDecoded = LoadStrokeIds(inputStream, bytesDecodedInCurrentTag);
                                break;
                            }

                        case KnownTagCache::KnownTagIndex::Stroke:
                            {
                                ISFDebugTrace("   Decoding Stroke Id#("")");

                                StrokeDescriptor* strokeDescriptor = nullptr;

                                // Load the stroke descriptor based on the index from the list of unique
                                // stroke descriptors
                                if (strokeDescriptorBlockDecoded)
                                {
                                    if (oldStrokeDescriptorTableIndex != strokeDescriptorTableIndex)
                                    {
                                        if (_strokeDescriptorTable.size() <= strokeDescriptorTableIndex)
                                            throw std::exception(("Invalid ISF data"));
                                    }

                                    strokeDescriptor = _strokeDescriptorTable[(int)strokeDescriptorTableIndex];
                                }

                                // use new transform if the last transform is uninit'd or has changed
                                if (oldTransformTableIndex != transformTableIndex)
                                {
                                    // if transform was specified in the ISF stream
                                    if (transformDecoded)
                                    {
                                        if (_transformTable.size() <= transformTableIndex)
                                            throw std::exception(("Invalid ISF data"));

                                        // Load the transform descriptor based on the index from the list of unique
                                        // transforn descriptors
                                        currentTabletToInkTransform = LoadTransform(_transformTable[(int)transformTableIndex]);
                                    }

                                    oldTransformTableIndex = transformTableIndex; // cache the transform by remembering the index

                                    // since ISF is stored in HIMETRIC, and we want to expose packet data
                                    //      as Avalon units, we'll update the convert the transform before loading the stroke
                                    currentTabletToInkTransform.scale(HimetricToAvalonMultiplier, HimetricToAvalonMultiplier);
                                }

                                MetricBlock* metricBlock = nullptr;

                                // Load the metric block based on the index from the list of unique metric blocks
                                if (metricBlockDecoded)
                                {
                                    if (oldMetricDescriptorTableIndex != metricDescriptorTableIndex)
                                    {
                                        if (_metricTable.size() <= metricDescriptorTableIndex)
                                            throw std::exception(("Invalid ISF data"));
                                    }

                                    metricBlock = _metricTable[(int)metricDescriptorTableIndex];
                                }

                                QSharedPointer<DrawingAttributes> activeDrawingAttributes;

                                // Load the drawing attributes based on the index from the list of unique drawing attributes
                                if (drawingAttributesBlockDecoded)
                                {
                                    if (oldDrawingAttributesTableIndex != drawingAttributesTableIndex)
                                    {
                                        if (_drawingAttributesTable.size() <= drawingAttributesTableIndex)
                                            throw std::exception(("Invalid ISF data"));

                                        oldDrawingAttributesTableIndex = drawingAttributesTableIndex;
                                    }
                                    QSharedPointer<DrawingAttributes> currDA = _drawingAttributesTable[(int)drawingAttributesTableIndex];
                                    //we always clone so we don't get strokes that share DAs, which can lead
                                    //to all sorts of unpredictable behavior (ex: see Windows OS Bugs 1450047)
                                    activeDrawingAttributes = currDA->Clone();
                                }

                                // if we didn't find an existing da to use, instance a new one
                                if (activeDrawingAttributes == nullptr)
                                {
                                    activeDrawingAttributes.reset(new DrawingAttributes());
                                }

                                // Now create the StylusPacketDescription from the stroke descriptor and metric block
                                if (oldMetricDescriptorTableIndex != metricDescriptorTableIndex || oldStrokeDescriptorTableIndex != strokeDescriptorTableIndex)
                                {
                                    currentStylusPointDescription = BuildStylusPointDescription(strokeDescriptor, metricBlock, guidList);
                                    oldStrokeDescriptorTableIndex = strokeDescriptorTableIndex;
                                    oldMetricDescriptorTableIndex = metricDescriptorTableIndex;
                                }

                                // Load the stroke
                                QSharedPointer<Stroke> localStroke;
#if OLD_ISF
                                localBytesDecoded = StrokeSerializer::DecodeStroke(inputStream, bytesDecodedInCurrentTag, guidList, strokeDescriptor, currentStylusPointDescription, activeDrawingAttributes, currentTabletToInkTransform, compressor, localStroke);
#else
                                localBytesDecoded = StrokeSerializer::DecodeStroke(inputStream, bytesDecodedInCurrentTag, guidList, *strokeDescriptor, currentStylusPointDescription, activeDrawingAttributes, currentTabletToInkTransform, localStroke);
#endif

                                if (localStroke != nullptr)
                                {
                                    _coreStrokes.AddWithoutEvent(localStroke);
                                    strokeIndex++;
                                }
                                break;
                            }

                        default:
                            {
                                throw std::exception(("Invalid ISF tag logic"));
                            }
                    }

                    // if this isfTag's decoded size != expected size, then error out
                    if (localBytesDecoded != bytesDecodedInCurrentTag)
                    {
                        throw std::exception(("Invalid ISF data"));
                    }

                    break;
                }

            case KnownTagCache::KnownTagIndex::Transform:
            case KnownTagCache::KnownTagIndex::TransformIsotropicScale:
            case KnownTagCache::KnownTagIndex::TransformAnisotropicScale:
            case KnownTagCache::KnownTagIndex::TransformRotate:
            case KnownTagCache::KnownTagIndex::TransformTranslate:
            case KnownTagCache::KnownTagIndex::TransformScaleAndTranslate:
                {
                    // Load a single Transform Block
                    TransformDescriptor* xform;

                    bytesDecodedInCurrentTag = DecodeTransformBlock(inputStream, isfTag, remainingBytesInStream, false, xform);
                    transformDecoded = true;
                    _transformTable.clear();
                    _transformTable.append(xform);
                    break;
                }

            case KnownTagCache::KnownTagIndex::TransformTableIndex:
                {
                    // Load the Index into the Transform Table which will be used by the stroke following this till
                    // a next different Index is found
                    bytesDecodedInCurrentTag = SerializationHelper::Decode(inputStream, transformTableIndex);
                    break;
                }

            case KnownTagCache::KnownTagIndex::MetricTableIndex:
                {
                    // Load the Index into the Metric Table which will be used by the stroke following this till
                    // a next different Index is found
                    bytesDecodedInCurrentTag = SerializationHelper::Decode(inputStream, metricDescriptorTableIndex);
                    break;
                }

            case KnownTagCache::KnownTagIndex::DrawingAttributesTableIndex:
                {
                    // Load the Index into the Drawing Attributes Table which will be used by the stroke following this till
                    // a next different Index is found
                    bytesDecodedInCurrentTag = SerializationHelper::Decode(inputStream, drawingAttributesTableIndex);
                    break;
                }

            case KnownTagCache::KnownTagIndex::InkSpaceRectangle:
                {
                    // Loads the Ink Space Rectangle information
                    bytesDecodedInCurrentTag = DecodeInkSpaceRectangle(inputStream, remainingBytesInStream);
                    break;
                }

            case KnownTagCache::KnownTagIndex::StrokeDescriptorTableIndex:
                {
                    // Load the Index into the Stroke Descriptor Table which will be used by the stroke following this till
                    // a next different Index is found
                    bytesDecodedInCurrentTag = SerializationHelper::Decode(inputStream, strokeDescriptorTableIndex);
                    break;
                }

            default:
                {
                    if ((uint)isfTag >= KnownIdCache::CustomGuidBaseIndex || ((uint)isfTag >= KnownTagCache::KnownTagCount && ((uint)isfTag < (KnownTagCache::KnownTagCount + KnownIdCache::OriginalISFIdTableLength))))
                    {
                        ISFDebugTrace("  CUSTOM_GUID=");

                        // Loads any custom property data
                        bytesDecodedInCurrentTag = remainingBytesInStream;

                        QUuid guid = guidList.FindGuid(isfTag);
                        if (guid == GuidList::Empty)
                        {
                            throw std::exception(("Global Custom Attribute tag embedded in ISF stream does not match guid table"));
                        }


                        QVariant data;

                        // load the custom property data from the stream (and decode the type)
                        localBytesDecoded = ExtendedPropertySerializer::DecodeAsISF(inputStream, bytesDecodedInCurrentTag, guidList, isfTag, guid, data);
                        if (localBytesDecoded > bytesDecodedInCurrentTag)
                        {
                            throw std::exception(("Invalid ISF data"));
                        }


                        // add the guid/data pair into the property collection (don't redecode the type)
                        _coreStrokes.ExtendedProperties()[guid] = data;
                    }
                    else
                    {
                        // Skip objects that this library doesn't know about
                        // First read the size associated with this unknown isfTag
                        localBytesDecoded = SerializationHelper::Decode(inputStream, bytesDecodedInCurrentTag);
                        if (remainingBytesInStream < (localBytesDecoded + bytesDecodedInCurrentTag))
                        {
                            throw std::exception(("Invalid ISF data"));
                        }
                        else
                        {
                            inputStream.seek(bytesDecodedInCurrentTag + localBytesDecoded + inputStream.pos());
                        }
                    }

                    bytesDecodedInCurrentTag = localBytesDecoded;
                    break;
                }
        }
        ISFDebugTrace("    Size = ");
        if (bytesDecodedInCurrentTag > remainingBytesInStream)
        {
            throw std::exception(("Invalid ISF data"));
        }

        // update remaining ISF buffer length with decoded so far
        remainingBytesInStream -= bytesDecodedInCurrentTag;
    }
    if (0 != remainingBytesInStream)
        throw std::exception(("Invalid ISF data"));
}

/// <summary>
/// Loads a DrawingAttributes Table from the stream and adds individual drawing attributes to the drawattr
/// list passed
/// </summary>
quint32 StrokeCollectionSerializer::LoadDrawAttrsTable(QIODevice& strm, GuidList& guidList, quint32 cbSize)
{
    _drawingAttributesTable.clear();

    // First, allocate a temporary buffer and read the stream into it.
    // These will be compressed DRAW_ATTR structures.
    quint32 cbTotal = cbSize;

    // OK, now we count the number of DRAW_ATTRS compressed into this block
    quint32 cbDA = 0;

    while (cbTotal > 0)
    {
        // First read the size of the first drawing attributes block
        quint32 cb = SerializationHelper::Decode(strm, cbDA);

        if (cbSize < cb)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cb;
        if (cbTotal < cbDA)
            throw std::exception(("Invalid ISF data"));



        // Create a new drawing attribute
        QSharedPointer<DrawingAttributes> attributes(new DrawingAttributes());
        // pull off our defaults onthe drawing attribute as we need to
        //  respect what the ISF has.
        attributes->SetDrawingFlags(0);
        cb = DrawingAttributeSerializer::DecodeAsISF(strm, guidList, cbDA, *attributes);

        // Load the stream into this attribute
        if (cbSize < cbDA)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cbDA;

        // Add this attribute to the global list
        _drawingAttributesTable.append(attributes);
    }

    if (0 != cbTotal)
        throw std::exception(("Invalid ISF data"));


    return cbSize;
}

/// <summary>
/// Reads and Decodes a stroke descriptor information from the stream. For details on how it is stored
/// please refer the spec
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <param name="descr"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeStrokeDescriptor(QIODevice& strm, quint32 cbSize, StrokeDescriptor*& descr)
{
    descr = new StrokeDescriptor();
    if (0 == cbSize)
        return 0;

    quint32 cb;
    quint32 cbBlock = cbSize;

    while (cbBlock > 0)
    {
        // first read the tag
        KnownTagCache::KnownTagIndex tag;
        quint32 uiTag;

        cb = SerializationHelper::Decode(strm, uiTag);
        tag = (KnownTagCache::KnownTagIndex)uiTag;
        if (cb > cbBlock)
            throw std::exception(("Invalid ISF data"));

        cbBlock -= cb;
        descr->Template.append(tag);

        // If this is TAG_BUTTONS
        if (KnownTagCache::KnownTagIndex::Buttons == tag && cbBlock > 0)
        {
            quint32 cbButton;

            // Read the no. of buttons first
            cb = SerializationHelper::Decode(strm, cbButton);
            if (cb > cbBlock)
                throw std::exception(("Invalid ISF data"));

            cbBlock -= cb;
            descr->Template.append((KnownTagCache::KnownTagIndex)cbButton);
            while (cbBlock > 0 && cbButton > 0)
            {
                quint32 dw;

                cb = SerializationHelper::Decode(strm, dw);
                if (cb > cbBlock)
                    throw std::exception(("Invalid ISF data"));

                cbBlock -= cb;
                cbButton--;
                descr->Template.append((KnownTagCache::KnownTagIndex)dw);
            }
        }
        else if (KnownTagCache::KnownTagIndex::StrokePropertyList == tag && cbBlock > 0)
        {
            // Usually stroke property comes last in the template. Hence everything below this is
            // are Tags for strokes extended properties
            while (cbBlock > 0)
            {
                quint32 dw;

                cb = SerializationHelper::Decode(strm, dw);
                if (cb > cbBlock)
                    throw std::exception(("Invalid ISF data"));

                cbBlock -= cb;
                descr->Template.append((KnownTagCache::KnownTagIndex)dw);
            }
        }
    }

    if (0 != cbBlock)
        throw std::exception("strm");

    return cbSize;
}


/// <summary>
/// Reads and Decodes a stroke descriptor information from the stream. For details on how it is stored
/// please refer the spec
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeStrokeDescriptorBlock(QIODevice& strm, quint32 cbSize)
{
    _strokeDescriptorTable.clear();
    if (0 == cbSize)
        return 0;

    StrokeDescriptor* descr;
    quint32 cbRead = DecodeStrokeDescriptor(strm, cbSize, descr);

    if (cbRead != cbSize)
        throw std::exception(("Invalid ISF data"));

    _strokeDescriptorTable.append(descr);
    return cbRead;
}


/// <summary>
/// Reads and Decodes a number of stroke descriptor information from the stream. For details on how they are stored
/// please refer the spec
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeStrokeDescriptorTable(QIODevice& strm, quint32 cbSize)
{
    _strokeDescriptorTable.clear();
    if (0 == cbSize)
        return 0;

    quint32 cb;                // Tracks the total no of bytes read from the stream
    quint32 cbTotal = cbSize;  // Tracks how many more bytes can be read from the stream for the table. Limited by cbSize

    while (cbTotal > 0)
    {
        // First decode the size of the next block
        quint32 cbBlock;

        cb = SerializationHelper::Decode(strm, cbBlock);
        if (cb > cbTotal)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cb;
        if (cbBlock > cbTotal)
            throw std::exception(("Invalid ISF data"));



        StrokeDescriptor* descr;

        cb = DecodeStrokeDescriptor(strm, cbBlock, descr);
        if (cb != cbBlock)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cb;

        // Add this stroke descriptor to the list of global stroke descriptors
        _strokeDescriptorTable.append(descr);
    }

    if (0 != cbTotal)
        throw std::exception(("Invalid ISF data"));


    return cbSize;
}


/// <summary>
/// Decodes metric table from the stream. For information on how they are stored in the stream, please refer to the spec.
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeMetricTable(QIODevice& strm, quint32 cbSize)
{
    _metricTable.clear();
    if (cbSize == 0)
        return 0;

    quint32 cb;
    quint32 cbTotal = cbSize;

    // This data is a list of Metric block. Each block starts with size of the block. After that it contains an
    // array of Metric Entries. Each metric enty comprises of size of the entry, tag for the property and the metric
    // properties.
    while (cbTotal > 0)
    {
        // First read the size of the metric block
        quint32 dw;

        cb = SerializationHelper::Decode(strm, dw);
        if (cb + dw > cbTotal)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cb;

        MetricBlock* newblock;

        cb = DecodeMetricBlock(strm, dw, newblock);
        if (cb != dw)
            throw std::exception("strm");


        cbTotal -= cb;
        _metricTable.append(newblock);
    }

    if (0 != cbTotal)
        throw std::exception("strm");


    return cbSize;
}


/// <summary>
/// Decodes a Metric Block from the stream. For information on how they are stored in the stream, please refer to the spec.
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <param name="block"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeMetricBlock(QIODevice& strm, quint32 cbSize, MetricBlock*& block)
{
    // allocate the block
    block = new MetricBlock();
    if (cbSize == 0)
        return 0;

    quint32 cb;
    quint32 cbTotal = cbSize;
    quint32 size;

    while (cbTotal > 0)
    {
        // First decode the tag for this entry
        quint32 dw;

        cb = SerializationHelper::Decode(strm, dw);
        if (cb > cbTotal)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cb;

        // Next read the size of the metric data
        cb = SerializationHelper::Decode(strm, size);
        if (cb + size > cbTotal)
            throw std::exception(("Invalid ISF data"));


        cbTotal -= cb;

        // now create new metric entry
        MetricEntry* entry = new MetricEntry();

        entry->SetTag((KnownTagCache::KnownTagIndex)dw);

        QVector<quint8> data(size);

        quint32 bytesRead = ReliableRead(strm, data.data(), size);
        cbTotal -= bytesRead;

        if ( bytesRead != size )
        {
            // Make sure the bytes read are expected. If not, we should bail out.
            // An exception will be thrown.
            break;
        }

        entry->SetData(data);
        block->AddMetricEntry(entry);

    }

    if (0 != cbTotal)
        throw std::exception("strm");


    return cbSize;
}


/// <summary>
/// Reads and Decodes a Table of Transform Descriptors from the stream. For information on how they are stored
/// in the stream, please refer to the spec.
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <param name="useDoubles"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeTransformTable(QIODevice& strm, quint32 cbSize, bool useDoubles)
{
    // only clear the transform table if not using doubles
    //      (e.g. first pass through transform table)
    if (!useDoubles)
    {
        _transformTable.clear();
    }

    if (0 == cbSize)
        return 0;

    quint32 cb;
    quint32 cbTotal = cbSize;
    int tableIndex = 0;

    while (cbTotal > 0)
    {
        KnownTagCache::KnownTagIndex tag;
        quint32 uiTag;
        cb = SerializationHelper::Decode(strm, uiTag);
        tag = (KnownTagCache::KnownTagIndex)uiTag;
        if (cb > cbTotal)
            throw std::exception(("Invalid ISF data"));

        cbTotal -= cb;

        TransformDescriptor* xform;

        cb = DecodeTransformBlock(strm, tag, cbTotal, useDoubles, xform);
        cbTotal -= cb;
        if (useDoubles)
        {
            _transformTable[tableIndex] = xform;
        }
        else
        {
            _transformTable.append(xform);
        }

        tableIndex++;
    }

    if (0 != cbTotal)
        throw std::exception(("Invalid ISF data"));

    return cbSize;
}

/// <summary>
/// ReliableRead
/// </summary>
/// <param name="stream"></param>
/// <param name="buffer"></param>
/// <param name="requestedCount"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::ReliableRead(QIODevice& stream, quint8* buffer, quint32 requestedCount)
{
    if (buffer == nullptr)
    {
        throw std::exception(("Invalid argument passed to ReliableRead"));
    }

    // let's read the whole block into our buffer
    quint32 totalBytesRead = 0;
    while (totalBytesRead < requestedCount)
    {
        int bytesRead = stream.read((char *)buffer + totalBytesRead,
                        (int)(requestedCount - totalBytesRead));
        if (bytesRead == 0)
        {
            break;
        }
        totalBytesRead += (uint)bytesRead;
    }
    return totalBytesRead;
}


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
quint32 StrokeCollectionSerializer::DecodeTransformBlock(QIODevice& strm, KnownTagCache::KnownTagIndex tag, quint32 cbSize, bool useDoubles, TransformDescriptor*& xform)
{
    xform = new TransformDescriptor();
    xform->Tag = tag;

    quint32 cbRead = 0;
    //quint32 cbTotal = cbSize;

    if (0 == cbSize)
        return 0;

    // samgeo - Presharp issue
    // Presharp gives a warning when local IDisposable variables are not closed
    // in this case, we can't call Dispose since it will also close the underlying stream
    // which still needs to be read from
//#pragma warning disable 1634, 1691
//#pragma warning disable 6518
    QDataStream bw(&strm);

    if (KnownTagCache::KnownTagIndex::TransformRotate == tag)
    {
        quint32 angle;

        cbRead = SerializationHelper::Decode(strm, angle);
        if (cbRead > cbSize)
            throw std::exception(("Invalid ISF data"));


        xform->Transform[0] = (double)angle;
        xform->Size = 1;
    }
    else
    {
        if (tag == KnownTagCache::KnownTagIndex::TransformIsotropicScale)
        {
            xform->Size = 1;
        }
        else if (tag == KnownTagCache::KnownTagIndex::TransformAnisotropicScale || tag == KnownTagCache::KnownTagIndex::TransformTranslate)
        {
            xform->Size = 2;
        }
        else if (tag == KnownTagCache::KnownTagIndex::TransformScaleAndTranslate)
        {
            xform->Size = 4;
        }
        else
        {
            xform->Size = 6;
        }

        if (useDoubles)
        {
            cbRead = xform->Size * 8;
        }
        else
        {
            cbRead = xform->Size * 4;
        }

        if (cbRead > cbSize)
            throw std::exception(("Invalid ISF data"));


        for (int i = 0; i < xform->Size; i++)
        {
            if (useDoubles)
            {
                bw >> xform->Transform[i];
            }
            else
            {
                float f;
                bw >> f;
                xform->Transform[i] = (double)f;
            }
        }
    }

    return cbRead;
//#pragma warning restore 6518
//#pragma warning restore 1634, 1691
}

/// <summary>
/// Decodes Ink Space Rectangle information from the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="cbSize"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::DecodeInkSpaceRectangle(QIODevice& strm, quint32 cbSize)
{
    quint32 cb, cbRead = 0;
    quint32 cbTotal = cbSize;
    int data;

    //Left
    cb = SerializationHelper::SignDecode(strm, data);
    if (cb > cbTotal)
        throw std::exception(("Invalid ISF data"));

    cbTotal -= cb;
    cbRead += cb;
    _inkSpaceRectangle.setX(data);
    if (cbRead > cbSize)
        throw std::exception(("Invalid ISF data"));

    //Top
    cb = SerializationHelper::SignDecode(strm, data);
    if (cb > cbTotal)
        throw std::exception(("Invalid ISF data"));

    cbTotal -= cb;
    cbRead += cb;
    _inkSpaceRectangle.setY(data);
    if (cbRead > cbSize)
        throw std::exception(("Invalid ISF data"));

    //Right
    cb = SerializationHelper::SignDecode(strm, data);
    if (cb > cbTotal)
        throw std::exception(("Invalid ISF data"));

    cbTotal -= cb;
    cbRead += cb;
    _inkSpaceRectangle.setWidth(data - _inkSpaceRectangle.left());
    if (cbRead > cbSize)
        throw std::exception(("Invalid ISF data"));

    //Bottom
    cb = SerializationHelper::SignDecode(strm, data);
    if (cb > cbTotal)
        throw std::exception(("Invalid ISF data"));

    cbTotal -= cb;
    cbRead += cb;
    _inkSpaceRectangle.setHeight(data - _inkSpaceRectangle.top());
    if (cbRead > cbSize)
        throw std::exception(("Invalid ISF data"));

    return cbRead;
}


/// <summary>
/// Creates a Matrix Information structure based on the transform descriptor
/// </summary>
/// <param name="tdrd"></param>
/// <returns></returns>
QMatrix StrokeCollectionSerializer::LoadTransform(TransformDescriptor* tdrd)
{
    double M00 = 0.0f, M01 = 0.0f, M10 = 0.0f, M11 = 0.0f, M20 = 0.0f, M21 = 0.0f;

    if (KnownTagCache::KnownTagIndex::TransformIsotropicScale == tdrd->Tag)
    {
        M00 = M11 = tdrd->Transform[0];
    }
    else if (KnownTagCache::KnownTagIndex::TransformRotate == tdrd->Tag)
    {
        double dAngle = (tdrd->Transform[0] / 100) * (M_PI / 180);

        M00 = M11 = cos(dAngle);
        M01 = sin(dAngle);
        if (M01 == 0.0f && M11 == 1.0f)
        {
            //special case for 0 degree rotate transforms
            //this is identity
            M10 = 0.0f;
        }
        else
        {
            M10 = -M11;
        }
    }
    else if (KnownTagCache::KnownTagIndex::TransformAnisotropicScale == tdrd->Tag)
    {
        M00 = tdrd->Transform[0];
        M11 = tdrd->Transform[1];
    }
    else if (KnownTagCache::KnownTagIndex::TransformTranslate == tdrd->Tag)
    {
        M20 = tdrd->Transform[0];
        M21 = tdrd->Transform[1];
    }
    else if (KnownTagCache::KnownTagIndex::TransformScaleAndTranslate == tdrd->Tag)
    {
        M00 = tdrd->Transform[0];
        M11 = tdrd->Transform[1];
        M20 = tdrd->Transform[2];
        M21 = tdrd->Transform[3];
    }
    else    // TAG_TRANSFORM
    {
        M00 = tdrd->Transform[0];
        M01 = tdrd->Transform[1];
        M10 = tdrd->Transform[2];
        M11 = tdrd->Transform[3];
        M20 = tdrd->Transform[4];
        M21 = tdrd->Transform[5];
    }

    return QMatrix(M00, M01, M10, M11, M20, M21);
}


/// <summary>
/// Sets the Property Metrics for a property based on Tag and metric descriptor block
/// </summary>
/// <param name="guid"></param>
/// <param name="tag"></param>
/// <param name="block"></param>
/// <returns></returns>
StylusPointPropertyInfo StrokeCollectionSerializer::GetStylusPointPropertyInfo(QUuid const & guid, KnownTagCache::KnownTagIndex tag, MetricBlock* block)
{
    int dw = 0;
    bool fSetDefault = false;
    quint32 cbEntry;
    // StylusPointPropertyInfo values that we need to read in.
    int minimum = 0;
    int maximum = 0;
    StylusPointPropertyUnit unit = StylusPointPropertyUnit::None;
    float resolution = 1.0f;

    // To begin with initialize the property metrics with respective default valuses
    // first check if this property belongs to optional list
    for (dw = 0; dw < 11; dw++)
    {
        if (MetricEntry::MetricEntry_Optional[dw].Tag == tag)
        {
            minimum = MetricEntry::MetricEntry_Optional[dw].PropertyMetrics.Minimum();
            maximum = MetricEntry::MetricEntry_Optional[dw].PropertyMetrics.Maximum();
            resolution = MetricEntry::MetricEntry_Optional[dw].PropertyMetrics.Resolution();
            unit = MetricEntry::MetricEntry_Optional[dw].PropertyMetrics.Unit();
            fSetDefault = true;
            break;
        }
    }

    if (false == fSetDefault)
    {
        // We will come here if the property is not found in the Optional List
        // All other cases, we will have only default values
        minimum = INT_MIN;
        maximum = INT_MAX;
        unit = StylusPointPropertyUnit::None;
        resolution = 1.0f;
        fSetDefault = true;
    }

    // Now see if there is a valid MetricBlock. If there is one, update the PROPERTY_METRICS with
    // values from this Block
    if (nullptr != block)
    {
        MetricEntry* entry = block->GetMetricEntryList();

        while (nullptr != entry)
        {
            if (entry->Tag() == tag)
            {
                cbEntry = 0;

                int range;

                //using (MemoryStream strm = new MemoryStream(entry.Data))
                QByteArray data((char *)entry->Data(), entry->Size());
                QBuffer strm(&data);
                {
                    // Decoded the Logical Min
                    cbEntry += SerializationHelper::SignDecode(strm, range);
                    if (cbEntry >= entry->Size())
                    {
                        break; // return false;
                    }

                    minimum = range;

                    // Logical Max
                    cbEntry += SerializationHelper::SignDecode(strm, range);
                    if (cbEntry >= entry->Size())
                    {
                        break; // return false;
                    }

                    maximum = range;

                    quint32 cb;

                    // Units
                    cbEntry += SerializationHelper::Decode(strm, cb);
                    unit = (StylusPointPropertyUnit)cb;
                    if (cbEntry >= entry->Size())
                    {
                        break; // return false;
                    }

                    //using (BinaryReader br = new BinaryReader(strm))
                    QDataStream br(&strm);
                    {
                        br >>resolution;
                        cbEntry += 4;
                    }
                }

                break;
            }

            entry = entry->Next();
        }
    }

    // return a new StylusPointPropertyInfo
    return StylusPointPropertyInfo( StylusPointProperty(guid, StylusPointPropertyIds::IsKnownButton(guid)),
                                        minimum,
                                        maximum,
                                        unit,
                                        resolution);
}


/// <summary>
/// Builds StylusPointDescription based on StrokeDescriptor and Metric Descriptor Block. Sometime Metric Descriptor block may contain
/// metric information for properties which are not part of the stroke descriptor. They are simply ignored.
/// </summary>
/// <param name="strd"></param>
/// <param name="block"></param>
/// <param name="guidList"></param>
/// <returns></returns>
QSharedPointer<StylusPointDescription> StrokeCollectionSerializer::BuildStylusPointDescription(StrokeDescriptor* strd, MetricBlock* block, GuidList& guidList)
{
    int cTags = 0;
    int packetPropertyCount = 0;
    quint32 buttonCount = 0;
    QVector<QUuid> buttonguids;
    QList<KnownTagCache::KnownTagIndex> tags;

    // if strd is null, it means there is only default descriptor with X & Y
    if (nullptr != strd)
    {
        //tags = new QList<KnownTagCache::KnownTagIndex>();
        while (cTags < strd->Template.size())
        {
            KnownTagCache::KnownTagIndex tag = (KnownTagCache::KnownTagIndex)strd->Template[cTags];

            if (KnownTagCache::KnownTagIndex::Buttons == tag)
            {
                cTags++;

                // The next item in the array is no of buttongs.
                buttonCount = (uint)strd->Template[cTags];
                cTags++;

                // Currently we skip the the no of buttons as buttons is not implimented yet
                buttonguids.resize(buttonCount);
                for (quint32 u = 0; u < buttonCount; u++)
                {
                    QUuid const & guid = guidList.FindGuid(strd->Template[cTags]);
                    if (guid == GuidList::Empty)
                    {
                        throw std::exception(("Button guid tag embedded in ISF stream does not match guid table"));
                    }

                    buttonguids[(int)u] = guid;
                    cTags++;
                }
            }
            else if (KnownTagCache::KnownTagIndex::StrokePropertyList == tag)
            {
                break; // since no more Packet properties can be stored
            }
            else
            {
                if (KnownTagCache::KnownTagIndex::NoX == tag ||
                    KnownTagCache::KnownTagIndex::NoY == tag)
                {
                    throw std::exception(("Invalid ISF with NoX or NoY specified"));
                }

                tags.append(strd->Template[cTags]);
                packetPropertyCount++;
                cTags++;
            }
        }
    }


    QVector<StylusPointPropertyInfo> stylusPointPropertyInfos;
    stylusPointPropertyInfos.append(GetStylusPointPropertyInfo(KnownIds::X, KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::X, block));
    stylusPointPropertyInfos.append(GetStylusPointPropertyInfo(KnownIds::Y, KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::Y, block));
    stylusPointPropertyInfos.append(GetStylusPointPropertyInfo(KnownIds::NormalPressure, KnownIdCache::KnownGuidBaseIndex + KnownIdCache::OriginalISFIdIndex::NormalPressure, block));

    int pressureIndex = -1;
    //if (tags != null)
    if (strd != nullptr)
    {
        for (int i = 0; i < tags.size(); i++)
        {
            QUuid const & guid = guidList.FindGuid(tags[i]);
            if (guid == GuidList::Empty)
            {
                throw std::exception(("Packet Description Property tag embedded in ISF stream does not match guid table"));
            }
            if (pressureIndex == -1 && guid == StylusPointPropertyIds::NormalPressure)
            {
                pressureIndex = i + 2; //x,y have already been accounted for
                continue; //we've already added pressure (above)
            }

            stylusPointPropertyInfos.append(GetStylusPointPropertyInfo(guid, tags[i], block));
        }

        //if (null != buttonguids)
        {
            //
            // add the buttons to the end of the description if they exist
            //
            for (int i = 0; i < buttonguids.size(); i++)
            {
                StylusPointProperty buttonProperty(buttonguids[i], true);
                StylusPointPropertyInfo buttonInfo(buttonProperty);
                stylusPointPropertyInfos.append(buttonInfo);
            }
        }
    }

    return QSharedPointer<StylusPointDescription>(new StylusPointDescription(stylusPointPropertyInfos, pressureIndex));
}
//#endregion

//#endregion // Decoding

//#region Encoding

//#region Public Methods
/// <summary>
/// This functions Saves the Ink as Ink Serialized Format based on the Compression code
/// </summary>
/// <returns>A quint8[] with the encoded ISF</returns>
void StrokeCollectionSerializer::EncodeISF(QIODevice& outputStream)
{
    _strokeLookupTable.clear();
    //    new QDictionary<Stroke, StrokeLookupEntry>(_coreStrokes.Count);

    // Next go through all the strokes
    for (int i = 0; i < _coreStrokes.size(); i++)
    {
        _strokeLookupTable.insert(_coreStrokes[i], new StrokeLookupEntry());
    }

    // Initialize all Arraylists
    //_strokeDescriptorTable = new List<StrokeDescriptor>(_coreStrokes.Count);
    _strokeDescriptorTable.reserve(_coreStrokes.size());
    //_drawingAttributesTable = new List<DrawingAttributes>();
    //_metricTable = new List<MetricBlock>();
    //_transformTable = new List<TransformDescriptor>();

    //using (MemoryStream localStream = new MemoryStream(_coreStrokes.Count * 125)) //reasonable default
    QBuffer localStream;
    localStream.open(QIODevice::ReadWrite);
    {
        GuidList guidList = BuildGuidList();
        quint32 cumulativeEncodedSize = 0;
        quint32 localEncodedSize = 0;

        quint8 xpData = (CurrentCompressionMode == CompressionMode::NoCompression) ? AlgoModule::NoCompression : AlgoModule::DefaultCompression;
        for (QSharedPointer<Stroke> s : _coreStrokes)
        {
            _strokeLookupTable[s]->CompressionData = xpData;

            //
            // we need to get this data up front so that we can
            // know if pressure was used (and thus if we need to add Pressure
            // to the ISF packet description
            //
            QVector<QVector<int>> isfReadyData;
            bool shouldStorePressure;
            s->StylusPoints()->ToISFReadyArrays(isfReadyData, shouldStorePressure);
            _strokeLookupTable[s]->ISFReadyStrokeData = isfReadyData;
            //
            // this is our flag that ToISFReadyArrays sets if pressure was all default
            //
            _strokeLookupTable[s]->StorePressure = shouldStorePressure;
        }


        // Store Ink space rectangle information if necessary and anything other than default
        if (_inkSpaceRectangle != QRectF())
        {
            localEncodedSize = cumulativeEncodedSize;

            QRectF inkSpaceRectangle = _inkSpaceRectangle;
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::InkSpaceRectangle);

            int i = (int)inkSpaceRectangle.left();

            cumulativeEncodedSize += SerializationHelper::SignEncode(localStream, i);
            i = (int)inkSpaceRectangle.top();
            cumulativeEncodedSize += SerializationHelper::SignEncode(localStream, i);
            i = (int)inkSpaceRectangle.right();
            cumulativeEncodedSize += SerializationHelper::SignEncode(localStream, i);
            i = (int)inkSpaceRectangle.bottom();
            cumulativeEncodedSize += SerializationHelper::SignEncode(localStream, i);

            // validate that the expected inkspace rectangle block in ISF was the actual size encoded
            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("Encoded InkSpaceRectangle: size=" + localEncodedSize);

            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // First prepare the compressor. Currently Compression is not supported.
        // Next write the persistence format information if anything other than ISF
        // Currently only ISF is implemented
        if (PersistenceFormat::InkSerializedFormat != CurrentPersistenceFormat)
        {
            localEncodedSize = cumulativeEncodedSize;

            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::PersistenceFormat);
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)SerializationHelper::VarSize((uint)CurrentPersistenceFormat));
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)CurrentPersistenceFormat);

            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("Encoded PersistenceFormat: size=" + localEncodedSize);

            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // Store any size information if necessary such as GIF image size
        // NTRAID#T2-00000-2004/03/15-Microsoft: WORK: Not Yet Implemented

        // Now store the Custom Guids
        localEncodedSize = cumulativeEncodedSize;
        cumulativeEncodedSize += guidList.Save(localStream);
        localEncodedSize = cumulativeEncodedSize - localEncodedSize;
        if (localEncodedSize != 0)
            ISFDebugTrace("Encoded Custom Guid Table: size=" + localEncodedSize);

        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));

        // Now build the tables
        BuildTables(guidList);

        // first write the drawing attributes
        localEncodedSize = cumulativeEncodedSize;
        cumulativeEncodedSize += SerializeDrawingAttrsTable(localStream, guidList);
        localEncodedSize = cumulativeEncodedSize - localEncodedSize;
        if (localEncodedSize != 0)
            ISFDebugTrace("Encoded DrawingAttributesTable: size=" + localEncodedSize);
        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));

        // Next write the stroke descriptor table
        localEncodedSize = cumulativeEncodedSize;
        cumulativeEncodedSize += SerializePacketDescrTable(localStream);
        localEncodedSize = cumulativeEncodedSize - localEncodedSize;
        if (localEncodedSize != 0)
            ISFDebugTrace("Encoded Packet Description: size=" + localEncodedSize);
        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));

        // Write the metric table
        localEncodedSize = cumulativeEncodedSize;
        cumulativeEncodedSize += SerializeMetricTable(localStream);
        localEncodedSize = cumulativeEncodedSize - localEncodedSize;
        if (localEncodedSize != 0)
            ISFDebugTrace("Encoded Metric Table: size=" + localEncodedSize);
        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));

        // Write the transform table
        localEncodedSize = cumulativeEncodedSize;
        cumulativeEncodedSize += SerializeTransformTable(localStream);
        localEncodedSize = cumulativeEncodedSize - localEncodedSize;
        if (localEncodedSize != 0)
            ISFDebugTrace("Encoded Transform Table: size=" + localEncodedSize);
        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));

        // Save global ink properties
        if (_coreStrokes.ExtendedProperties().Count() > 0)
        {
            localEncodedSize = cumulativeEncodedSize;
            cumulativeEncodedSize += ExtendedPropertySerializer::EncodeAsISF(_coreStrokes.ExtendedProperties(), localStream, guidList, GetCompressionAlgorithm(), true);
            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("Encoded Global Ink Attributes Table: size=" + localEncodedSize);
            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // Save stroke ids
        localEncodedSize = cumulativeEncodedSize;
        cumulativeEncodedSize += SaveStrokeIds(_coreStrokes, localStream, false);
        localEncodedSize = cumulativeEncodedSize - localEncodedSize;
        if (localEncodedSize != 0)
            ISFDebugTrace("Encoded Stroke Id List: size=" + localEncodedSize);
        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));

        StoreStrokeData(localStream, guidList, cumulativeEncodedSize, localEncodedSize);

        ISFDebugTrace("Embedded ISF Stream size=" + cumulativeEncodedSize);

        // Now that all data has been written we need to prepend the stream
        long preEncodingPosition = outputStream.pos();
        quint32 cbFinal = SerializationHelper::Encode(outputStream, (uint)0x00);

        cbFinal += SerializationHelper::Encode(outputStream, cumulativeEncodedSize);

        //we have to use localStream to encode ISF because we have to place a variable quint8 'size of isf' at the
        //beginning of the stream
        outputStream.write(localStream.data().data(), (int)cumulativeEncodedSize);
        cbFinal += cumulativeEncodedSize;

        ISFDebugTrace("Final ISF Stream size=" + cbFinal);

        if (cbFinal != outputStream.pos() - preEncodingPosition)
        {
            throw std::exception(("Calculated ISF stream size != actual stream size"));
        }
    }
}

#if OLD_ISF
/// <Summary>
/// Encodes all of the strokes in a strokecollection to ISF
/// </Summary>
/// <SecurityNote>
///     Critical - Calls the critical method StrokeSerializer::EncodeStroke
///
///     This directly called by EncodeISF
///
///     TreatAsSafe boundary is EncodeISF
///
/// </SecurityNote>
[SecurityCritical]
#else
/// <Summary>
/// Encodes all of the strokes in a strokecollection to ISF
/// </Summary>
#endif
void StrokeCollectionSerializer::StoreStrokeData(QIODevice& localStream, GuidList& guidList, quint32& cumulativeEncodedSize, quint32& localEncodedSize)
{
    // Now we will save the stroke data
    quint32 currentDrawingAttributesTableIndex = 0;
    quint32 currentStrokeDescriptorTableIndex = 0;
    quint32 uCurrMetricDescriptorTableIndex = 0;
    quint32 currentTransformTableIndex = 0;

    QVector<int> strokeIds = StrokeIdGenerator::GetStrokeIds(_coreStrokes);
    for (int i = 0; i < _coreStrokes.size(); i++)
    {
        QSharedPointer<Stroke> s = _coreStrokes[i];
        quint32 cbStroke = 0;

        ISFDebugTrace("Encoding Stroke Id#");

        // if the drawing attribute index is different from the current one, write it
        if (currentDrawingAttributesTableIndex != _strokeLookupTable[s]->DrawingAttributesTableIndex)
        {
            localEncodedSize = cumulativeEncodedSize;
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::DrawingAttributesTableIndex);
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, _strokeLookupTable[s]->DrawingAttributesTableIndex);
            currentDrawingAttributesTableIndex = _strokeLookupTable[s]->DrawingAttributesTableIndex;
            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("    Encoded DrawingAttribute Table Index: size=" + localEncodedSize);
            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // if the stroke descriptor index is different from the current one, write it
        if (currentStrokeDescriptorTableIndex != _strokeLookupTable[s]->StrokeDescriptorTableIndex)
        {
            localEncodedSize = cumulativeEncodedSize;
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::StrokeDescriptorTableIndex);
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, _strokeLookupTable[s]->StrokeDescriptorTableIndex);
            currentStrokeDescriptorTableIndex = _strokeLookupTable[s]->StrokeDescriptorTableIndex;
            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("    Encoded Stroke Descriptor Index: size=" + localEncodedSize);
            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // if the metric table index is different from the current one, write it
        if (uCurrMetricDescriptorTableIndex != _strokeLookupTable[s]->MetricDescriptorTableIndex)
        {
            localEncodedSize = cumulativeEncodedSize;
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::MetricTableIndex);
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, _strokeLookupTable[s]->MetricDescriptorTableIndex);
            uCurrMetricDescriptorTableIndex = _strokeLookupTable[s]->MetricDescriptorTableIndex;
            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("    Encoded Metric Index: size=" + localEncodedSize);
            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // if the Transform index is different from the current one, write it
        if (currentTransformTableIndex != _strokeLookupTable[s]->TransformTableIndex)
        {
            localEncodedSize = cumulativeEncodedSize;
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::TransformTableIndex);
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, _strokeLookupTable[s]->TransformTableIndex);
            currentTransformTableIndex = _strokeLookupTable[s]->TransformTableIndex;
            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("    Encoded Transform Index: size=" + localEncodedSize);
            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }

        // now create a separate Memory Stream object which will be used for storing the saved stroke data temporarily
        //using (MemoryStream tempstrm = new MemoryStream(s.StylusPoints.Count * 5)) //good approximation based on profiling isf files
        QBuffer tempstrm;
        tempstrm.open(QIODevice::ReadWrite);
        {
            localEncodedSize = cumulativeEncodedSize;
#if OLD_ISF
            // Now save the stroke in the temp stream
            cbStroke = StrokeSerializer::EncodeStroke(s, tempstrm, null/*we never use CompressionMode::Max)*/, GetCompressionAlgorithm(), guidList, _strokeLookupTable[s]);
#else
            cbStroke = StrokeSerializer::EncodeStroke(*s, tempstrm, GetCompressionAlgorithm(), guidList, *_strokeLookupTable[s]);
#endif

            if (cbStroke != tempstrm.size())
            {
                throw std::exception(("Encoded stroke size != reported size"));
            }

            // Now write the tag KnownTagCache::KnownTagIndex::Stroke
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, (uint)KnownTagCache::KnownTagIndex::Stroke);
            ISFDebugTrace("Stroke size=");

            // Now write the size of the stroke
            cumulativeEncodedSize += SerializationHelper::Encode(localStream, cbStroke);

            // Finally write the stroke data
            localStream.write(tempstrm.data().data(), (int)cbStroke);
            cumulativeEncodedSize += cbStroke;

            localEncodedSize = cumulativeEncodedSize - localEncodedSize;
            if (localEncodedSize != 0)
                ISFDebugTrace("Encoding Stroke Id#");
            if (cumulativeEncodedSize != localStream.size())
                throw std::exception(("Calculated ISF stream size != actual stream size"));
        }
        if (cumulativeEncodedSize != localStream.size())
            throw std::exception(("Calculated ISF stream size != actual stream size"));
    }
}

// <summary>
/// Saves the stroke Ids in the stream.
/// </summary>
/// <param name="strokes"></param>
/// <param name="strm"></param>
/// <param name="forceSave">save ids even if they are contiguous</param>
quint32 StrokeCollectionSerializer::SaveStrokeIds(StrokeCollection& strokes, QIODevice& strm, bool forceSave)
{
    if (0 == strokes.size())
        return 0;

    // Define an ArrayList to store the stroke ids
    QVector<int> strkIds = StrokeIdGenerator::GetStrokeIds(strokes);

    // First enumerate all strokes to collect the ids and also check if the follow the default sequence.
    // If they do we don't save the stroke ids
    bool fDefIds = true;

    if (!forceSave)
    {
        // since the stroke allocation algorithm is i++, we check if any
        //  values are not equal to the sequential and consecutive list
        for (int i = 0; i < strkIds.size(); i++)
        {
            if (strkIds[i] != (i + 1))
            {
                    // if non-sequential or non-consecutive, then persist the ids
                fDefIds = false;
                break;
            }
        }
        // no need to store them if all of them follow the default sequence
        if (fDefIds) return 0;
    }

    // The format is as follows
    // <Tag.StrokeIds> <Encoded Size of Stroke Id data> <StrokeId Count> <Huff compressed array of longs>
    // Encode size of stroke count
    // First write the KnownTagCache::KnownTagIndex::StrokeIds
    quint32 cbWrote = SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::StrokeIds);

    ISFDebugTrace("Saved KnownTagCache::KnownTagIndex::StrokeIds size=");

    // First findout the no of bytes required to huffman compress these ids
    quint8 algorithm = AlgoModule::DefaultCompression;
#if OLD_ISF
    QByteArray data = Compressor::CompressPacketData(null, strkIds, ref algorithm);
#else
    QByteArray data = Compressor::CompressPacketData(strkIds, algorithm);
#endif


    //if (data != nullptr)
    //{
        // First write the encoded size of the buffer
    //    cbWrote += SerializationHelper::Encode(strm, (uint)(data.size() + SerializationHelper::VarSize((uint)strokes.Count)));

        // Write the count of ids
    //    cbWrote += SerializationHelper::Encode(strm, (uint)strokes.size());
    //    strm.write(data, data.size());
    //    cbWrote += (uint)data.size();
    //}
    // If compression fails for some reason, write the uncompressed data
    //else
    {
        quint8 bCompAlgo = 0;//AlgoModule.NoCompression;

        // Find the size of the data + size of the id count
        quint32 cbStrokeId = (uint)(strokes.size() * 4 + 1 + SerializationHelper::VarSize((uint)strokes.size())); // 1 is for the compression header

        cbWrote += SerializationHelper::Encode(strm, cbStrokeId);
        cbWrote += SerializationHelper::Encode(strm, (uint)strokes.size());
        strm.putChar(bCompAlgo);
        cbWrote++;

        // Now write all the ids in the stream
        // samgeo - Presharp issue
        // Presharp gives a warning when local IDisposable variables are not closed
        // in this case, we can't call Dispose since it will also close the underlying stream
        // which still needs to be written to
//#pragma warning disable 1634, 1691
//#pragma warning disable 6518
        QDataStream bw(&strm);

        for (int i = 0; i < strkIds.size(); i++)
        {
            bw << (strkIds[i]);
            cbWrote += 4;
        }
//#pragma warning restore 6518
//#pragma warning restore 1634, 1691
    }

    return cbWrote;
}


//#endregion

//#region Methods

/// <summary>
/// Simple helper method to examine the first 7 members (if they exist)
/// of the quint8[] and see if they have the ascii characters 'base64:' in them.
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
bool StrokeCollectionSerializer::IsBase64Data(QIODevice& data)
{
    //Debug::Assert(data != nullptr);
    long currentPosition = data.pos();
    //try
    {
        FinallyHelper final([currentPosition, &data](){
            data.seek(currentPosition);
        });
        QByteArray isfBase64PrefixBytes((char *)Base64HeaderBytes, sizeof(Base64HeaderBytes));
        if (data.bytesAvailable() < isfBase64PrefixBytes.size())
        {
            return false;
        }

        for (int x = 0; x < isfBase64PrefixBytes.size(); x++)
        {
            if ((quint8)ReadByte(data) != isfBase64PrefixBytes[x])
            {
                return false;
            }
        }
        return true;
    }
    //finally
    //{
        //reset position
    //    data.Position = currentPosition;
    //}
}

/// <summary>
/// Builds the GuidList& based on ExtendedPropeties and StrokeCollection
/// </summary>
/// <returns></returns>
GuidList StrokeCollectionSerializer::BuildGuidList()
{
    GuidList guidList;
    //int i = 0;

    // First go through the list of ink properties
    auto& attributes = _coreStrokes.ExtendedProperties();
    //for (i = 0; i < attributes.size(); i++)
    for (QUuid const & id : attributes.GetGuidArray())
    {
        guidList.Add(id);
    }

    // Next go through all the strokes
    for (int j = 0; j < _coreStrokes.size(); j++)
    {
        BuildStrokeGuidList(_coreStrokes[j], guidList);
    }

    return guidList;
}
/// <summary>
/// Builds the list of Custom Guids that were used by this particular stroke, either in the packet layout
/// or in the drawing attributes, or in the buttons or in Extended properties or in the point properties
/// and updates the GuidList& with that information
/// </summary>
/// <param name="stroke"></param>
/// <param name="guidList"></param>
void StrokeCollectionSerializer::BuildStrokeGuidList(QSharedPointer<Stroke> stroke, GuidList& guidList)
{
    int i = 0;

    // First drawing attributes
    //      Ignore the default Guids/attributes in the DrawingAttributes
    int count;
    QVector<QUuid> guids = ExtendedPropertySerializer::GetUnknownGuids(stroke->GetDrawingAttributes()->ExtendedProperties(), count);

    for (i = 0; i < count; i++)
    {
        guidList.Add(guids[i]);
    }

    QVector<QUuid> descriptionGuids = stroke->StylusPoints()->Description()->GetStylusPointPropertyIds();
    for (i = 0; i < descriptionGuids.size(); i++)
    {
        guidList.Add(descriptionGuids[i]);
    }

    if (stroke->ExtendedProperties().Count() > 0)
    {
        // Add the ExtendedProperty guids in the list
        //for (i = 0; i < stroke->ExtendedProperties().size(); i++)
        for (QUuid const & id : stroke->GetPropertyDataIds())
        {
            guidList.Add(id);
        }
    }
}


quint8 StrokeCollectionSerializer::GetCompressionAlgorithm()
{
    //if (CompressionMode::Compressed == CurrentCompressionMode)
    //{
    //    return AlgoModule.DefaultCompression;
    //}
    return 0;//AlgoModule.NoCompression;
}


/// <summary>
/// This function serializes Stroke Descriptor Table in the stream. For information on how they are serialized, please refer to the spec.
///
/// </summary>
/// <param name="strm"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::SerializePacketDescrTable(QIODevice& strm)
{
    if (_strokeDescriptorTable.size() == 0)
        return 0;

    int count = 0;
    quint32 cbData = 0;

    // First add the appropriate header information
    if (_strokeDescriptorTable.size() == 1)
    {
        StrokeDescriptor& tmp = *_strokeDescriptorTable[0];

        // If there is no tag, that means default template and only one entry in the list. Return from here
        if (tmp.Template.size() == 0)
            return 0;
        else
        {
            // Write it directly
            // First the tag
            cbData += SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::StrokeDescriptorBlock);

            // Now encode the descriptor itself
            cbData += EncodeStrokeDescriptor(strm, tmp);
        }
    }
    else
    {
        quint32 cbTotal = 0;

        // First calculate the total encoded size of the all the Templates
        for (count = 0; count < _strokeDescriptorTable.size(); count++)
        {
            cbTotal += SerializationHelper::VarSize((_strokeDescriptorTable[count])->Size) + (_strokeDescriptorTable[count])->Size;
        }

        // Now write the Tag
        cbData += SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::StrokeDescriptorTable);
        cbData += SerializationHelper::Encode(strm, cbTotal);

        // Now write the encoded templates
        for (count = 0; count < _strokeDescriptorTable.size(); count++)
        {
            cbData += EncodeStrokeDescriptor(strm, *_strokeDescriptorTable[count]);
        }
    }

    return cbData;
}


/// <summary>
/// This function serializes Metric Descriptor Table in the stream. For information on how they are serialized, please refer to the spec.
/// </summary>
/// <param name="strm"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::SerializeMetricTable(QIODevice& strm)
{
    quint32 cSize = 0;
    MetricBlock* block;

    if (0 == _metricTable.size())
        return 0;

    for (int i = 0; i < _metricTable.size(); i++)
        cSize += _metricTable[i]->Size();

    quint32 cbData = 0;

    // if total size of the blocks is 1, then there is nothing to write
    //  the reason that the size of the blocks is 1 instead of 0 is because
    //  MetricBlock.Size returns the size of the block plus the quint8 encoded
    //  size value itself. If the MetricBlock size value is 0, then byte
    //  encoded size value is 0, which has a quint8 size of 1.
    if (1 == cSize)
    {
        return 0;
    }
    else if (1 == _metricTable.size())
    {
        cbData += SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::MetricBlock);
    }
    else
    {
        cbData += SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::MetricTable);
        cbData += SerializationHelper::Encode(strm, cSize);
    }

    for (int i = 0; i < _metricTable.size(); i++)
    {
        block = _metricTable[i];
        cbData += block->Pack(strm);
    }

    return cbData;
}


/// <summary>
/// Multiquint8 Encodes a Stroke Descroptor
/// </summary>
/// <param name="strm"></param>
/// <param name="strd"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::EncodeStrokeDescriptor(QIODevice& strm, StrokeDescriptor& strd)
{
    quint32 cbData = 0;

    // First encode the size of the descriptor
    cbData += SerializationHelper::Encode(strm, strd.Size);
    for (int count = 0; count < strd.Template.size(); count++)
    {
        // Now encode all members of the descriptor
        cbData += SerializationHelper::Encode(strm, (uint)strd.Template[count]);
    }

    return cbData;
}


/// <summary>
/// This function serializes Transform Descriptor Table in the stream. For information on how they are serialized, please refer to the spec.
/// </summary>
/// <param name="strm"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::SerializeTransformTable(QIODevice& strm)
{
    // If there is only one entry in the TransformDescriptor table
    //      and it is the default descriptor, skip serialization of transforms
    if (_transformTable.size() == 1 && _transformTable[0]->Size == 0)
    {
        return 0;
    }

    quint32 floatTotal = 0;
    quint32 doubleTotal = 0;

    // First count the size of all transforms (handling both float && double versions)
    for (int i = 0; i < _transformTable.size(); i++)
    {
        TransformDescriptor* xform = _transformTable[i];
        quint32 cbLocal = SerializationHelper::VarSize((uint)xform->Tag);

        floatTotal += cbLocal;
        doubleTotal += cbLocal;
        if (KnownTagCache::KnownTagIndex::TransformRotate == xform->Tag)
        {
            cbLocal = SerializationHelper::VarSize((uint)(xform->Transform[0] + 0.5f));
            floatTotal += cbLocal;
            doubleTotal += cbLocal;
        }
        else
        {
            cbLocal = xform->Size * 4;
            floatTotal += cbLocal;
            doubleTotal += cbLocal * 2;
        }
    }

    quint32 cbTotal = 0;

    // If there is only one entry in the TransformDescriptor table
    if (_transformTable.size() == 1)
    {
        TransformDescriptor* xform = _transformTable[0];

        cbTotal = EncodeTransformDescriptor(strm, *xform, false);
    }
    else
    {
        // Now first write the block descriptor and then write all transforms
        cbTotal += SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::TransformTable);
        cbTotal += SerializationHelper::Encode(strm, floatTotal);
        for (int i = 0; i < _transformTable.size(); i++)
        {
            cbTotal += EncodeTransformDescriptor(strm, *_transformTable[i], false);
        }
    }
    // now write the Extended Transform table (using doubles instead of floats)
    { // note that we do not distinguish between 1 and > 1 transforms for compression
        // Now first write the block descriptor and then write all transforms
        cbTotal += SerializationHelper::Encode(strm, (uint)KnownTagCache::KnownTagIndex::ExtendedTransformTable);
        cbTotal += SerializationHelper::Encode(strm, doubleTotal);
        for (int i = 0; i < _transformTable.size(); i++)
        {
            cbTotal += EncodeTransformDescriptor(strm, *_transformTable[i], true);
        }
    }
    return cbTotal;
}


/// <summary>
/// Multiquint8 Encode if necessary a Transform Descriptor into the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="xform"></param>
/// <param name="useDoubles"></param>
/// <returns></returns>
quint32 StrokeCollectionSerializer::EncodeTransformDescriptor(QIODevice& strm, TransformDescriptor& xform, bool useDoubles)
{
    quint32 cbData = 0;

    // First encode the tag
    cbData = SerializationHelper::Encode(strm, (uint)xform.Tag);

    // Encode specially if transform denotes rotation
    if (KnownTagCache::KnownTagIndex::TransformRotate == xform.Tag)
    {
        quint32 angle = (uint)(xform.Transform[0] + 0.5f);

        cbData += SerializationHelper::Encode(strm, angle);
    }
    else
    {
        // samgeo - Presharp issue
        // Presharp gives a warning when local IDisposable variables are not closed
        // in this case, we can't call Dispose since it will also close the underlying stream
        // which still needs to be written to
//#pragma warning disable 1634, 1691
//#pragma warning disable 6518
        QDataStream bw(&strm);
        bw.setVersion(QDataStream::Qt_4_0);

        for (int i = 0; i < xform.Size; i++)
        {
            // note that the binary writer changes serialization
            //      lengths depending on the Write parameter cast
            if (useDoubles)
            {
                bw << (xform.Transform[i]);
                cbData += 8;
            }
            else
            {
                bw << ((float)xform.Transform[i]);
                cbData += 4;
            }
        }
//#pragma warning restore 6518
//#pragma warning restore 1634, 1691
    }

    return cbData;
}

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
[SecurityCritical]
#else
/// <summary>
/// This function serializes Drawing Attributes Table in the stream. For information on how they are serialized, please refer to the spec.
/// </summary>
/// <param name="stream"></param>
/// <param name="guidList"></param>
#endif
quint32 StrokeCollectionSerializer::SerializeDrawingAttrsTable(QIODevice& stream, GuidList& guidList)
{
    quint32 totalSizeOfSerializedBytes = 0;
    quint32 sizeOfHeaderInBytes = 0;

    if (1 == _drawingAttributesTable.size())
    {
        //we always serialize a single DA, even if it has default values so we will write width back to the stream
        QSharedPointer<DrawingAttributes> drawingAttributes = _drawingAttributesTable[0];

        // There is single drawing attribute. Save it along with the size
        totalSizeOfSerializedBytes += SerializationHelper::Encode(stream, (uint)KnownTagCache::KnownTagIndex::DrawingAttributesBlock);

        // Get the size of the saved bytes
        //using (MemoryStream drawingAttributeStream = new MemoryStream(16)) //reasonable default based onn profiling
        {
            QBuffer drawingAttributeStream;
            drawingAttributeStream.open(QIODevice::ReadWrite);
            sizeOfHeaderInBytes = DrawingAttributeSerializer::EncodeAsISF(*drawingAttributes, drawingAttributeStream, guidList, 0, true);

            // Write the size first
            totalSizeOfSerializedBytes += SerializationHelper::Encode(stream, sizeOfHeaderInBytes);

            // write the data
            quint32 bytesWritten = drawingAttributeStream.pos();
            totalSizeOfSerializedBytes += bytesWritten;
            Debug::Assert(sizeOfHeaderInBytes == bytesWritten);

            stream.write(   drawingAttributeStream.data().data(),
                            bytesWritten);
        }
    }
    else
    {
        // Temporarily declare an array to hold the size of the saved drawing attributes
        QVector<uint> sizes(_drawingAttributesTable.size());
        QVector<QBuffer*> drawingAttributeStreams(_drawingAttributesTable.size());

        // First calculate the size of each attribute
        for (int i = 0; i < _drawingAttributesTable.size(); i++)
        {
            QSharedPointer<DrawingAttributes> drawingAttributes = _drawingAttributesTable[i];
            drawingAttributeStreams[i]->open(QIODevice::ReadWrite); //reasonable default based on profiling

            sizes[i] = DrawingAttributeSerializer::EncodeAsISF(*drawingAttributes, *drawingAttributeStreams[i], guidList, 0, true);
            sizeOfHeaderInBytes += SerializationHelper::VarSize(sizes[i]) + sizes[i];
        }

        // Now write the KnownTagCache::KnownTagIndex::DrawingAttributesTable first, then sizeOfHeaderInBytes and then individual Drawing Attributes
        totalSizeOfSerializedBytes = SerializationHelper::Encode(stream, (uint)KnownTagCache::KnownTagIndex::DrawingAttributesTable);

        totalSizeOfSerializedBytes += SerializationHelper::Encode(stream, sizeOfHeaderInBytes);
        for (int i = 0; i < _drawingAttributesTable.size(); i++)
        {
            QSharedPointer<DrawingAttributes> drawingAttributes = _drawingAttributesTable[i];

            // write the size of the block
            totalSizeOfSerializedBytes += SerializationHelper::Encode(stream, sizes[i]);

            // write the saved data
            quint32 bytesWritten = drawingAttributeStreams[i]->pos();
            totalSizeOfSerializedBytes += bytesWritten;
            Debug::Assert(sizes[i] == bytesWritten);

            stream.write(   drawingAttributeStreams[i]->data(), //returns a direct ref, no copies
                            bytesWritten);

            drawingAttributeStreams[i]->close();
        }
    }

    return totalSizeOfSerializedBytes;
}

/// <summary>
/// This function builds list of all unique Tables, ie Stroke Descriptor Table, Metric Descriptor Table, Transform Descriptor Table
/// and Drawing Attributes Table based on all the strokes. Each entry in the Table is unique with respect to the table.
/// </summary>
/// <param name="guidList"></param>
void StrokeCollectionSerializer::BuildTables(GuidList& guidList)
{
    _transformTable.clear();
    _strokeDescriptorTable.clear();
    _metricTable.clear();
    _drawingAttributesTable.clear();

    int count = 0;

    for (count = 0; count < _coreStrokes.size(); count++)
    {
        QSharedPointer<Stroke> stroke = _coreStrokes[count];

        // First get the updated descriptor from the stroke
        StrokeDescriptor* strokeDescriptor;
        MetricBlock* metricBlock;
        StrokeSerializer::BuildStrokeDescriptor(*stroke, guidList, *_strokeLookupTable[stroke], strokeDescriptor, metricBlock);
        bool fMatch = false;

        // Compare this with all the global stroke descriptor for a match
        for (int descriptorIndex = 0; descriptorIndex < _strokeDescriptorTable.size(); descriptorIndex++)
        {
            if (strokeDescriptor->IsEqual(*_strokeDescriptorTable[descriptorIndex]))
            {
                fMatch = true;
                _strokeLookupTable[stroke]->StrokeDescriptorTableIndex = (uint)descriptorIndex;
                break;
            }
        }
        if (false == fMatch)
        {
            _strokeDescriptorTable.append(strokeDescriptor);
            _strokeLookupTable[stroke]->StrokeDescriptorTableIndex = (uint)_strokeDescriptorTable.size() - 1;
        }

        // If there is at least one entry in the metric block, check if the current Block is equvalent to
        // any of the existing one.
        fMatch = false;
        for (int tmp = 0; tmp < _metricTable.size(); tmp++)
        {
            MetricBlock& block = *_metricTable[tmp];
            SetType type = SetType::SubSet;

            if (block.CompareMetricBlock(*metricBlock, type))
            {
                // This entry exists in the list. If it is a subset of the element, do nothing.
                // Otherwise, replace the entry with this one
                if (type == SetType::SuperSet)
                {
                    _metricTable[tmp] = metricBlock;
                }

                fMatch = true;
                _strokeLookupTable[stroke]->MetricDescriptorTableIndex = (uint)tmp;
                break;
            }
        }

        if (false == fMatch)
        {
            _metricTable.append(metricBlock);
            _strokeLookupTable[stroke]->MetricDescriptorTableIndex = (uint)(_metricTable.size() - 1);
        }

        // Now build the Transform Table
        fMatch = false;

        //
        // always identity
        //
        TransformDescriptor xform = IdentityTransformDescriptor;

        // First check to see if this matches with any existing Transform Blocks
        for (int i = 0; i < _transformTable.size(); i++)
        {
            if (true == xform.Compare(*_transformTable[i]))
            {
                fMatch = true;
                _strokeLookupTable[stroke]->TransformTableIndex = (uint)i;
                break;
            }
        }

        if (false == fMatch)
        {
            _transformTable.append(&xform);
            _strokeLookupTable[stroke]->TransformTableIndex = (uint)(_transformTable.size() - 1);
        }

        // Now build the drawing attributes table
        fMatch = false;

        QSharedPointer<DrawingAttributes> drattrs = _coreStrokes[count]->GetDrawingAttributes();

        // First check to see if this matches with any existing transform blocks
        for (int i = 0; i < _drawingAttributesTable.size(); i++)
        {
            if (true == drattrs->Equals(*_drawingAttributesTable[i]))
            {
                fMatch = true;
                _strokeLookupTable[stroke]->DrawingAttributesTableIndex = (uint)i;
                break;
            }
        }

        if (false == fMatch)
        {
            _drawingAttributesTable.append(drattrs);
            _strokeLookupTable[stroke]->DrawingAttributesTableIndex = (uint)_drawingAttributesTable.size() - 1;
        }
    }
}


/// <summary>
/// Generates backwards compatible StrokeID's for the strokes
/// </summary>
/// <param name="strokes">strokes</param>
/// <returns></returns>
QVector<int> StrokeIdGenerator::GetStrokeIds(StrokeCollection& strokes)
{
    //Debug::Assert(strokes != null);

    QVector<int> strokeIds(strokes.size());
    for (int x = 0; x < strokeIds.size(); x++)
    {
        //stroke ID's are 1 based (1,2,3...)
        strokeIds[x] = x + 1;
    }
    return strokeIds;
}
