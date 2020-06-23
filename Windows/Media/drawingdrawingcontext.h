#ifndef DRAWINGDRAWINGCONTEXT_H
#define DRAWINGDRAWINGCONTEXT_H

#include "InkCanvas_global.h"

#include "drawingcontext.h"
#include "geometry.h"

#include <QVector>

// namespace System.Windows.Media
INKCANVAS_BEGIN_NAMESPACE

class DrawingGroup;

/// <summary>
/// Creates DrawingGroup content to represent the Draw calls made to this DrawingContext
/// </summary>
class INKCANVAS_EXPORT DrawingDrawingContext : public DrawingContext
{
    //#region Constructors

public:
    /// <summary>
    /// Default DrawingDrawingContext constructor.
    /// </summary>
    DrawingDrawingContext();

    virtual ~DrawingDrawingContext() override;

    //#endregion Constructors

    //#region Public Methods

    /// <summary>
    ///     DrawLine -
    ///     Draws a line with the specified pen.
    ///     Note that this API does not accept a Brush, as there is no area to fill.
    /// </summary>
    /// <param name="pen"> The QPen with which to stroke the line. </param>
    /// <param name="point0"> The start QPointF const & for the line. </param>
    /// <param name="point1"> The end QPointF const & for the line. </param>
    virtual void DrawLine(
        QPen pen,
        QPointF const & point0,
        QPointF const & point1) override;

    /// <summary>
    ///     DrawLine -
    ///     Draws a line with the specified pen.
    ///     Note that this API does not accept a Brush, as there is no area to fill.
    /// </summary>
    /// <param name="pen"> The QPen with which to stroke the line. </param>
    /// <param name="point0"> The start QPointF const & for the line. </param>
    /// <param name="point0Animations"> Optional AnimationClock * for point0. </param>
    /// <param name="point1"> The end QPointF const & for the line. </param>
    /// <param name="point1Animations"> Optional AnimationClock * for point1. </param>
    virtual void DrawLine(
        QPen pen,
        QPointF const & point0,
        AnimationClock * point0Animations,
        QPointF const & point1,
        AnimationClock * point1Animations) override;
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
        QRectF const & rectangle) override;

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
        AnimationClock * rectangleAnimations) override;

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
        double radiusY) override;

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
        AnimationClock * radiusYAnimations) override;

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
        QPointF const & center,
        double radiusX,
        double radiusY) override;

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
        QPointF const & center,
        AnimationClock * centerAnimations,
        double radiusX,
        AnimationClock * radiusXAnimations,
        double radiusY,
        AnimationClock * radiusYAnimations) override;
    /// <summary>
    ///     DrawGeometry -
    ///     Draw a Geometry& with the provided QBrush and/or Pen.
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
    /// <param name="geometry"> The Geometry& to fill and/or stroke. </param>
    virtual void DrawGeometry(
        QBrush brush,
        QPen pen,
        Geometry * geometry) override;

    /// <summary>
    ///     DrawImage -
    ///     Draw an Image into the region specified by the Rect.
    ///     The Image will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an ImageBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="imageSource"> The ImageSource to draw. </param>
    /// <param name="rectangle">
    ///     The QRectF const & into which the ImageSource will be fit.
    /// </param>
    virtual void DrawImage(
        QImage imageSource,
        QRectF const & rectangle) override;

    /// <summary>
    ///     DrawImage -
    ///     Draw an Image into the region specified by the Rect.
    ///     The Image will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an ImageBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="imageSource"> The ImageSource to draw. </param>
    /// <param name="rectangle">
    ///     The QRectF const & into which the ImageSource will be fit.
    /// </param>
    /// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
    virtual void DrawImage(
        QImage imageSource,
        QRectF const & rectangle,
        AnimationClock * rectangleAnimations) override;

    /// <summary>
    ///     DrawDrawing -
    ///     Draw a Drawing.
    ///     For more fine grained control, consider filling a QRectF const & with an DrawingBrush via
    ///     DrawRect.
    /// </summary>
    /// <param name="drawing"> The Drawing to draw. </param>
    virtual void DrawDrawing(
        Drawing * drawing) override;

    /// <summary>
    ///     DrawVideo -
    ///     Draw a Video into the region specified by the Rect.
    ///     The Video will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an VideoBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="player"> The MediaPlayer to draw. </param>
    /// <param name="rectangle">
    ///     The QRectF const & into which the MediaPlayer will be fit.
    /// </param>
    //virtual void DrawVideo(
    //    MediaPlayer player,
    //    QRectF const & rectangle) override;

    /// <summary>
    ///     DrawVideo -
    ///     Draw a Video into the region specified by the Rect.
    ///     The Video will potentially be stretched and distorted to fit the Rect.
    ///     For more fine grained control, consider filling a QRectF const & with an VideoBrush via
    ///     DrawRectangle.
    /// </summary>
    /// <param name="player"> The MediaPlayer to draw. </param>
    /// <param name="rectangle">
    ///     The QRectF const & into which the MediaPlayer will be fit.
    /// </param>
    /// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
    //virtual void DrawVideo(
    //    MediaPlayer player,
    //    QRectF const & rectangle,
    //    AnimationClock * rectangleAnimations) override;

    /// <summary>
    ///     PushClip -
    ///     Push a clip region, which will apply to all drawing primitives until the
    ///     corresponding Pop call.
    /// </summary>
    /// <param name="clipGeometry"> The Geometry& to which we will clip. </param>
    virtual void PushClip(
        Geometry * clipGeometry) override;

    virtual void PushOpacityMask(QBrush brush) override;

    /// <summary>
    ///     PushOpacity -
    ///     Push an opacity which will blend the composite of all drawing primitives added
    ///     until the corresponding Pop call.
    /// </summary>
    /// <param name="opacity">
    ///     The opacity with which to blend - 0 is transparent, 1 is opaque.
    /// </param>
    virtual void PushOpacity(
        double opacity
        ) override;

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
        AnimationClock * opacityAnimations) override;

    /// <summary>
    ///     PushTransform -
    ///     Push a Transform which will apply to all drawing operations until the corresponding
    ///     Pop.
    /// </summary>
    /// <param name="transform"> The Transform to push. </param>
    virtual void PushTransform(
        QTransform transform) override;

    /// <summary>
    ///     PushGuidelineSet -
    ///     Push a set of guidelines which should be applied
    ///     to all drawing operations until the
    ///     corresponding Pop.
    /// </summary>
    /// <param name="guidelines"> The GuidelineSet to push. </param>
    //virtual void PushGuidelineSet(
    //    GuidelineSet guidelines) override;

    /// <summary>
    ///     PushGuidelineY1 -
    ///     Explicitly push one horizontal guideline.
    /// </summary>
    /// <param name="coordinate"> The coordinate of leading guideline. </param>
    //virtual void PushGuidelineY1(
    //    double coordinate) override;

    /// <summary>
    ///     PushGuidelineY2 -
    ///     Explicitly push a pair of horizontal guidelines.
    /// </summary>
    /// <param name="leadingCoordinate"> The coordinate of leading guideline. </param>
    /// <param name="offsetToDrivenCoordinate">
    ///     The offset from leading guideline to driven guideline.
    /// </param>
    //virtual void PushGuidelineY2(
    //    double leadingCoordinate,
    //    double offsetToDrivenCoordinate) override;

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
    //    BitmapEffectInput effectInput) override;

    /// <summary>
    /// Pop
    /// </summary>
    virtual void Pop() override;

    /// <summary>
    /// Draw a GlyphRun.
    /// </summary>
    /// <param name="foregroundBrush">Foreground QBrush to draw GlyphRun with. </param>
    /// <param name="glyphRun"> The GlyphRun to draw. </param>
    /// <exception cref="ObjectDisposedException">
    /// This call is illegal if this object has already been closed or disposed.
    /// </exception>
    //virtual void DrawGlyphRun(Brush foregroundBrush, GlyphRun glyphRun) override;

    /// <summary>
    /// Dispose() closes this DrawingContext for any further additions, and
    /// returns it's content to the object that created it.
    /// </summary>
    /// <remarks>
    /// Further Draw/Push/Pop calls to this DrawingContext will result in an
    /// exception.  This method also matches any outstanding Push calls with
    /// a cooresponding Pop.  Calling Close after this object has been closed
    /// or Disposed will also result in an exception.
    /// </remarks>
    /// <exception cref="ObjectDisposedException">
    /// This call is illegal if this object has already been Closed.
    /// </exception>
    virtual void Close() override;

protected:
    /// <summary>
    /// Dispose() closes this DrawingContext for any further additions, and
    /// returns it's content to the object that created it.
    /// </summary>
    /// <remarks>
    /// Further Draw/Push/Pop calls to this DrawingContext will result in an
    /// exception.  This method also matches any outstanding Push calls with
    /// a cooresponding Pop.  Multiple calls to Dispose will not result in
    /// an exception.
    /// </remarks>
    virtual void DisposeCore();

    //#endregion Public Methods

    //#region Protected Methods

    /// <summary>
    /// Called during Close/Dispose when the content created this DrawingContext
    /// needs to be committed.
    /// </summary>
    /// <param name="rootDrawingGroupChildren">
    ///     Collection containing the Drawing elements created with this
    ///     DrawingContext.
    /// </param>
    /// <remarks>
    ///     This will only be called once (at most) per instance.
    /// </remarks>
    virtual void CloseCore(QList<Drawing*> rootDrawingGroupChildren);

    /// <summary>
    /// Verifies that the DrawingContext is being referenced from the
    /// appropriate UIContext, and that the object hasn't been disposed
    /// </summary>
    virtual void VerifyApiNonstructuralChange();

    //#endregion Protected Methods

    //#region Internal Properties

public:
    /// <summary>
    /// Determines whether this DrawingContext should connect inheritance contexts
    /// to DependencyObjects which are passed to its methods.
    /// This property is modeled after DependencyObject.CanBeInheritanceContext.
    /// Defaults to true.
    /// NOTE: This is currently only respected by DrawingDrawingContext and sub-classes.
    /// </summary>
    bool CanBeInheritanceContext()
    {
        return _canBeInheritanceContext;
    }

    void SetCanBeInheritanceContext(bool value)
    {
        _canBeInheritanceContext = value;
    }

    //#endregion Internal Properties

    //#region Methods

private:
    /// <summary>
    /// Throws an exception if this object is already disposed.
    /// </summary>
    void VerifyNotDisposed();

    /// <summary>
    /// Freezes the given freezable if the fFreeze flag is true.  Used by
    /// the various drawing methods to freeze resources if there is no
    /// chance the user might attempt to mutate it.
    /// (i.e., there are no animations and the dependant properties are
    /// null or themselves frozen.)
    /// </summary>
    //Freezable SetupNewFreezable(Freezable newFreezable, bool fFreeze);

    /// <summary>
    /// Contains the functionality common to GeometryDrawing operations of
    /// instantiating the GeometryDrawing, setting it's Freezable state,
    /// and Adding it to the Drawing Graph.
    /// </summary>
    void AddNewGeometryDrawing(QBrush brush, QPen pen, Geometry* geometry);

    /// <summary>
    /// Creates a new DrawingGroup for a Push* call by setting the
    /// _currentDrawingGroup to a newly instantiated DrawingGroup,
    /// and saving the previous _currentDrawingGroup value on the
    /// _previousDrawingGroupStack.
    /// </summary>
    void PushNewDrawingGroup();

    /// <summary>
    /// Adds a new Drawing to the DrawingGraph.
    ///
    /// This method avoids creating a DrawingGroup for the common case
    /// where only a single child exists in the root DrawingGroup.
    /// </summary>
    void AddDrawing(Drawing* newDrawing);

    //#endregion Methods

    //#region Fields

protected:
    // Root drawing created by this DrawingContext.
    //
    // If there is only a single child of the root DrawingGroup, _rootDrawing
    // will reference the single child, and the root _currentDrawingGroup
    // value will be null.  Otherwise, _rootDrawing will reference the
    // root DrawingGroup, and be the same value as the root _currentDrawingGroup.
    //
    // Either way, _rootDrawing always references the root drawing.
    Drawing* _rootDrawing = nullptr;

    // Current DrawingGroup that new children are added to
    DrawingGroup* _currentDrawingGroup = nullptr;

private:
    // Previous values of _currentDrawingGroup
    QVector<DrawingGroup*> _previousDrawingGroupStack;

    // Has Dispose() or Close() been called?
    bool _disposed = false;

    // Determines whether this DrawingContext should connect inheritance contexts
    // to DependencyObjects which are passed to its methods.
    bool _canBeInheritanceContext = true;
    //#endregion Fields
};

INKCANVAS_END_NAMESPACE

#endif // DRAWINGDRAWINGCONTEXT_H
