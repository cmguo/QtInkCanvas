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
        GuidTable = 1,
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

        // See comments for KnownQUuidBaseIndex to determine ranges of tags/Guids/indices
    static constexpr uint MaximumPossibleKnownTags = 50;
    static constexpr uint KnownTagCount = MaximumPossibleKnownTags;
};

class KnownIdCache
{
public:
    // This id table includes the original Guids that were hardcoded
    //      into ISF for the TabletPC v1 release
    static QUuid OriginalISFIdTable[];
    static uint OriginalISFIdTableLength;

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

    // This id table includes the Guids that used the persistence APIs
    //      - meaning they didn't have the data type information encoded in ISF
    static QUuid TabletInternalIdTable[];

        // lookup indices for table of Guids used with non-Automation APIs
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

    static constexpr KnownTagCache::KnownTagIndex KnownGuidBaseIndex =
            static_cast<KnownTagCache::KnownTagIndex>(KnownTagCache::MaximumPossibleKnownTags);

        // The maximum value that can be encoded into a single byte is 127.
        // To improve the chances of storing all of the Guids in the ISF QUuid table
        //      with single-byte lookups, the Guids are broken into two ranges
        // 0-50 known tags
        // 50-100 known Guids (reserved)
        // 101-127 custom Guids (user-defined Guids)
        // 128-... more custom Guids, but requiring multiples bytes for QUuid table lookup

        // These values aren't currently used, so comment them out
    // static uint KnownQUuidIndexLimit = MaximumPossibleKnownQUuidIndex;
    static constexpr uint MaximumPossibleKnownGuidIndex = 100;
    static constexpr uint CustomGuidBaseIndex = MaximumPossibleKnownGuidIndex;

    // This id table includes the Guids that have been added to ISF as ExtendedProperties
    //      Note that they are visible to 3rd party applications
    static QUuid ExtendedISFIdTable[];
};

static KnownTagCache::KnownTagIndex operator+(KnownTagCache::KnownTagIndex l, KnownIdCache::OriginalISFIdIndex r)
{
    return static_cast<KnownTagCache::KnownTagIndex>(static_cast<int>(l) + static_cast<int>(r));
}

#endif // KNOWNIDCACHE_H
