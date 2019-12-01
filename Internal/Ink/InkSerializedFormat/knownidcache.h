#ifndef KNOWNIDCACHE_H
#define KNOWNIDCACHE_H

#include <QUuid>

// namespace MS.Internal.Ink.InkSerializedFormat

class KnownTagCache
{
public:
    enum KnownTagIndex
    {
        Unknown = 0,
        InkSpaceRectangle = 0,
        QUuidTable = 1,
        DrawingAttributesTable = 2,
        DrawingAttributesBlock = 3,
        StrokeDescriptorTable = 4,
        StrokeDescriptorBlock = 5,
        Buttons = 6,
        NoX = 7,
        NoY = 8,
        DrawingAttributesTableIndex = 9,
        Stroke = 10,
        StrokePropertyList = 11,
        PointProperty = 12,
        StrokeDescriptorTableIndex = 13,
        CompressionHeader = 14,
        TransformTable = 15,
        Transform = 16,
        TransformIsotropicScale = 17,
        TransformAnisotropicScale = 18,
        TransformRotate = 19,
        TransformTranslate = 20,
        TransformScaleAndTranslate = 21,
        TransformQuad = 22,
        TransformTableIndex = 23,
        MetricTable = 24,
        MetricBlock = 25,
        MetricTableIndex = 26,
        Mantissa = 27,
        PersistenceFormat = 28,
        HimetricSize = 29,
        StrokeIds = 30,
        ExtendedTransformTable = 31,
    };

        // See comments for KnownQUuidBaseIndex to determine ranges of tags/QUuids/indices
    static constexpr uint MaximumPossibleKnownTags = 50;
    static constexpr uint KnownTagCount = MaximumPossibleKnownTags;
};

class KnownIdCache
{
public:
    // This id table includes the original QUuids that were hardcoded
    //      into ISF for the TabletPC v1 release
    static QUuid OriginalISFIdTable[];

    // Size of data used by identified by specified QUuid/Id
    static uint OriginalISFIdPersistenceSize[];

    enum OriginalISFIdIndex
    {
        X = 0,
        Y = 1,
        Z = 2,
        PacketStatus = 3,
        TimerTick = 4,
        SerialNumber = 5,
        NormalPressure = 6,
        TangentPressure = 7,
        ButtonPressure = 8,
        XTiltOrientation = 9,
        YTiltOrientation = 10,
        AzimuthOrientation = 11,
        AltitudeOrientation = 12,
        TwistOrientation = 13,
        PitchRotation = 14,
        RollRotation = 15,
        YawRotation = 16,
        PenStyle = 17,
        ColorRef = 18,
        StylusWidth = 19,
        StylusHeight = 20,
        PenTip = 21,
        DrawingFlags = 22,
        CursorId = 23,
        WordAlternates = 24,
        CharAlternates = 25,
        InkMetrics = 26,
        QUuideStructure = 27,
        Timestamp = 28,
        Language = 29,
        Transparency = 30,
        CurveFittingError = 31,
        RecoLattice = 32,
        CursorDown = 33,
        SecondaryTipSwitch = 34,
        BarrelDown = 35,
        TabletPick = 36,
        RasterOperation = 37,
        MAXIMUM = 37,
    };

    // This id table includes the QUuids that used the persistence APIs
    //      - meaning they didn't have the data type information encoded in ISF
    static QUuid TabletInternalIdTable[];

        // lookup indices for table of QUuids used with non-Automation APIs
    enum TabletInternalIdIndex
    {
        Highlighter = 0,
        InkProperties = 1,
        InkStyleBold = 2,
        InkStyleItalics = 3,
        StrokeTimestamp = 4,
        StrokeTimeId = 5,
        InkStrokeLattice = 6,
        InkCustomStrokes = 7,
        InternalMAXIMUM = 7
    };

    static constexpr KnownTagCache::KnownTagIndex KnownQUuidBaseIndex =
            static_cast<KnownTagCache::KnownTagIndex>(KnownTagCache::MaximumPossibleKnownTags);

        // The maximum value that can be encoded into a single byte is 127.
        // To improve the chances of storing all of the QUuids in the ISF QUuid table
        //      with single-byte lookups, the QUuids are broken into two ranges
        // 0-50 known tags
        // 50-100 known QUuids (reserved)
        // 101-127 custom QUuids (user-defined QUuids)
        // 128-... more custom QUuids, but requiring multiples bytes for QUuid table lookup

        // These values aren't currently used, so comment them out
    // static uint KnownQUuidIndexLimit = MaximumPossibleKnownQUuidIndex;
    static constexpr uint MaximumPossibleKnownQUuidIndex = 100;
    static constexpr uint CustomQUuidBaseIndex = MaximumPossibleKnownQUuidIndex;

    // This id table includes the QUuids that have been added to ISF as ExtendedProperties
    //      Note that they are visible to 3rd party applications
    static QUuid ExtendedISFIdTable[];
};


#endif // KNOWNIDCACHE_H
