#include "Internal/Ink/InkSerializedFormat/strokeserializer.h"
#include "Internal/Ink/InkSerializedFormat/guidlist.h"
#include "Internal/Ink/InkSerializedFormat/strokedescriptor.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Internal/Ink/InkSerializedFormat/metricblock.h"
#include "Internal/Ink/InkSerializedFormat/compress.h"
#include "Internal/Ink/InkSerializedFormat/algomodule.h"
#include "Internal/debug.h"
#include "Windows/Input/styluspointcollection.h"
#include "Windows/Input/styluspointdescription.h"
#include "Windows/Ink/stroke.h"
#include "Windows/Ink/knownids.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Ink/extendedproperty.h"
#include "Windows/Input/styluspointproperties.h"

#include <QIODevice>
#include <QDebug>

INKCANVAS_BEGIN_NAMESPACE

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
uint StrokeSerializer::DecodeStroke(QIODevice& stream,
                         uint size,
                         GuidList& guidList,
                         StrokeDescriptor& strokeDescriptor,
                         SharedPointer<StylusPointDescription> stylusPointDescription,
                         SharedPointer<DrawingAttributes> drawingAttributes,
                         Matrix& transform,
                         SharedPointer<Stroke>& stroke)
{
    ExtendedPropertyCollection* extendedProperties;
    SharedPointer<StylusPointCollection> stylusPoints;

    uint cb = DecodeISFIntoStroke(
        stream,
        size,
        guidList,
        strokeDescriptor,
        stylusPointDescription,
        transform,
        stylusPoints,
        extendedProperties);

    if (cb != size)
    {
        throw std::runtime_error("Stroke size ("") != expected ("")");
    }

    stroke.reset(new Stroke(stylusPoints, drawingAttributes, extendedProperties));

    return cb;
}

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
uint StrokeSerializer::DecodeISFIntoStroke(
    QIODevice& stream,
    uint totalBytesInStrokeBlockOfIsfStream,
    GuidList& guidList,
    StrokeDescriptor& strokeDescriptor,
    SharedPointer<StylusPointDescription> stylusPointDescription,
    Matrix& transform,
    SharedPointer<StylusPointCollection>& stylusPoints,
    ExtendedPropertyCollection*& extendedProperties)
{
    stylusPoints = nullptr;
    extendedProperties = nullptr;

    // We do allow a stroke with no packet data
    if (0 == totalBytesInStrokeBlockOfIsfStream)
    {
        return 0;
    }

    uint locallyDecodedBytes;
    uint remainingBytesInStrokeBlock = totalBytesInStrokeBlockOfIsfStream;

    // First try to load any packet data
    locallyDecodedBytes = LoadPackets(  stream,
                                        remainingBytesInStrokeBlock,
                                        stylusPointDescription,
                                        transform,
                                        stylusPoints);

    if (locallyDecodedBytes > remainingBytesInStrokeBlock)
        throw std::runtime_error(("Packet buffer overflowed the ISF stream"));

    remainingBytesInStrokeBlock -= locallyDecodedBytes;
    if (0 == remainingBytesInStrokeBlock)
    {
        return locallyDecodedBytes;
    }

    // Now read the extended propertes
    for (int iTag = 1; iTag < strokeDescriptor.Template.Count() && remainingBytesInStrokeBlock > 0; iTag++)
    {
        KnownTagCache::KnownTagIndex tag = strokeDescriptor.Template[iTag - 1];

        switch (tag)
        {
            case KnownTagCache::KnownTagIndex::StrokePropertyList:
                {
                    // we've found the stroke extended properties. Load them now.
                    while (iTag < strokeDescriptor.Template.Count() && remainingBytesInStrokeBlock > 0)
                    {
                        tag = strokeDescriptor.Template[iTag];

                        Variant data;
                        Guid guid = guidList.FindGuid(tag);
                        if (guid == GuidList::Empty)
                        {
                            throw std::runtime_error(("Stroke Custom Attribute tag embedded in ISF stream does not match guid table"));
                        }

                        // load the extended property data from the stream (and decode the type)
                        //locallyDecodedBytes = ExtendedPropertySerializer::DecodeAsISF(stream, remainingBytesInStrokeBlock, guidList, tag, ref guid, out data);

                        // add the guid/data pair into the property collection (don't redecode the type)
                        if (extendedProperties == nullptr)
                        {
                            extendedProperties = new ExtendedPropertyCollection;
                        }
                        extendedProperties->Set(guid, data);
                        if (locallyDecodedBytes > remainingBytesInStrokeBlock)
                            throw std::runtime_error(("Invalid ISF data"));

                        remainingBytesInStrokeBlock -= locallyDecodedBytes;
                        iTag++;
                    }
                }
                break;

            case KnownTagCache::KnownTagIndex::Buttons:
                {
                    // Next tag is count of buttons and the tags for the button guids
                    iTag += (int)((uint)strokeDescriptor.Template[iTag]) + 1;
                }
                break;

            // ignore any tags embedded in the Stroke block that this
            //      version of the ISF decoder doesn't understand
            default:
                {
                    qDebug() << ("Ignoring unhandled stroke tag in ISF stroke descriptor");
                }
                break;
        }
    }

    // Now try to load any tagged property data or point property data
    while (remainingBytesInStrokeBlock > 0)
    {
        // Read the tag first
        KnownTagCache::KnownTagIndex tag;
        uint uiTag;

        locallyDecodedBytes = SerializationHelper::Decode(stream, uiTag);
        tag = (KnownTagCache::KnownTagIndex)uiTag;
        if (locallyDecodedBytes > remainingBytesInStrokeBlock)
            throw std::runtime_error(("Invalid ISF data"));

        remainingBytesInStrokeBlock -= locallyDecodedBytes;

        // if it is a point property block
        switch (tag)
        {
            case KnownTagCache::KnownTagIndex::PointProperty:
                {
                    // First load the totalBytesInStrokeBlockOfIsfStream of the point property block
                    uint cbsize;

                    locallyDecodedBytes = SerializationHelper::Decode(stream, cbsize);
                    if (locallyDecodedBytes > remainingBytesInStrokeBlock)
                        throw std::runtime_error(("Invalid ISF data"));

                    remainingBytesInStrokeBlock -= locallyDecodedBytes;
                    while (remainingBytesInStrokeBlock > 0)
                    {
                        // First read the tag corresponding to the property
                        locallyDecodedBytes = SerializationHelper::Decode(stream, uiTag);
                        tag = (KnownTagCache::KnownTagIndex)uiTag;
                        if (locallyDecodedBytes > remainingBytesInStrokeBlock)
                            throw std::runtime_error(("Invalid ISF data"));

                        remainingBytesInStrokeBlock -= locallyDecodedBytes;

                        // Now read the packet index for which the property will apply
                        uint propindex;

                        locallyDecodedBytes = SerializationHelper::Decode(stream, propindex);
                        if (locallyDecodedBytes > remainingBytesInStrokeBlock)
                            throw std::runtime_error(("Invalid ISF data"));

                        remainingBytesInStrokeBlock -= locallyDecodedBytes;

                        uint propsize;

                        locallyDecodedBytes = SerializationHelper::Decode(stream, propsize);
                        if (locallyDecodedBytes > remainingBytesInStrokeBlock)
                            throw std::runtime_error(("Invalid ISF data"));

                        remainingBytesInStrokeBlock -= locallyDecodedBytes;

                        // Compressed data totalBytesInStrokeBlockOfIsfStream
                        propsize += 1;

                        // Make sure we have enough data to read
                        if (propsize > remainingBytesInStrokeBlock)
                            throw std::runtime_error(("Invalid ISF data"));

                        QByteArray in_buffer; in_buffer.resize(propsize);

                        uint bytesRead = StrokeCollectionSerializer::ReliableRead(stream, (quint8*)in_buffer.data(), propsize);
                        if (propsize != bytesRead)
                        {
                            throw std::runtime_error(("Read different size from stream then expected"));
                        }

                        QByteArray out_buffer;// = Compressor.DecompressPropertyData(in_buffer);

                        Debug::Assert(false, "ExtendedProperties for points are not supported");

                        // skip the bytes in both success & failure cases
                        // Note: Point ExtendedProperties are discarded
                        remainingBytesInStrokeBlock -= propsize;
                    }
                }
                break;

            default:
                {
                    Variant data;
                    Guid guid = guidList.FindGuid(tag);
                    if (guid == GuidList::Empty)
                    {
                        throw std::runtime_error(("Stroke Custom Attribute tag embedded in ISF stream does not match guid table"));
                    }

                    // load the extended property data from the stream (and decode the type)
                    //locallyDecodedBytes = ExtendedPropertySerializer::DecodeAsISF(stream, remainingBytesInStrokeBlock, guidList, tag, guid, out data);

                    // add the guid/data pair into the property collection (don't redecode the type)
                    if (extendedProperties == nullptr)
                    {
                        extendedProperties = new ExtendedPropertyCollection();
                    }
                    extendedProperties->Set(guid, data);
                    if (locallyDecodedBytes > remainingBytesInStrokeBlock)
                    {
                        throw std::runtime_error(("ExtendedProperty decoded totalBytesInStrokeBlockOfIsfStream exceeded ISF stream totalBytesInStrokeBlockOfIsfStream"));
                    }

                    remainingBytesInStrokeBlock -= locallyDecodedBytes;
                }
                break;
        }
    }

    if (0 != remainingBytesInStrokeBlock)
        throw std::runtime_error(("Invalid ISF data"));

    return totalBytesInStrokeBlockOfIsfStream;
}

/// <summary>
/// Loads packets from the input stream.  For example, packets are all of the x's in a stroke
/// </summary>
uint StrokeSerializer::LoadPackets(QIODevice& inputStream,
                        uint totalBytesInStrokeBlockOfIsfStream,
                        SharedPointer<StylusPointDescription> stylusPointDescription,
                        Matrix& transform,
                        SharedPointer<StylusPointCollection>& stylusPoints)
{
    stylusPoints = nullptr;

    if (0 == totalBytesInStrokeBlockOfIsfStream)
        return 0;

    uint locallyDecodedBytesRemaining = totalBytesInStrokeBlockOfIsfStream;
    uint localBytesRead;

    // First read the no of packets
    uint pointCount;

    localBytesRead = SerializationHelper::Decode(inputStream, pointCount);
    if (locallyDecodedBytesRemaining < localBytesRead)
        throw std::runtime_error(("Invalid ISF data"));

    locallyDecodedBytesRemaining -= localBytesRead;
    if (0 == locallyDecodedBytesRemaining)
        return localBytesRead;

    // Allocate packet properties
    int intsPerPoint = stylusPointDescription->GetInputArrayLengthPerPoint();
    int buttonCount = stylusPointDescription->ButtonCount();
    int buttonIntsPerPoint = (buttonCount > 0 ? 1 : 0);
    int valueIntsPerPoint = intsPerPoint - buttonIntsPerPoint;
    //add one int per point for button data if it exists
    Array<int> rawPointData(pointCount * intsPerPoint);
    QVector<int> packetDataSet(pointCount);


    // copy the rest of the data from the stroke data
    QByteArray inputBuffer ; inputBuffer.resize(locallyDecodedBytesRemaining);

    // Read the input data into the byte array
    uint bytesRead = StrokeCollectionSerializer::ReliableRead(inputStream, (quint8*)inputBuffer.data(), locallyDecodedBytesRemaining);

    if ( bytesRead != locallyDecodedBytesRemaining )
    {
        // Make sure the bytes read are expected. If not, we should bail out.
        // An exception will be thrown.
        throw std::runtime_error(("Invalid ISF data"));
    }

    // at this point, we have read all of the bytes remaining in the input
    //      stream's packet block, and while we will keep the bytes remaining
    //      variable for positioning within the local byte buffer, we should
    //      not read from the stream again, or we risk reading into another
    //      ISF tag's block.
    int originalPressureIndex = stylusPointDescription->OriginalPressureIndex();
    for (int i = 0; i < valueIntsPerPoint && locallyDecodedBytesRemaining > 0; i++)
    {
        localBytesRead = locallyDecodedBytesRemaining;
        Compressor::DecompressPacketData(
                inputBuffer,
                localBytesRead,
                packetDataSet);

        if (localBytesRead > locallyDecodedBytesRemaining)
            throw std::runtime_error(("Invalid ISF data"));

        //
        // packetDataSet is like this:
        // -------------
        // |X|X|X|X|X|X|
        // -------------
        //
        // we need to copy into rawPointData at
        //
        // -------------
        // |X| |X| |X| |
        // -------------
        //
        // additionally, for NormalPressure, if it exists and was
        // reordered in the StylusPointDescription, we need to account for that here
        //
        int tempi = i;
        if (tempi > 1 &&
            originalPressureIndex != -1 &&
            originalPressureIndex != StylusPointDescription::RequiredPressureIndex/*2*/)
        {
            //
            // NormalPressure exists in the packet stream and was not at index 2
            // StylusPointDescription enforces that NormalPressure is at index 2
            // so we need to copy packet data beyond X and Y into a different location
            //
            // take the example of the original StylusPointDescription
            //  |X|Y|XTilt|YTilt|NormalPressure|Rotation|
            //
            // originalPressureIndex is 4, and we know it is now 2
            // which means that everything before index 4 has been shifted one
            // and everything after index 4 is still good.  Index 4 should be copied to index 2
            if (tempi == originalPressureIndex)
            {
                tempi = 2;
            }
            else if (tempi < originalPressureIndex)
            {
                tempi++;
            }
        }

        locallyDecodedBytesRemaining -= localBytesRead;
        for (int j = 0, x = 0; j < pointCount; j++, x += intsPerPoint)
        {

            rawPointData[x + tempi] = packetDataSet[j];
        }

        // Move the array elements to point to next set of compressed data
        for (uint u = 0; u < locallyDecodedBytesRemaining; u++)
        {
            inputBuffer[u] = inputBuffer[u + (int)localBytesRead];
        }
    }

    // Now that we've read packet data, we must read button data if it is there
    QByteArray buttonData;
    // since the button state is a simple bit value (either down or up), the button state
    // for a series of packets is packed into an array of bits rather than integers
    // For example, if there are 16 packets, and 2 buttons, then 32 bits can be used (e.g. 1 32-bit integer)
    if (0 != locallyDecodedBytesRemaining && buttonCount > 0)
    {
            // calculate the number of full bytes used for buttons per packet
            //   Example: 10 buttons would require 1 full byte
        int fullBytesForButtonsPerPacket = buttonCount / 8;
            // calculate the number of bits that spill beyond the full byte boundary
            //   Example: 10 buttons would require 2 extra bits (8 fit in a full byte)
        int partialBitsForButtonsPerPacket = buttonCount % 8;

        // Now figure out how many bytes we need to read for the button data
        localBytesRead = (uint)((buttonCount * pointCount + 7) / 8);
        if (localBytesRead > locallyDecodedBytesRemaining)
        {
            throw std::runtime_error(("Buffer range is smaller than expected expected size"));
        }
        locallyDecodedBytesRemaining -= localBytesRead;

        int buttonSizeInBytes = (buttonCount + 7)/8;
        buttonData.resize(pointCount * buttonSizeInBytes);

        // Create a bit reader to unpack the bits from the ISF stream into
        //    loosely packed byte buffer (e.g. button data aligned on full byte
        //    boundaries only)
        //BitStreamReader bitReader =
        //    new BitStreamReader(inputBuffer, (uint)buttonCount * pointCount);

        // unpack the button data into each packet
        int byteCounter = 0;
        //while (!bitReader.EndOfStream)
        {
            // unpack the fully bytes first
            for (int fullBytes = 0; fullBytes < fullBytesForButtonsPerPacket; fullBytes++)
            {
                //buttonData[byteCounter++] = bitReader.ReadByte(8);
            }
            // then unpack a single partial byte if necessary
            if (partialBitsForButtonsPerPacket > 0)
            {
                //buttonData[byteCounter++] = bitReader.ReadByte((int)partialBitsForButtonsPerPacket);
            }
        }

        // if the number of bytes allocated != necessary byte amount then an error occurred
        if (byteCounter != buttonData.size())
        {
            throw std::runtime_error(("Button data length not equal to expected length"));
        }

        //
        // set the point data in the raw array
        //
        FillButtonData( (int)pointCount,
                        buttonCount,
                        valueIntsPerPoint, //gives the first button index
                        rawPointData,
                        buttonData);
    }


    stylusPoints.reset(new StylusPointCollection(stylusPointDescription, rawPointData, {}, transform));

    // if we read too far into the stream (e.g. the packets were compressed)
    //      then move the stream pointer back to the end of the actual packet
    //      data before returning. This keeps the return value on the function
    //      (representing bytes read) honest and consistent with the stream
    //      position movement in this function.
    if (0 != locallyDecodedBytesRemaining)
    {
        inputStream.seek(0 - (long)locallyDecodedBytesRemaining + inputStream.pos());
    }

    return totalBytesInStrokeBlockOfIsfStream - locallyDecodedBytesRemaining;
}

/// <summary>
/// Fill packet buffer with button data
/// </summary>
/// <param name="pointCount">how many points in the stroke</param>
/// <param name="buttonCount">how many buttons</param>
/// <param name="buttonIndex">the first index of the buttons in packets</param>
/// <param name="packets">packet data</param>
/// <param name="buttonData">button data to fill</param>
void StrokeSerializer::FillButtonData(
    int pointCount,
    int buttonCount,
    int buttonIndex,
    Array<int> packets,
    QByteArray buttonData)
{
    int intsPerPoint = buttonIndex + 1;
    int iPacket;
    int iDim = buttonIndex; //count of value properties

    // Get button data
    //if (null != buttonData)
    {
        // Count of bytes per button
        int nBtnDim = (buttonCount + 7) / 8;

        // Count of complete integers
        int pointCountOfFullIntegers = (int)(nBtnDim / 4);

        // Remaining number of bytes in button data
        int pointCountOfPartialBytes = (int)(nBtnDim % 4);

        // Index into button byte array
        int iBtnByte = 0;

        // Copy the complete integers
        for (int iInt = 0; iInt < pointCountOfFullIntegers; ++iInt, ++iDim)
        {
            // Index into button byte array
            iBtnByte = (int)(iInt * 4);
            iPacket = iDim;

            // Get the button integers from each packet
            for (int i = 0; i < pointCount; ++i, iPacket += intsPerPoint, iBtnByte += nBtnDim)
            {
                packets[iPacket] = ((int)((uint)((buttonData[iBtnByte] << (8 * 3)) | (buttonData[iBtnByte + 1] << (8 * 2)) | (buttonData[iBtnByte + 2] << 8) | (buttonData[iBtnByte + 3]))));
            }
        }

        // If remaining number of bytes per button is non-negative,
        // construct button data from those bytes
        if (0 < pointCountOfPartialBytes)
        {
            // Index into button byte array
            iBtnByte = (int)(pointCountOfFullIntegers * 4);
            iPacket = iDim;

            // Constructs ints from partial button bytes
            for (int i = 0; i < pointCount; ++i, iPacket += intsPerPoint, iBtnByte += nBtnDim)
            {
                // First byte will always be there
                uint btn = buttonData[iBtnByte];

                // For the remaining bytes, shift left 8 bits
                for (int iPart = 1; iPart < pointCountOfPartialBytes; ++iPart)
                {
                    btn = (btn << 8) | (buttonData[iBtnByte + iPart]);
                }

                // Assign the reconstructed button data to packet data
                packets[iPacket] = (int)btn;
            }
        }
    }
}



//#endregion

//#endregion // Decoding

//#region Encoding

//#region Public Methods
/// <summary>
/// Returns an array of bytes of the saved stroke
/// </summary>
/// <param name="stroke">Stroke to save</param>
/// <param name="stream">null to calculate only the size</param>
/// <param name="compressionAlgorithm"></param>
/// <param name="guidList"></param>
/// <param name="strokeLookupEntry"></param>
uint StrokeSerializer::EncodeStroke(
    Stroke& stroke,
    QIODevice& stream,
    quint8 compressionAlgorithm,
    GuidList& guidList,
    StrokeCollectionSerializer::StrokeLookupEntry& strokeLookupEntry)
{
    uint cbWrite = SavePackets( stroke,
                                stream,
                                strokeLookupEntry);

    //if (stroke.ExtendedProperties().Count() > 0)
    //    cbWrite += ExtendedPropertySerializer::EncodeAsISF(stroke.ExtendedProperties(), stream, guidList, compressionAlgorithm, false);

    return cbWrite;
}

/// <summary>
/// Builds the Stroke Descriptor for this stroke based on Packet Layout and Extended Properties
/// For details on how this is strored please refer to the spec.
/// </summary>
void StrokeSerializer::BuildStrokeDescriptor(
    Stroke const& stroke,
    GuidList& guidList,
    StrokeCollectionSerializer::StrokeLookupEntry& strokeLookupEntry,
    StrokeDescriptor*& strokeDescriptor,
    MetricBlock*& metricBlock)
{
    // Initialize the metric block for this stroke
    metricBlock = new MetricBlock();

    // Clear any existing template
    strokeDescriptor = new StrokeDescriptor();

    // Uninitialized variable passed in AddMetricEntry
    MetricEntryType metricEntryType;
    SharedPointer<StylusPointDescription> stylusPointDescription = stroke.StylusPoints()->Description();

    KnownTagCache::KnownTagIndex tag = guidList.FindTag(KnownIds::X, true);
    metricEntryType = metricBlock->AddMetricEntry(stylusPointDescription->GetPropertyInfo(StylusPointProperties::X), tag);

    tag = guidList.FindTag(KnownIds::Y, true);
    metricEntryType = metricBlock->AddMetricEntry(stylusPointDescription->GetPropertyInfo(StylusPointProperties::Y), tag);

    List<StylusPointPropertyInfo> propertyInfos
        = stylusPointDescription->GetStylusPointProperties();

    int i = 0; //i is defined out of the for loop so we can use it later for buttons
    for (i = 2/*past x,y*/; i < propertyInfos.Count(); i++)
    {
        if (i == StylusPointDescription::RequiredPressureIndex/*2*/ &&
            !strokeLookupEntry.StorePressure)
        {
            //
            // don't store pressure information
            //
            continue;
        }
        StylusPointPropertyInfo propertyInfo = propertyInfos[i];
        if (propertyInfo.IsButton())
        {
            //we don't serialize buttons
            break;
        }

        tag = guidList.FindTag(propertyInfo.Id(), true);

        strokeDescriptor->Template.Add(tag);
        strokeDescriptor->Size += SerializationHelper::VarSize((uint)tag);

        // Create the MetricEntry for this property if necessary
        metricEntryType = metricBlock->AddMetricEntry(propertyInfo, tag);
    }

    /*
    we drop button data on the floor.  See Windows OS Bugs 1413460 for details
    int buttonCount = stylusPointDescription.ButtonCount;
    // Now write the button tags in the Template
    if (buttonCount > 0)
    {
        // First write the TAG_BUTTONS
        strokeDescriptor->Template.Add(KnownTagCache::KnownTagIndex::Buttons);
        strokeDescriptor->Size += SerializationHelper::VarSize((uint)KnownTagCache::KnownTagIndex::Buttons);

        // Next write the i of buttons
        strokeDescriptor->Template.Add((KnownTagCache::KnownTagIndex)buttonCount);
        strokeDescriptor->Size += SerializationHelper::VarSize((uint)buttonCount);

        //we broke above on i when it was a button, it still
        //points to the first button
        for (; i < propertyInfos.Count; i++)
        {
            StylusPointPropertyInfo propertyInfo = propertyInfos[i];
            tag = guidList.FindTag(propertyInfo.Id, false);

            strokeDescriptor->Template.Add(tag);
            strokeDescriptor->Size += SerializationHelper::VarSize((uint)tag);
        }
    }
    */

    // Now write the extended properties in the template
    if (stroke.ExtendedProperties().Count() > 0)
    {
        strokeDescriptor->Template.Add(KnownTagCache::KnownTagIndex::StrokePropertyList);
        strokeDescriptor->Size += SerializationHelper::VarSize((uint)KnownTagCache::KnownTagIndex::StrokePropertyList);

        // Now write the tags corresponding to each extended properties of the stroke
        for (int x = 0; x < stroke.ExtendedProperties().Count(); x++)
        {
            tag = guidList.FindTag(stroke.ExtendedProperties()[(int)x].Id(), false);

            strokeDescriptor->Template.Add(tag);
            strokeDescriptor->Size += SerializationHelper::VarSize((uint)tag);
        }
    }
}

//#endregion // Private Methods

//#region Private methods

/// <summary>
/// Saves the packets into a stream of bytes
/// </summary>
/// <param name="stroke">Stroke to save</param>
/// <param name="stream">null to calculate size only</param>
/// <param name="strokeLookupEntry"></param>
uint StrokeSerializer::SavePackets(
    Stroke& stroke,
    QIODevice& stream,
    StrokeCollectionSerializer::StrokeLookupEntry& strokeLookupEntry)
{
    // First write or calculate how many points are there
    uint pointCount = (uint)stroke.StylusPoints()->Count();
    //uint localBytesWritten = (stream != null) ? SerializationHelper::Encode(stream, pointCount) : SerializationHelper::VarSize(pointCount);
    uint localBytesWritten = SerializationHelper::Encode(stream, pointCount);
    quint8 compressionAlgorithm;

    Array<Array<int>> outputArrays = strokeLookupEntry.ISFReadyStrokeData;
    //We don't serialize button data, see Windows OS Bugs 1413460 for details
    //int valuesPerPoint = stroke.StylusPoints.Description.GetOutputArrayLengthPerPoint();
    //int buttonCount = stroke.StylusPoints.Description.ButtonCount;

    List<StylusPointPropertyInfo> propertyInfos
        = stroke.StylusPoints()->Description()->GetStylusPointProperties();

    int i = 0;
    for (; i < propertyInfos.Count(); i++)
    {
        StylusPointPropertyInfo propertyInfo = propertyInfos[i];
        if (i == 2 && !strokeLookupEntry.StorePressure)
        {
            //
            // only store pressure if we need to
            //
            continue;
        }
        if (propertyInfo.IsButton())
        {
            //
            // we're at the buttons, handle this below
            //
            break;
        }
        compressionAlgorithm = strokeLookupEntry.CompressionData;
        localBytesWritten += SavePacketPropertyData(outputArrays[i],
                                                    stream,
                                                    propertyInfo.Id(),
                                                    compressionAlgorithm);
    }

    /*
    We don't serialize button data, see Windows OS Bugs 1413460 for details
    // Now write all button data. Button data is stored as if it is another packet property
    // with size (cbuttoncount + 7)/8 bytes and corresponding guids are stored in the packet
    // description. Button data is only stored if buttons are present in the description and there
    // are packets in the stroke
    if (buttonCount > 0 && pointCount > 0)
    {
        Debug::Assert(i == valuesPerPoint - 1);
        BitStreamWriter bitWriter = new BitStreamWriter();
        //
        // Get the array of button data (i is still pointing at it)
        //
        int[] buttonData = outputArrays[i];

        for (int x = 0; x < pointCount; x++)
        {
            //
            // each int in the button data array contains buttonCount number
            // of bits that need to be written to the BitStreamWriter
            // the BitStreamWriter takes bytes at a time.  We always write the most
            // signifigant bits first
            //
            int uncompactedButtonDataForPoint = buttonData[x];

            // calculate the number of full bytes used for buttons per packet
            //   Example: 10 buttons would require 1 full byte
            //            but 8 would require
            int fullBytesForButtonsPerPacket = buttonCount / 8;

            // calculate the number of bits that spill beyond the full byte boundary
            //   Example: 10 buttons would require 2 extra bits (8 fit in a full byte)
            int bitsToWrite = buttonCount % 8;

            for (; fullBytesForButtonsPerPacket >= 0; fullBytesForButtonsPerPacket--)
            {
                byte byteOfButtonData =
                    Convert.ToByte(uncompactedButtonDataForPoint >> (fullBytesForButtonsPerPacket * 8));
                //
                // write 8 or less bytes to the bitwriter
                // checking for 0 handles the case where we're writing 8, 16 or 24 bytes
                // and bitsToWrite is initialize to zero
                //
                if (bitsToWrite > 0)
                {
                    bitWriter.Write(byteOfButtonData, bitsToWrite);
                }
                if (fullBytesForButtonsPerPacket > 0)
                {
                    bitsToWrite = 8;
                }
            }
        }

        // retrieve the button bytes
        byte[] packedButtonData = bitWriter.ToBytes();

        if (packedButtonData.Length !=
            ((buttonCount * pointCount + 7) / 8))
        {
            throw std::runtime_error(("Packed button length not equal to expected length"));
        }

        // write out the packed button data to the output stream
        stream.Write(packedButtonData, 0, packedButtonData.Length);
        localBytesWritten += (uint)packedButtonData.Length;
    }
    */

    return localBytesWritten;
}
/// <summary>
/// Saves the packets data corresponding to a packet property (identified by the guid) into the stream
/// based on the Compression algorithm and compress header
/// </summary>
/// <param name="packetdata">packet data to save</param>
/// <param name="stream">null to calculate only the size</param>
/// <param name="guid"></param>
/// <param name="algo"></param>
uint StrokeSerializer::SavePacketPropertyData(
    Array<int> packetdata,
    QIODevice& stream,
    Guid const & guid,
    quint8& algo)
{
    if (packetdata.Length() == 0)
    {
        return 0;
    }

    QByteArray data = Compressor::CompressPacketData(
                packetdata,
                algo);

    //Debug::Assert(stream != null);
    // Now write the data in the stream
    stream.write(data.data(), (int)data.size());

    return (uint)data.size();
}

INKCANVAS_END_NAMESPACE
