#ifndef DRAWINGCONTEXT_H
#define DRAWINGCONTEXT_H

#include <QPointF>
#include <QRectF>
#include <QPen>
#include <QBrush>
#include <QTransform>

class AnimationClock;
class Geometry;
class Drawing;

class DrawingContext
{
public:

    virtual ~DrawingContext() {}

    /// <summary>
    ///     DrawLine -
    ///     Draws a line with the specified pen.
    ///     Note that this API does not accept a Brush, as there is no area to fill.
    /// </summary>
    /// <param name="pen"> The QPen with which to stroke the line. </param>
    /// <param name="point0"> The start QPointF const &for the line. </param>
    /// <param name="point1"> The end QPointF const &for the line. </param>
    virtual void DrawLine(
        QPen pen,
        QPointF const &point0,
        QPointF const &point1) = 0;

    /// <summary>
    ///     DrawLine -
    ///     Draws a line with the specified pen.
    ///     Note that this API does not accept a Brush, as there is no area to fill.
    /// </summary>
    /// <param name="pen"> The QPen with which to stroke the line. </param>
    /// <param name="point0"> The start QPointF const &for the line. </param>
    /// <param name="point0Animations"> Optional AnimationClock * for point0. </param>
    /// <param name="point1"> The end QPointF const &for the line. </param>
    /// <param name="point1Animations"> Optional AnimationClock * for point1. </param>
    virtual void DrawLine(
        QPen pen,
        QPointF const &point0,
        AnimationClock * point0Animations,
        QPointF const &point1,
        AnimationClock * point1Animations) = 0;

    /// <summary>
    ///     DrawRectangle -
    ///     Draw a rectangle with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the rectangle.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the rectangle.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="rectangle"> The QRectF const & to fill and/or stroke. </param>
    virtual void DrawRectangle(
        QBrush brush,
        QPen pen,
        QRectF const& rectangle) = 0;

    /// <summary>
    ///     DrawRectangle -
    ///     Draw a rectangle with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the rectangle.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the rectangle.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="rectangle"> The QRectF const & to fill and/or stroke. </param>
    /// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
    virtual void DrawRectangle(
        QBrush brush,
        QPen pen,
        QRectF const & rectangle,
        AnimationClock * rectangleAnimations) = 0;

    /// <summary>
    ///     DrawRoundedRectangle -
    ///     Draw a rounded rectangle with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the rectangle.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the rectangle.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="rectangle"> The QRectF const & to fill and/or stroke. </param>
    /// <param name="radiusX">
    ///     The radius in the X dimension of the rounded corners of this
    ///     rounded Rect.  This value will be clamped to the range [0..rectangle.Width/2]
    /// </param>
    /// <param name="radiusY">
    ///     The radius in the Y dimension of the rounded corners of this
    ///     rounded Rect.  This value will be clamped to the range [0..rectangle.Height/2].
    /// </param>
    virtual void DrawRoundedRectangle(
        QBrush brush,
        QPen pen,
        QRectF const & rectangle,
        double radiusX,
        double radiusY) = 0;

    /// <summary>
    ///     DrawRoundedRectangle -
    ///     Draw a rounded rectangle with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the rectangle.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the rectangle.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="rectangle"> The QRectF const & to fill and/or stroke. </param>
    /// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
    /// <param name="radiusX">
    ///     The radius in the X dimension of the rounded corners of this
    ///     rounded Rect.  This value will be clamped to the range [0..rectangle.Width/2]
    /// </param>
    /// <param name="radiusXAnimations"> Optional AnimationClock * for radiusX. </param>
    /// <param name="radiusY">
    ///     The radius in the Y dimension of the rounded corners of this
    ///     rounded Rect.  This value will be clamped to the range [0..rectangle.Height/2].
    /// </param>
    /// <param name="radiusYAnimations"> Optional AnimationClock * for radiusY. </param>
    virtual void DrawRoundedRectangle(
        QBrush brush,
        QPen pen,
        QRectF const & rectangle,
        AnimationClock * rectangleAnimations,
        double radiusX,
        AnimationClock * radiusXAnimations,
        double radiusY,
        AnimationClock * radiusYAnimations) = 0;

    /// <summary>
    ///     DrawEllipse -
    ///     Draw an ellipse with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the ellipse.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the ellipse.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="center">
    ///     The center of the ellipse to fill and/or stroke.
    /// </param>
    /// <param name="radiusX">
    ///     The radius in the X dimension of the ellipse.
    ///     The absolute value of the radius provided will be used.
    /// </param>
    /// <param name="radiusY">
    ///     The radius in the Y dimension of the ellipse.
    ///     The absolute value of the radius provided will be used.
    /// </param>
    virtual void DrawEllipse(
        QBrush brush,
        QPen pen,
        QPointF const &center,
        double radiusX,
        double radiusY) = 0;

    /// <summary>
    ///     DrawEllipse -
    ///     Draw an ellipse with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the ellipse.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the ellipse.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="center">
    ///     The center of the ellipse to fill and/or stroke.
    /// </param>
    /// <param name="centerAnimations"> Optional AnimationClock * for center. </param>
    /// <param name="radiusX">
    ///     The radius in the X dimension of the ellipse.
    ///     The absolute value of the radius provided will be used.
    /// </param>
    /// <param name="radiusXAnimations"> Optional AnimationClock * for radiusX. </param>
    /// <param name="radiusY">
    ///     The radius in the Y dimension of the ellipse.
    ///     The absolute value of the radius provided will be used.
    /// </param>
    /// <param name="radiusYAnimations"> Optional AnimationClock * for radiusY. </param>
    virtual void DrawEllipse(
        QBrush brush,
        QPen pen,
        QPointF const &center,
        AnimationClock * centerAnimations,
        double radiusX,
        AnimationClock * radiusXAnimations,
        double radiusY,
        AnimationClock * radiusYAnimations) = 0;

    /// <summary>
    ///     DrawGeometry -
    ///     Draw a Geometry with the provided QBrush and/or Pen.
    ///     If both the QBrush and QPen are null this call is a no-op.
    /// </summary>
    /// <param name="brush">
    ///     The QBrush with which to fill the Geometry.
    ///     This is optional, and can be null, in which case no fill is performed.
    /// </param>
    /// <param name="pen">
    ///     The QPen with which to stroke the Geometry.
    ///     This is optional, and can be null, in which case no stroke is performed.
    /// </param>
    /// <param name="geometry"> The Geometry to fill and/or stroke. </param>
    virtual void DrawGeometry(
        QBrush brush,
        QPen pen,
        Geometry * geometry) = 0;

    /// <summary>
    ///     DrawImage -
    ///     Draw an Image into the region specified by the Rect.
    ///     The Image will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an ImageQBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="imageSource"> The ImageSource to draw. </param>
    /// <param name="rectangle">
    ///     The QRectF const & into which the ImageSource will be fit.
    /// </param>
    virtual void DrawImage(
        QPixmap imageSource,
        QRectF const & rectangle) = 0;

    /// <summary>
    ///     DrawImage -
    ///     Draw an Image into the region specified by the Rect.
    ///     The Image will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an ImageQBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="imageSource"> The ImageSource to draw. </param>
    /// <param name="rectangle">
    ///     The QRectF const & into which the ImageSource will be fit.
    /// </param>
    /// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
    virtual void DrawImage(
        QPixmap imageSource,
        QRectF const & rectangle,
        AnimationClock * rectangleAnimations) = 0;

    /// <summary>
    ///     DrawGlyphRun -
    ///     Draw a GlyphRun
    /// </summary>
    /// <param name="foregroundBrush">
    ///     Foreground QBrush to draw the GlyphRun with.
    /// </param>
    /// <param name="glyphRun"> The GlyphRun to draw.  </param>
    //virtual void DrawGlyphRun(
    //    QBrush foregroundBrush,
    //    GlyphRun glyphRun) = 0;

    /// <summary>
    ///     DrawDrawing -
    ///     Draw a Drawing by appending a sub-Drawing to the current Drawing.
    /// </summary>
    /// <param name="drawing"> The drawing to draw. </param>
    virtual void DrawDrawing(
        Drawing * drawing) = 0;

    /// <summary>
    ///     DrawVideo -
    ///     Draw a Video into the region specified by the Rect.
    ///     The Video will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an VideoQBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="player"> The MediaPlayer to draw. </param>
    /// <param name="rectangle"> The QRectF const & into which the media will be fit. </param>
    //virtual void DrawVideo(
    //    MediaPlayer player,
    //    QRectF const & rectangle) = 0;

    /// <summary>
    ///     DrawVideo -
    ///     Draw a Video into the region specified by the Rect.
    ///     The Video will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an VideoQBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="player"> The MediaPlayer to draw. </param>
    /// <param name="rectangle"> The QRectF const & into which the media will be fit. </param>
    /// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
    //virtual void DrawVideo(
    //    MediaPlayer player,
    //    QRectF const & rectangle,
    //    AnimationClock * rectangleAnimations) = 0;

    /// <summary>
    ///     PushClip -
    ///     Push a clip region, which will apply to all drawing primitives until the
    ///     corresponding Pop call.
    /// </summary>
    /// <param name="clipGeometry"> The Geometry to which we will clip. </param>
    virtual void PushClip(
        Geometry * clipGeometry) = 0;

    /// <summary>
    ///     PushOpacityMask -
    ///     Push an opacity mask which will blend the composite of all drawing primitives added
    ///     until the corresponding Pop call.
    /// </summary>
    /// <param name="opacityMask"> The opacity mask </param>
    virtual void PushOpacityMask(
        QBrush opacityMask) = 0;

    /// <summary>
    ///     PushOpacity -
    ///     Push an opacity which will blend the composite of all drawing primitives added
    ///     until the corresponding Pop call.
    /// </summary>
    /// <param name="opacity">
    ///     The opacity with which to blend - 0 is transparent, 1 is opaque.
    /// </param>
    virtual void PushOpacity(
        double opacity) = 0;

    /// <summary>
    ///     PushOpacity -
    ///     Push an opacity which will blend the composite of all drawing primitives added
    ///     until the corresponding Pop call.
    /// </summary>
    /// <param name="opacity">
    ///     The opacity with which to blend - 0 is transparent, 1 is opaque.
    /// </param>
    /// <param name="opacityAnimations"> Optional AnimationClock * for opacity. </param>
    virtual void PushOpacity(
        double opacity,
        AnimationClock * opacityAnimations) = 0;

    /// <summary>
    ///     PushTransform -
    ///     Push a Transform which will apply to all drawing operations until the corresponding
    ///     Pop.
    /// </summary>
    /// <param name="transform"> The Transform to push. </param>
    virtual void PushTransform(
        QTransform transform) = 0;

    /// <summary>
    ///     PushGuidelineSet -
    ///     Push a set of guidelines which will apply to all drawing operations until the
    ///     corresponding Pop.
    /// </summary>
    /// <param name="guidelines"> The GuidelineSet to push. </param>
    //virtual void PushGuidelineSet(
    //    GuidelineSet guidelines) = 0;

    /// <summary>
    ///     PushGuidelineY1 -
    ///     Explicitly push one horizontal guideline.
    /// </summary>
    /// <param name="coordinate"> The coordinate of leading guideline. </param>
    //virtual void PushGuidelineY1(
    //    double coordinate) = 0;

    /// <summary>
    ///     PushGuidelineY2 -
    ///     Explicitly push a pair of horizontal guidelines.
    /// </summary>
    /// <param name="leadingCoordinate">
    ///     The coordinate of leading guideline.
    /// </param>
    /// <param name="offsetToDrivenCoordinate">
    ///     The offset from leading guideline to driven guideline.
    /// </param>
    //virtual void PushGuidelineY2(
    //    double leadingCoordinate,
    //    double offsetToDrivenCoordinate) = 0;

    /// <summary>
    ///     PushEffect -
    ///     Push a BitmapEffect which will apply to all drawing operations until the
    ///     corresponding Pop.
    /// </summary>
    /// <param name="effect"> The BitmapEffect to push. </param>
    /// <param name="effectInput"> The BitmapEffectInput. </param>
    //[Obsolete(MS.Internal.Media.VisualTreeUtils.BitmapEffectObsoleteMessage)]
    //virtual void PushEffect(
    //    BitmapEffect effect,
    //    BitmapEffectInput effectInput) = 0;

    /// <summary>
    /// Pop
    /// </summary>
    virtual void Pop(
        ) = 0;

    /// <summary>
    /// Closes the DrawingContext and flushes the content.
    /// Afterwards the DrawingContext can not be used anymore.
    /// This call does not require all Push calls to have been Popped.
    /// </summary>
    /// <exception cref="ObjectDisposedException">
    /// This call is illegal if this object has already been closed or disposed.
    /// </exception>
    virtual void Close() = 0;
};

#endif // DRAWINGCONTEXT_H
