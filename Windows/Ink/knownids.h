#ifndef KNOWNIDS_H
#define KNOWNIDS_H

#include <QUuid>

// namespace System.Windows.Ink

/// <summary>
///    <para>DrawingAttributeIds</para>
/// </summary>
class DrawingAttributeIds
{
public:
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Color;
    /// <summary>
    /// const QUuid identifying the StylusTip
    /// </summary>
    static QUuid const StylusTip;
    /// <summary>
    /// const QUuid identifying the StylusTipTransform
    /// </summary>
    static QUuid const StylusTipTransform;
    /// <summary>
    ///    <para>The height of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static QUuid const & StylusHeight;
    /// <summary>
    ///    <para>The width of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static QUuid const & StylusWidth;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & DrawingFlags;
    /// <summary>
    /// const QUuid identifying IsHighlighter
    /// </summary>
    static QUuid const IsHighlighter;
};

/// <summary>
///    <para>[To be supplied.]</para>
/// </summary>
class KnownIds
{
public:
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & X;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Y;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Z;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & PacketStatus;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & TimerTick;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & SerialNumber;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & NormalPressure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & TangentPressure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & ButtonPressure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & XTiltOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & YTiltOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & AzimuthOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & AltitudeOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & TwistOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & PitchRotation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & RollRotation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & YawRotation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Color;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & DrawingFlags;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & CursorId;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & WordAlternates;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & CharacterAlternates;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & InkMetrics;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & QUuideStructure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Timestamp;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Language;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & Transparency;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & CurveFittingError;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & RecognizedLattice;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & CursorDown;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & SecondaryTipSwitch;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & TabletPick;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & BarrelDown;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static QUuid const & RasterOperation;

    /// <summary>
    ///    <para>The height of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static QUuid const & StylusHeight;

    /// <summary>
    ///    <para>The width of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static QUuid const & StylusWidth;

    /// <summary>
    /// const QUuid identifying the highlighter property
    /// </summary>
    static QUuid const & Highlighter;
    /// <summary>
    /// const QUuid identifying the Ink properties
    /// </summary>
    static QUuid const & InkProperties;
    /// <summary>
    /// const QUuid identifying the Ink Style bold property
    /// </summary>
    static QUuid const & InkStyleBold;
    /// <summary>
    /// const QUuid identifying the ink style italics property
    /// </summary>
    static QUuid const & InkStyleItalics;
    /// <summary>
    /// const QUuid identifying the stroke timestamp property
    /// </summary>
    static QUuid const & StrokeTimestamp;
    /// <summary>
    /// const QUuid identifying the stroke timeid property
    /// </summary>
    static QUuid const & StrokeTimeId;

    /// <summary>
    /// const QUuid identifying the StylusTip
    /// </summary>
    static QUuid const StylusTip;

    /// <summary>
    /// const QUuid identifying the StylusTipTransform
    /// </summary>
    static QUuid const StylusTipTransform;


    /// <summary>
    /// const QUuid identifying IsHighlighter
    /// </summary>
    static QUuid const IsHighlighter;

//        /// <summary>
//        /// const QUuid used for identifying the fill-brush for rendering a stroke.
//        /// </summary>
//        static QUuid const & FillBrush              = new QUuid(0x9a547c5c, 0x1fff, 0x4987, 0x8a, 0xb6, 0xbe, 0xed, 0x75, 0xde, 0xa, 0x1d);
//
//        /// <summary>
//        /// const QUuid used for identifying the pen used for rendering a stroke's outline.
//        /// </summary>
//        static QUuid const & OutlinePen             = new QUuid(0x9967aea6, 0x3980, 0x4337, 0xb7, 0xc6, 0x34, 0xa, 0x33, 0x98, 0x8e, 0x6b);
//
//        /// <summary>
//        /// const QUuid used for identifying the blend mode used for rendering a stroke (similar to ROP in v1).
//        /// </summary>
//        static QUuid const & BlendMode              = new QUuid(0xd6993943, 0x7a84, 0x4a80, 0x84, 0x68, 0xa8, 0x3c, 0xca, 0x65, 0xb0, 0x5);
//
//        /// <summary>
//        /// const QUuid used for identifying StylusShape object
//        /// </summary>
//        static QUuid const & StylusShape = new QUuid(0xf998e7f8, 0x7cdb, 0x4c0e, 0xb2, 0xe2, 0x63, 0x2b, 0xca, 0x21, 0x2a, 0x7b);


    /// <summary>
    ///    <para>The style of the rendering used for the pen tip.</para>
    /// </summary>
    static QUuid const & PenStyle;

    /// <summary>
    ///    <para>The shape of the tip of the pen used for stroke rendering.</para>
    /// </summary>
    static QUuid const & PenTip;

    /// <summary>
    /// const QUuid used for identifying the Custom Stroke
    /// </summary>
    /// <remarks>NTRAID#T2-17751-2003/11/26-Microsoft: SPECISSUE: Should we hide the CustomStrokes and StrokeLattice data?</remarks>
    static QUuid const & InkCustomStrokes;

    /// <summary>
    /// const QUuid used for identifying the Stroke Lattice
    /// </summary>
    static QUuid const & InkStrokeLattice;

#if UNDO_ENABLED
    /// <summary>
    /// const QUuid used for identifying if an undo/event has already been handled
    /// </summary>
    /// <remarks>{053BF717-DBE7-4e52-805E-64906138FAAD}</remarks>
    static QUuid const & UndoEventArgsHandled = new QUuid(0x53bf717, 0xdbe7, 0x4e52, 0x80, 0x5e, 0x64, 0x90, 0x61, 0x38, 0xfa, 0xad);
#endif

    static QString ConvertToString (const QUuid & id)
    {
        return id.toString();
    }

};

#endif // KNOWNIDS_H
