#include "Internal/Ink/InkSerializedFormat/drawingattributeserializer.h"
#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"
#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"
#include "Internal/Ink/InkSerializedFormat/strokecollectionserializer.h"
#include "Internal/Ink/InkSerializedFormat/guidlist.h"
#include "Internal/Ink/InkSerializedFormat/compress.h"
#include "Internal/doubleutil.h"
#include "Windows/Ink/drawingattributes.h"
#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Ink/knownids.h"
#include "Internal/debug.h"

#include <QIODevice>
#include <QDataStream>
#include <QBuffer>

/// <summary>
/// Loads drawing attributes from a memory buffer.
/// </summary>
/// <param name="stream">Memory buffer to read from</param>
/// <param name="guidList">Guid tags if extended properties are used</param>
/// <param name="maximumStreamSize">Maximum size of buffer to read through</param>
/// <param name="da">The drawing attributes collection to decode into</param>
/// <returns>Number of bytes read</returns>

quint32 DrawingAttributeSerializer::DecodeAsISF(QIODevice& stream, GuidList& guidList, quint32 maximumStreamSize, DrawingAttributes & da)
{
    PenTip penTip = PenTip::Default;
    PenStyle penStyle = PenStyle::PenStyleDefault;
    double stylusWidth = V1PenWidthWhenWidthIsMissing;
    double stylusHeight = V1PenHeightWhenHeightIsMissing;
    quint32 rasterOperation = RasterOperationDefaultV1;
    int transparency = TransparencyDefaultV1;
    bool widthIsSetInISF = false; //did we find KnownIds::Width?
    bool heightIsSetInISF = false; //did we find KnownIds::Height?


    quint32 cbTotal = maximumStreamSize;
    while (maximumStreamSize > 0)
    {
        KnownTagCache::KnownTagIndex tag;
        quint32 uiTag;
        // First read the tag
        quint32 cb = SerializationHelper::Decode (stream, uiTag);
        tag = (KnownTagCache::KnownTagIndex)uiTag;

        if (maximumStreamSize < cb)
        {
            throw std::exception("ISF size is larger than maximum stream size");
        }

        maximumStreamSize -= cb;

        // Get the guid based on the tag
        QUuid guid = guidList.FindGuid (tag);
        if (guid == GuidList::Empty)
        {
            throw std::exception("Drawing Attribute tag embedded in ISF stream does not match guid table");
        }

        quint32 dw = 0;

        if (KnownIds::PenTip == guid)
        {
            cb = SerializationHelper::Decode (stream, dw);
            penTip = (PenTip)dw;
            //if (!PenTipHelper.IsDefined(penTip))
            {
                throw std::exception("Invalid PenTip value found in ISF stream");
            }
            maximumStreamSize -= cb;
        }
        else if (KnownIds::PenStyle == guid)
        {
            cb = SerializationHelper::Decode(stream, dw);
            penStyle = (PenStyle)dw;
            maximumStreamSize -= cb;
        }
        else if (KnownIds::DrawingFlags == guid)
        {
            // Encode the drawing flags with considerations for v2 model
            cb = SerializationHelper::Decode (stream, dw);
            DrawingFlags flags = (DrawingFlags)dw;
            da.SetDrawingFlags(flags);
            maximumStreamSize -= cb;
        }
        else if (KnownIds::RasterOperation == guid)
        {
            quint32 ropSize = GuidList::GetDataSizeIfKnownGuid(KnownIds::RasterOperation);
            if (ropSize == 0)
            {
                throw  std::exception(("ROP data size was not found"));
            }

            quint8* data = new quint8[ropSize];
            stream.read( (char *)data, (int)ropSize);

            //if (data != nullptr && data.Length > 0)
            {
                //data[0] holds the allowable values of 0-255
                rasterOperation = data[0];
            }

            maximumStreamSize -= ropSize;
        }
        else if (KnownIds::CurveFittingError == guid)
        {
            cb = SerializationHelper::Decode (stream, dw);
            da.SetFittingError((int)dw);
            maximumStreamSize -= cb;
        }
        else if (KnownIds::StylusHeight == guid || KnownIds::StylusWidth == guid)
        {
            double _size;
            cb = SerializationHelper::Decode (stream, dw);
            _size = dw;
            maximumStreamSize -= cb;
            if (maximumStreamSize > 0)
            {
                cb = SerializationHelper::Decode (stream, dw);
                maximumStreamSize -= cb;
                if (KnownTagCache::KnownTagIndex::Mantissa == (KnownTagCache::KnownTagIndex)dw)
                {
                    quint32 cbInSize;
                    // First thing that is in there is maximumStreamSize of the data
                    cb = SerializationHelper::Decode (stream, cbInSize);
                    maximumStreamSize -= cb;

                    // in maximumStreamSize is one more than the decoded no
                    cbInSize++;
                    if (cbInSize > maximumStreamSize)
                    {
                        throw std::exception("ISF size if greater then maximum stream size");
                    }
                    QByteArray in_data(cbInSize, 0);

                    quint32 bytesRead = (quint32) stream.read (in_data.data(), cbInSize);
                    if (cbInSize != bytesRead)
                    {
                        throw std::exception("Read different size from stream then expected");
                    }

                    //using (MemoryStream localStream = new MemoryStream(out_buffer))
                    //using (BinaryReader rdr = new BinaryReader(localStream))
                    {
                        QByteArray out_buffer = Compressor::DecompressPropertyData (in_data);
                        QBuffer localStream(&out_buffer);
                        localStream.open(QIODevice::ReadOnly);
                        QDataStream rdr(&localStream);
                        short sFraction = 0;
                        rdr >> sFraction;
                        _size += (sFraction / DrawingAttributes::StylusPrecision);

                        maximumStreamSize -= cbInSize;
                    }
                }
                else
                {
                    // Seek it back by cb
                    stream.seek(stream.pos() - cb);
                    maximumStreamSize += cb;
                }
            }
            if (KnownIds::StylusWidth == guid)
            {
                widthIsSetInISF = true;
                stylusWidth = _size;
            }
            else
            {
                heightIsSetInISF = true;
                stylusHeight = _size;
            }
        }
        else if (KnownIds::Transparency == guid)
        {
            cb = SerializationHelper::Decode(stream, dw);
            transparency = (int)dw;
            maximumStreamSize -= cb;
        }
        else if (KnownIds::Color == guid)
        {
            cb = SerializationHelper::Decode(stream, dw);

            QColor color = QColor::fromRgb((quint8)(dw & 0xff), (quint8)((dw & 0xff00) >> 8), (quint8)((dw & 0xff0000) >> 16));
            da.SetColor(color);
            maximumStreamSize -= cb;
        }
        //else if (KnownIds::StylusTipTransform == guid)
        //{
            //try
            //{
            //    object data;
            //    cb = ExtendedPropertySerializer.DecodeAsISF(stream, maximumStreamSize, guidList, tag, guid, data);

            //    Matrix matrix = Matrix.Parse((string)data);
            //    da.GetStylusTip()Transform = matrix;
            //}
            //catch (InvalidOperationException) // Matrix.Parse failed.
            //{
            //    Debug::Assert(false, "Corrupt Matrix in the ExtendedPropertyCollection!");
            //}
            //finally
            //{
            //    maximumStreamSize -= cb;
            //}
        //}
        //else
        //{
        //    object data;
        //    cb = ExtendedPropertySerializer.DecodeAsISF(stream, maximumStreamSize, guidList, tag, guid, data);
        //    maximumStreamSize -= cb;
        //    da.AddPropertyData(guid,data);
        //}
    }

    if (0 != maximumStreamSize)
    {
        throw std::exception ();
    }

    //
    // time to create our drawing attributes.
    //
    // 1) First we need to evaluate PenTip / StylusTip
    // Here is the V1 - V2 mapping
    //
    // PenTip::Circle == StylusTip::Ellipse
    // PenTip::Rectangle == StylusTip::Rectangle
    // PenTip::Rectangle == StylusTip::Diamond
    if (penTip == PenTip::Default)
    {
        //Since StylusTip is stored in the EPC at this point (if set), we can compare against it here.
        if (da.GetStylusTip() != StylusTip::Ellipse)
        {
            //
            // StylusTip was set to something other than Ellipse
            // when we last serialized (or else StylusTip would be Ellipse, the default)
            // when StylusTip is != Ellipse and we serialize, we set PenTip to Rectangle
            // which is not the default.  Therefore, if PenTip is back to Circle,
            // that means someone set it in V1 and we should respect that by
            // changing StylusTip back to Ellipse
            //
            da.SetStylusTip(StylusTip::Ellipse);
        }
        //else da.GetStylusTip() is already set
    }
    else
    {
        Debug::Assert(penTip == PenTip::Rectangle);
        if (da.GetStylusTip() == StylusTip::Ellipse)
        {
            //
            // PenTip is Rectangle and StylusTip was either not set
            // before or was set to Ellipse and PenTip was changed
            // in a V1 ink object.  Either way, we need to change StylusTip to Rectangle
            da.SetStylusTip(StylusTip::Rectangle);
        }
        //else da.GetStylusTip() is already set
    }

    //
    // 2) next we need to set hight and width
    //
    if (da.GetStylusTip() == StylusTip::Ellipse &&
        widthIsSetInISF &&
        !heightIsSetInISF)
    {
        //
        // special case: V1 PenTip of Circle only used Width to compute the circle size
        // and so it only serializes Width of 53
        // but since our default is Ellipse, if Height is unset and we use the default
        // height of 30, then your ink that looked like 53,53 in V1 will look
        // like 30,53 here.
        //
        //
        stylusHeight = stylusWidth;
        da.SetHeightChangedForCompatabity(true);
    }
    // need to convert width/height into Avalon, since they are stored in HIMETRIC in ISF
    stylusHeight *= StrokeCollectionSerializer::HimetricToAvalonMultiplier;
    stylusWidth *= StrokeCollectionSerializer::HimetricToAvalonMultiplier;

    // Map 0.0 width to DrawingAttributes::DefaultXXXXXX (V1 53 equivalent)
    double height = DoubleUtil::IsZero(stylusHeight) ? DrawingAttributes::GetDefaultDrawingAttributeValue(KnownIds::StylusHeight).toDouble() : stylusHeight;
    double width = DoubleUtil::IsZero(stylusWidth) ? DrawingAttributes::GetDefaultDrawingAttributeValue(KnownIds::StylusWidth).toDouble() : stylusWidth;

    da.SetHeight(GetCappedHeightOrWidth(height));
    da.SetWidth(GetCappedHeightOrWidth(width));

    //
    // 3) next we need to set IsHighlighter (by looking for RasterOperation.MaskPen)
    //

    //
    // always store raster op
    //
    da.SetRasterOperation(rasterOperation);
    if (rasterOperation == RasterOperationDefaultV1)
    {
        //
        // if rasterop is default, make sure IsHighlighter isn't in the EPC
        //
        if (da.ContainsPropertyData(KnownIds::IsHighlighter))
        {
            da.RemovePropertyData(KnownIds::IsHighlighter);
        }
    }
    else
    {
        if (rasterOperation == RasterOperationMaskPen)
        {
            da.SetIsHighlighter(true);
        }
    }
    //else, IsHighlighter will be set to false by default, no need to set it

    //
    // 4) see if there is a transparency we need to add to color
    //
    if (transparency > TransparencyDefaultV1)
    {
        //note: Color.A is set to 255 by default, which means fully opaque
        //transparency is just the opposite - 0 means fully opaque so
        //we need to flip the values
        int alpha = qAbs(transparency - 255);
        QColor color = da.Color();
        color.setAlpha(alpha);
        da.SetColor(color);
    }
    return cbTotal;
}

/// <summary>
/// helper to limit what we set for width or height on deserialization
/// </summary>
double DrawingAttributeSerializer::GetCappedHeightOrWidth(double heightOrWidth)
{
    Debug::Assert(DrawingAttributes::MaxHeight == DrawingAttributes::MaxWidth &&
                 DrawingAttributes::MinHeight == DrawingAttributes::MinWidth);

    if (heightOrWidth > DrawingAttributes::MaxHeight)
    {
        return DrawingAttributes::MaxHeight;
    }
    if (heightOrWidth < DrawingAttributes::MinHeight)
    {
        return DrawingAttributes::MinHeight;
    }
    return heightOrWidth;
}
//#endregion // Decoding

//#region Encoding

#if OLD_ISF
/// <Summary>
/// Encodes a DrawingAttriubtesin the ISF stream.
/// </Summary>
/// <SecurityNote>
///     Critical - Calls the critical methods
///         PersistExtendedProperties
///         PersistStylusTip
///
///     This directly called by StrokeCollectionSerializer::SerializeDrawingAttrsTable
///
///     TreatAsSafe boundary is StrokeCollectionSerializer::EncodeISF
///
/// </SecurityNote>
[SecurityCritical]
#else
/// <Summary>
/// Encodes a DrawingAttriubtesin the ISF stream.
/// </Summary>
#endif
quint32 DrawingAttributeSerializer::EncodeAsISF(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, unsigned char compressionAlgorithm, bool fTag)
{
#if DEBUG
    Debug::Assert(compressionAlgorithm == 0);
    Debug::Assert(fTag == true);
#endif
    //Debug::Assert(stream != nullptr);
    quint32 cbData = 0;
    QDataStream bw(&stream);

    PersistDrawingFlags(da, stream, guidList, cbData, bw);

    PersistColorAndTransparency(da, stream, guidList, cbData, bw);

    PersistRasterOperation(da, stream, guidList, cbData, bw);

    PersistWidthHeight(da, stream, guidList, cbData, bw);

    PersistStylusTip(da, stream, guidList, cbData, bw);

    PersistExtendedProperties(da, stream, guidList, cbData, bw, compressionAlgorithm, fTag);

    return cbData;
}


void DrawingAttributeSerializer::PersistDrawingFlags(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw)
{
    //
    // always serialize DrawingFlags, even when it is the default of AntiAliased.  V1 loaders
    // expect it.
    //
    //Debug::Assert(bw != nullptr);
    cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(KnownIds::DrawingFlags, true));
    cbData += SerializationHelper::Encode(stream, (quint32)da.GetDrawingFlags());

    if (da.ContainsPropertyData(KnownIds::CurveFittingError))
    {
        //Debug::Assert(bw != nullptr);
        cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(KnownIds::CurveFittingError, true));
        cbData += SerializationHelper::Encode(stream, (quint32)da.GetPropertyData(KnownIds::CurveFittingError).toUInt());
    }
}

void DrawingAttributeSerializer::PersistColorAndTransparency(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw)
{
    // if the color is non-default (e.g. not black), then store it
    // the v1 encoder throws away the default color (Black) so it isn't valuable
    // to save.
    if (da.ContainsPropertyData(KnownIds::Color))
    {
        QColor daColor = da.Color();
        Debug::Assert(da.Color() != DrawingAttributes::GetDefaultDrawingAttributeValue(KnownIds::Color).value<QColor>(), "Color was put in the EPC for the default value!");

        //Note: we don't store the alpha value of the color (we don't use it)
        quint32 r = (quint32)daColor.red(), g = (quint32)daColor.green(), b = (quint32)(daColor.blue());
        quint32 colorVal = r + (g << 8) + (b << 16);

        //Debug::Assert(bw != nullptr);
        cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(KnownIds::Color, true));
        cbData += SerializationHelper::Encode(stream, colorVal);
    }

    //set transparency if Color.A is set
    quint8 alphaChannel = da.Color().alpha();
    if (alphaChannel != 255)
    {
        //note: Color.A is set to 255 by default, which means fully opaque
        //transparency is just the opposite - 0 means fully opaque so
        //we need to flip the values
        int transparency = qAbs(( (int)alphaChannel ) - 255);
        //Debug::Assert(bw != nullptr);
        cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(KnownIds::Transparency, true));
        cbData += SerializationHelper::Encode(stream, (quint32)transparency);
    }
}

void DrawingAttributeSerializer::PersistRasterOperation(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw)
{
    // write any non-default RasterOp value that we might have picked up from
    // V1 interop or by setting IsHighlighter.
    if (da.RasterOperation() != RasterOperationDefaultV1)
    {
        quint32 ropSize = GuidList::GetDataSizeIfKnownGuid(KnownIds::RasterOperation);
        if (ropSize == 0)
        {
            throw  std::exception("ROP data size was not found");
        }

        //Debug::Assert(bw != nullptr);
        cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(KnownIds::RasterOperation, true));
        long currentPosition = stream.pos();
        bw << (da.RasterOperation());
        if ((quint32)(stream.pos() - currentPosition) != ropSize)
        {
            throw  std::exception("ROP data was incorrectly serialized");
        }
        cbData += ropSize;
    }
}
#if OLD_ISF
/// <Summary>
/// Encodes the ExtendedProperties in the ISF stream.
/// </Summary>
/// <SecurityNote>
///     Critical - Calls the critical method ExtendedPropertySerializer.EncodeAsISF
///
///     This directly called by EncodeAsISF
///
///     TreatAsSafe boundary is StrokeCollectionSerializer::EncodeISF
///
/// </SecurityNote>
[SecurityCritical]
#else
/// <Summary>
/// Encodes the ExtendedProperties in the ISF stream.
/// </Summary>
#endif
void DrawingAttributeSerializer::PersistExtendedProperties(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw, unsigned char compressionAlgorithm, bool fTag)
{
    // Now save the extended properties
    ExtendedPropertyCollection* epcClone = da.CopyPropertyData();

    //walk from the back removing EPs that are uses for DrawingAttributes
    for (int x = epcClone->Count() - 1; x >= 0; x--)
    {
        //
        // look for StylusTipTransform while we're at it and turn it into a string
        // for serialization
        //
        //if (epcClone[x].Id == KnownIds::StylusTipTransform)
        //{
        //    Matrix matrix = (Matrix)epcClone[x].Value;
        //    string matrixString = matrix.ToString(System.Globalization.CultureInfo.InvariantCulture);
        //    epcClone[x].Value = matrixString;
        //    continue;
        //}

        //if (DrawingAttributes::RemoveIdFromExtendedProperties(epcClone[x].Id))
        //{
        //    epcClone.Remove(epcClone[x].Id);
        //}
    }

    //cbData += ExtendedPropertySerializer.EncodeAsISF(epcClone, stream, guidList, compressionAlgorithm, fTag);
}
#if OLD_ISF
/// <Summary>
/// Encodes the StylusTip in the ISF stream.
/// </Summary>
/// <SecurityNote>
///     Critical - Calls the critical method ExtendedPropertySerializer.EncodeAsISF
///
///     This directly called by EncodeAsISF
///
///     TreatAsSafe boundary is StrokeCollectionSerializer::EncodeISF
///
/// </SecurityNote>
[SecurityCritical]
#else
/// <Summary>
/// Encodes the StylusTip in the ISF stream.
/// </Summary>
#endif
void DrawingAttributeSerializer::PersistStylusTip(DrawingAttributes& da, QIODevice& stream, GuidList& guidList, quint32& cbData, QDataStream& bw)
{
    //
    // persist the StylusTip
    //
    if (da.ContainsPropertyData(KnownIds::StylusTip))
    {
        Debug::Assert(da.GetStylusTip() != StylusTip::Ellipse, "StylusTip was put in the EPC for the default value!");

        //
        // persist PenTip::Rectangle for V1 ISF
        //
        //Debug::Assert(bw != nullptr);
        cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(KnownIds::PenTip, true));
        cbData += SerializationHelper::Encode(stream, (quint32)PenTip::Rectangle);

        //using (MemoryStream localStream = new MemoryStream(6)) //reasonable default
        //{
        //    Int32 stylusTip = Convert.ToInt32(da.GetStylusTip(), System.Globalization.CultureInfo.InvariantCulture);
        //    System.Runtime.InteropServices.VarEnum type = SerializationHelper::ConvertToVarEnum(PersistenceTypes.GetStylusTip(), true);
        //    ExtendedPropertySerializer.EncodeAttribute(KnownIds::StylusTip, stylusTip, type, localStream);

        //    cbData += ExtendedPropertySerializer.EncodeAsISF(KnownIds::StylusTip, localStream.ToArray(), stream, guidList, 0, true);
        //}
    }
}

void DrawingAttributeSerializer::PersistWidthHeight(DrawingAttributes& da, QIODevice&  stream, GuidList& guidList, quint32& cbData, QDataStream& bw)
{
    //persist the height and width
    // For v1 loaders we persist height and width in StylusHeight and StylusWidth
    double stylusWidth = da.Width();
    double stylusHeight = da.Height();

    // Save the pen tip's width and height.
    for (int i = 0; i < 2; i++)
    {
        QUuid const & guid = (i == 0) ? KnownIds::StylusWidth : KnownIds::StylusHeight;
        double size = (0 == i) ? stylusWidth : stylusHeight;

        //
        // the size is now in Avalon units, we need to convert to HIMETRIC
        //
        size *= StrokeCollectionSerializer::AvalonToHimetricMultiplier;

        double sizeWhenMissing = (0 == i) ? V1PenWidthWhenWidthIsMissing : V1PenHeightWhenHeightIsMissing;

        //
        // only persist height / width if they are equal to the height / width
        // when missing in the isf stream OR for compatibility with V1
        //
        bool skipPersisting = DoubleUtil::AreClose(size, sizeWhenMissing);
        if ( stylusWidth == stylusHeight &&
             da.GetStylusTip() == StylusTip::Ellipse &&
             guid == KnownIds::StylusHeight &&
             da.HeightChangedForCompatabity())
        {
            //we need to put height in the ISF stream for compat
            skipPersisting = true;
        }


        if (!skipPersisting)
        {
            quint32 uIntegral = (quint32)(size + 0.5f);

            //Debug::Assert(bw != nullptr);
            cbData += SerializationHelper::Encode(stream, (quint32)guidList.FindTag(guid, true));
            cbData += SerializationHelper::Encode(stream, uIntegral);

            short sFraction = (size > uIntegral) ? (short)(DrawingAttributes::StylusPrecision * (size - uIntegral) + 0.5f) : (short)(DrawingAttributes::StylusPrecision * (size - uIntegral) - 0.5);

            // If the fractional values is non zero, we store this value along with TAG_MANTISSA and size with a precisson of 1000
            if (0 != sFraction)
            {
                quint32 cb = 16; // For header NO_COMPRESS

                //Debug::Assert(bw != nullptr);
                cbData += SerializationHelper::Encode(stream, (quint32)KnownTagCache::KnownTagIndex::Mantissa);
                cbData += SerializationHelper::Encode(stream, cb);
                bw << ((quint8)0x00);
                bw << ((short)sFraction);

                cbData += cb + 1; // include size of encoded 0 and encoded fraction value
            }
        }
    }
}

