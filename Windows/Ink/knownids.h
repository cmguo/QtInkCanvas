#ifndef KNOWNIDS_H
#define KNOWNIDS_H

#include "InkCanvas_global.h"

#include "guid.h"

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

/// <summary>
///    <para>DrawingAttributeIds</para>
/// </summary>
class DrawingAttributeIds
{
public:
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Color;
    /// <summary>
    /// const Guid identifying the StylusTip
    /// </summary>
    static Guid const StylusTip;
    /// <summary>
    /// const Guid identifying the StylusTipTransform
    /// </summary>
    static Guid const StylusTipTransform;
    /// <summary>
    ///    <para>The height of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static Guid const & StylusHeight;
    /// <summary>
    ///    <para>The width of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static Guid const & StylusWidth;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & DrawingFlags;
    /// <summary>
    /// const Guid identifying IsHighlighter
    /// </summary>
    static Guid const IsHighlighter;
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
    static Guid const & X;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Y;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Z;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & PacketStatus;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & TimerTick;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & SerialNumber;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & NormalPressure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & TangentPressure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & ButtonPressure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & XTiltOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & YTiltOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & AzimuthOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & AltitudeOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & TwistOrientation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & PitchRotation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & RollRotation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & YawRotation;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Color;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & DrawingFlags;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & CursorId;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & WordAlternates;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & CharacterAlternates;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & InkMetrics;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & GuideStructure;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Timestamp;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Language;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & Transparency;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & CurveFittingError;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & RecognizedLattice;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & CursorDown;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & SecondaryTipSwitch;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & TabletPick;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & BarrelDown;
    /// <summary>
    ///    <para>[To be supplied.]</para>
    /// </summary>
    static Guid const & RasterOperation;

    /// <summary>
    ///    <para>The height of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static Guid const & StylusHeight;

    /// <summary>
    ///    <para>The width of the pen tip which affects the stroke rendering.</para>
    /// </summary>
    static Guid const & StylusWidth;

    /// <summary>
    /// const Guid identifying the highlighter property
    /// </summary>
    static Guid const & Highlighter;
    /// <summary>
    /// const Guid identifying the Ink properties
    /// </summary>
    static Guid const & InkProperties;
    /// <summary>
    /// const Guid identifying the Ink Style bold property
    /// </summary>
    static Guid const & InkStyleBold;
    /// <summary>
    /// const Guid identifying the ink style italics property
    /// </summary>
    static Guid const & InkStyleItalics;
    /// <summary>
    /// const Guid identifying the stroke timestamp property
    /// </summary>
    static Guid const & StrokeTimestamp;
    /// <summary>
    /// const Guid identifying the stroke timeid property
    /// </summary>
    static Guid const & StrokeTimeId;

    /// <summary>
    /// const Guid identifying the StylusTip
    /// </summary>
    static Guid const StylusTip;

    /// <summary>
    /// const Guid identifying the StylusTipTransform
    /// </summary>
    static Guid const StylusTipTransform;


    /// <summary>
    /// const Guid identifying IsHighlighter
    /// </summary>
    static Guid const IsHighlighter;

//        /// <summary>
//        /// const Guid used for identifying the fill-brush for rendering a stroke.
//        /// </summary>
//        static Guid const & FillBrush              = new Guid(0x9a547c5c, 0x1fff, 0x4987, 0x8a, 0xb6, 0xbe, 0xed, 0x75, 0xde, 0xa, 0x1d);
//
//        /// <summary>
//        /// const Guid used for identifying the pen used for rendering a stroke's outline.
//        /// </summary>
//        static Guid const & OutlinePen             = new Guid(0x9967aea6, 0x3980, 0x4337, 0xb7, 0xc6, 0x34, 0xa, 0x33, 0x98, 0x8e, 0x6b);
//
//        /// <summary>
//        /// const Guid used for identifying the blend mode used for rendering a stroke (similar to ROP in v1).
//        /// </summary>
//        static Guid const & BlendMode              = new Guid(0xd6993943, 0x7a84, 0x4a80, 0x84, 0x68, 0xa8, 0x3c, 0xca, 0x65, 0xb0, 0x5);
//
//        /// <summary>
//        /// const Guid used for identifying StylusShape object
//        /// </summary>
//        static Guid const & StylusShape = new Guid(0xf998e7f8, 0x7cdb, 0x4c0e, 0xb2, 0xe2, 0x63, 0x2b, 0xca, 0x21, 0x2a, 0x7b);


    /// <summary>
    ///    <para>The style of the rendering used for the pen tip.</para>
    /// </summary>
    static Guid const & PenStyle;

    /// <summary>
    ///    <para>The shape of the tip of the pen used for stroke rendering.</para>
    /// </summary>
    static Guid const & PenTip;

    /// <summary>
    /// const Guid used for identifying the Custom Stroke
    /// </summary>
    /// <remarks>NTRAID#T2-17751-2003/11/26-Microsoft: SPECISSUE: Should we hide the CustomStrokes and StrokeLattice data?</remarks>
    static Guid const & InkCustomStrokes;

    /// <summary>
    /// const Guid used for identifying the Stroke Lattice
    /// </summary>
    static Guid const & InkStrokeLattice;

#if UNDO_ENABLED
    /// <summary>
    /// const Guid used for identifying if an undo/event has already been handled
    /// </summary>
    /// <remarks>{053BF717-DBE7-4e52-805E-64906138FAAD}</remarks>
    static Guid const & UndoEventArgsHandled = new Guid(0x53bf717, 0xdbe7, 0x4e52, 0x80, 0x5e, 0x64, 0x90, 0x61, 0x38, 0xfa, 0xad);
#endif

#ifndef INKCANVAS_CORE
    static QString ConvertToString (const Guid & id)
    {
        return QUuid(id).toString();
    }
#endif

};

INKCANVAS_END_NAMESPACE

#endif // KNOWNIDS_H
