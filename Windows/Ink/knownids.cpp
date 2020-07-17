#include "Windows/Ink/knownids.h"
#include "Internal/Ink/InkSerializedFormat/isftagandguidcache.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & DrawingAttributeIds::Color = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::ColorRef];
/// <summary>
/// Guid const & DrawingAttributeIds::identifying the StylusTip
/// </summary>
Guid const DrawingAttributeIds::StylusTip(0x3526c731, 0xee79u, 0x4988, 0xb9, 0x3e, 0x70, 0xd9, 0x2f, 0x89, 0x7, 0xed);
/// <summary>
/// Guid const & DrawingAttributeIds::identifying the StylusTipTransform
/// </summary>
Guid const DrawingAttributeIds::StylusTipTransform(0x4b63bc16, 0x7bc4, 0x4fd2, 0x95, 0xda, 0xac, 0xff, 0x47, 0x75, 0x73, 0x2d);
/// <summary>
///    <para>The height of the pen tip which affects the stroke rendering.</para>
/// </summary>
Guid const & DrawingAttributeIds::StylusHeight = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::StylusHeight];
/// <summary>
///    <para>The width of the pen tip which affects the stroke rendering.</para>
/// </summary>
Guid const & DrawingAttributeIds::StylusWidth = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::StylusWidth];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & DrawingAttributeIds::DrawingFlags = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::DrawingFlags];
/// <summary>
/// Guid const & DrawingAttributeIds::identifying IsHighlighter
/// </summary>
Guid const DrawingAttributeIds::IsHighlighter(0xce305e1a, 0xe08, 0x45e3, 0x8c, 0xdc, 0xe4, 0xb, 0xb4, 0x50, 0x6f, 0x21);


/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::X = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::X];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::Y = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::Y];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::Z = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::Z];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::PacketStatus = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::PacketStatus];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::TimerTick = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::TimerTick];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::SerialNumber = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::SerialNumber];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::NormalPressure = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::NormalPressure];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::TangentPressure = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::TangentPressure];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::ButtonPressure = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::ButtonPressure];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::XTiltOrientation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::XTiltOrientation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::YTiltOrientation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::YTiltOrientation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::AzimuthOrientation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::AzimuthOrientation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::AltitudeOrientation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::AltitudeOrientation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::TwistOrientation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::TwistOrientation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::PitchRotation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::PitchRotation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::RollRotation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::RollRotation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::YawRotation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::YawRotation];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::Color = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::ColorRef];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::DrawingFlags = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::DrawingFlags];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::CursorId = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::CursorId];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::WordAlternates = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::WordAlternates];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::CharacterAlternates = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::CharAlternates];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::InkMetrics = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::InkMetrics];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::GuideStructure = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::GuideStructure];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::Timestamp = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::Timestamp];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::Language = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::Language];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::Transparency = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::Transparency];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::CurveFittingError = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::CurveFittingError];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::RecognizedLattice = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::RecoLattice];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::CursorDown = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::CursorDown];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::SecondaryTipSwitch = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::SecondaryTipSwitch];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::TabletPick = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::TabletPick];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::BarrelDown = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::BarrelDown];
/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
Guid const & KnownIds::RasterOperation = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::RasterOperation];

/// <summary>
///    <para>The height of the pen tip which affects the stroke rendering.</para>
/// </summary>
Guid const & KnownIds::StylusHeight = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::StylusHeight];

/// <summary>
///    <para>The width of the pen tip which affects the stroke rendering.</para>
/// </summary>
Guid const & KnownIds::StylusWidth = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::StylusWidth];

/// <summary>
/// Guid const & identifying the highlighter property
/// </summary>
Guid const & KnownIds::Highlighter = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::Highlighter];
/// <summary>
/// Guid const & identifying the Ink properties
/// </summary>
Guid const & KnownIds::InkProperties = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::InkProperties];
/// <summary>
/// Guid const & identifying the Ink Style bold property
/// </summary>
Guid const & KnownIds::InkStyleBold = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::InkStyleBold];
/// <summary>
/// Guid const & identifying the ink style italics property
/// </summary>
Guid const & KnownIds::InkStyleItalics = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::InkStyleItalics];
/// <summary>
/// Guid const & identifying the stroke timestamp property
/// </summary>
Guid const & KnownIds::StrokeTimestamp = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::StrokeTimestamp];
/// <summary>
/// Guid const & identifying the stroke timeid property
/// </summary>
Guid const & KnownIds::StrokeTimeId = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::StrokeTimeId];

/// <summary>
/// Guid const & identifying the StylusTip
/// </summary>
Guid const KnownIds::StylusTip(0x3526c731, 0xee79u, 0x4988, 0xb9, 0x3e, 0x70, 0xd9, 0x2f, 0x89, 0x7, 0xed);

/// <summary>
/// Guid const & identifying the StylusTipTransform
/// </summary>
Guid const KnownIds::StylusTipTransform(0x4b63bc16, 0x7bc4, 0x4fd2, 0x95, 0xda, 0xac, 0xff, 0x47, 0x75, 0x73, 0x2d);

/// <summary>
/// Guid const & identifying IsHighlighter
/// </summary>
Guid const KnownIds::IsHighlighter(0xce305e1a, 0xe08, 0x45e3, 0x8c, 0xdc, 0xe4, 0xb, 0xb4, 0x50, 0x6f, 0x21);

/// <summary>
///    <para>The style of the rendering used for the pen tip.</para>
/// </summary>
Guid const & KnownIds::PenStyle = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::PenStyle];

/// <summary>
///    <para>The shape of the tip of the pen used for stroke rendering.</para>
/// </summary>
Guid const & KnownIds::PenTip = KnownIdCache::OriginalISFIdTable[KnownIdCache::OriginalISFIdIndex::PenTip];

/// <summary>
/// Guid const & used for identifying the Custom Stroke
/// </summary>
/// <remarks>NTRAID#T2-17751-2003/11/26-Microsoft: SPECISSUE: Should we hide the CustomStrokes and StrokeLattice data?</remarks>
Guid const & KnownIds::InkCustomStrokes     = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::InkCustomStrokes];

/// <summary>
/// Guid const & used for identifying the Stroke Lattice
/// </summary>
Guid const & KnownIds::InkStrokeLattice     = KnownIdCache::TabletInternalIdTable[KnownIdCache::TabletInternalIdIndex::InkStrokeLattice];

INKCANVAS_END_NAMESPACE
