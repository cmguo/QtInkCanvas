#include "drawingdrawingcontext.h"
#include "debug.h"
#include "drawing.h"

/// <summary>
/// Default DrawingDrawingContext constructor.
/// </summary>
DrawingDrawingContext::DrawingDrawingContext()
{
}

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
void DrawingDrawingContext::DrawingDrawingContext::DrawLine(
    QPen pen,
    QPointF const & point0,
    QPointF const & point1)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawLine(const)");
//#endif

    // Forward call to the animate version with nullptr animations
    DrawLine(pen, point0, nullptr, point1, nullptr);
}

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
void DrawingDrawingContext::DrawingDrawingContext::DrawLine(
    QPen pen,
    QPointF const & point0,
    AnimationClock * point0Animations,
    QPointF const & point1,
    AnimationClock * point1Animations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawLine(animate)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //if (pen == nullptr)
    //{
    //    return;
    //}

    //
    // Create a geometry & add animations if they exist
    //

    // Instantiate the geometry
    LineGeometry* geometry = new LineGeometry(point0, point1);

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //geometry.CanBeInheritanceContext = CanBeInheritanceContext;

    // Setup the geometries freezable-related state
    //SetupNewFreezable(
    //    geometry,
    //    (point0Animations == nullptr) && // Freeze if animations are nullptr
    //    (point1Animations == nullptr)
    //    );

    // Add animations to the geometry
    if (point0Animations != nullptr)
    {
        ////geometry.ApplyAnimationClock(LineGeometry.StartPointProperty, point0Animations);
    }

    if(point1Animations != nullptr)
    {
        ////geometry.ApplyAnimationClock(LineGeometry.EndPointProperty, point1Animations);
    }

    //
    // Add Drawing to the Drawing graph
    //

    AddNewGeometryDrawing(QBrush(), pen, geometry);
}
/// <summary>
///     DrawRectangle -
///     Draw a rectangle with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the rectangle.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the rectangle.
///     This is optional, and can be nullptr, in which case no stroke is performed.
/// </param>
/// <param name="rectangle"> The QRectF const & to fill and/or stroke. </param>
void DrawingDrawingContext::DrawingDrawingContext::DrawRectangle(
    QBrush brush,
    QPen pen,
    QRectF const & rectangle)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawRectangle(const)");
//#endif

    // Forward call to the animate version with nullptr animations
    DrawRectangle(brush, pen, rectangle, nullptr);
}

/// <summary>
///     DrawRectangle -
///     Draw a rectangle with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the rectangle.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the rectangle.
///     This is optional, and can be nullptr, in which case no stroke is performed.
/// </param>
/// <param name="rectangle"> The QRectF const & to fill and/or stroke. </param>
/// <param name="rectangleAnimations"> Optional AnimationClock * for rectangle. </param>
void DrawingDrawingContext::DrawingDrawingContext::DrawRectangle(
    QBrush brush,
    QPen pen,
    QRectF const & rectangle,
    AnimationClock * rectangleAnimations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawRectangle(animate)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //if ((brush == nullptr) && (pen == nullptr))
    //{
    //    return;
    //}

    //
    // Create a geometry & add animations if they exist
    //

    // Instantiate the geometry
    RectangleGeometry* geometry = new RectangleGeometry(rectangle);

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //geometry.CanBeInheritanceContext = CanBeInheritanceContext;

    // Setup the geometries freezable-related state
    //SetupNewFreezable(
    //    geometry,
    //    (rectangleAnimations == nullptr) // Freeze if there are no animations
    //    );

    // Add animations to the geometry
    if (rectangleAnimations != nullptr)
    {
        ////geometry.ApplyAnimationClock(RectangleGeometry.RectProperty, rectangleAnimations);
    }

    //
    // Add Drawing to the Drawing graph
    //

    AddNewGeometryDrawing(brush, pen, geometry);
}

/// <summary>
///     DrawRoundedRectangle -
///     Draw a rounded rectangle with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the rectangle.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the rectangle.
///     This is optional, and can be nullptr, in which case no stroke is performed.
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
void DrawingDrawingContext::DrawingDrawingContext::DrawRoundedRectangle(
    QBrush brush,
    QPen pen,
    QRectF const & rectangle,
    double radiusX,
    double radiusY)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawRoundedRectangle(const)");
//#endif

    // Forward call to the animate version with nullptr animations
    DrawRoundedRectangle(brush, pen, rectangle, nullptr, radiusX, nullptr, radiusY, nullptr);
}

/// <summary>
///     DrawRoundedRectangle -
///     Draw a rounded rectangle with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the rectangle.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the rectangle.
///     This is optional, and can be nullptr, in which case no stroke is performed.
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
void DrawingDrawingContext::DrawingDrawingContext::DrawRoundedRectangle(
    QBrush brush,
    QPen pen,
    QRectF const & rectangle,
    AnimationClock * rectangleAnimations,
    double radiusX,
    AnimationClock * radiusXAnimations,
    double radiusY,
    AnimationClock * radiusYAnimations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawRoundedRectangle(animate)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //if ((brush == nullptr) && (pen == nullptr))
    //{
    //    return;
    //}

    //
    // Create a geometry & add animations if they exist
    //

    // Instantiate the geometry
    RectangleGeometry* geometry = new RectangleGeometry(rectangle, radiusX, radiusY);

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //geometry.CanBeInheritanceContext = CanBeInheritanceContext;

    // Setup the geometries freezable-related state
    //SetupNewFreezable(
    //    geometry,
    //    (rectangleAnimations == nullptr) &&    // Freeze if animations are nullptr
    //    (radiusXAnimations == nullptr) &&
    //    (radiusYAnimations == nullptr)
    //    );

    // Add animations to the geometry
    if (rectangleAnimations != nullptr)
    {
        ////geometry.ApplyAnimationClock(RectangleGeometry.RectProperty, rectangleAnimations);
    }

    if (radiusXAnimations != nullptr)
    {
        ////geometry.ApplyAnimationClock(RectangleGeometry.RadiusXProperty, radiusXAnimations);
    }

    if (radiusYAnimations != nullptr)
    {
        ////geometry.ApplyAnimationClock(RectangleGeometry.RadiusYProperty, radiusYAnimations);
    }

    //
    // Add Drawing to the Drawing graph
    //

    AddNewGeometryDrawing(brush, pen, geometry);
}

/// <summary>
///     DrawEllipse -
///     Draw an ellipse with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the ellipse.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the ellipse.
///     This is optional, and can be nullptr, in which case no stroke is performed.
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
void DrawingDrawingContext::DrawingDrawingContext::DrawEllipse(
    QBrush brush,
    QPen pen,
    QPointF const & center,
    double radiusX,
    double radiusY)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawEllipse(const)");
//#endif

    // Forward call to the animate version with nullptr animations
    DrawEllipse(brush, pen, center, nullptr, radiusX, nullptr, radiusY, nullptr);
}

/// <summary>
///     DrawEllipse -
///     Draw an ellipse with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the ellipse.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the ellipse.
///     This is optional, and can be nullptr, in which case no stroke is performed.
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
void DrawingDrawingContext::DrawingDrawingContext::DrawEllipse(
    QBrush brush,
    QPen pen,
    QPointF const & center,
    AnimationClock * centerAnimations,
    double radiusX,
    AnimationClock * radiusXAnimations,
    double radiusY,
    AnimationClock * radiusYAnimations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawEllipse(animate)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //if ((brush == nullptr) && (pen == nullptr))
    //{
    //    return;
    //}

    //
    // Create a geometry & add animations if they exist
    //

    // Instantiate the geometry
    EllipseGeometry* geometry = new EllipseGeometry(center, radiusX, radiusY);

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //geometry.CanBeInheritanceContext = CanBeInheritanceContext;

    // Setup the geometries freezable-related state
    //SetupNewFreezable(
    //    geometry,
    //    (centerAnimations == nullptr) && // Freeze if there are no animations
    //    (radiusXAnimations == nullptr) &&
    //    (radiusYAnimations == nullptr)
    //    );

    // Add animations to the geometry
    if (centerAnimations != nullptr)
    {
        //geometry.ApplyAnimationClock(EllipseGeometry.CenterProperty, centerAnimations);
    }

    if (radiusXAnimations != nullptr)
    {
        //geometry.ApplyAnimationClock(EllipseGeometry.RadiusXProperty, radiusXAnimations);
    }

    if (radiusYAnimations != nullptr)
    {
        //geometry.ApplyAnimationClock(EllipseGeometry.RadiusYProperty, radiusYAnimations);
    }

    //
    // Add Drawing to the Drawing graph
    //

    AddNewGeometryDrawing(brush, pen, geometry);
}
/// <summary>
///     DrawGeometry -
///     Draw a Geometry with the provided QBrush and/or Pen.
///     If both the QBrush and QPen are nullptr this call is a no-op.
/// </summary>
/// <param name="brush">
///     The QBrush with which to fill the Geometry.
///     This is optional, and can be nullptr, in which case no fill is performed.
/// </param>
/// <param name="pen">
///     The QPen with which to stroke the Geometry.
///     This is optional, and can be nullptr, in which case no stroke is performed.
/// </param>
/// <param name="geometry"> The Geometry to fill and/or stroke. </param>
void DrawingDrawingContext::DrawingDrawingContext::DrawGeometry(
    QBrush brush,
    QPen pen,
    Geometry* geometry)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawGeometry(const)");
//#endif

    //
    // Create a drawing & add animations if they exist
    //

    VerifyApiNonstructuralChange();

    //if (((brush == nullptr) && (pen == nullptr)) || (geometry == nullptr))
    //{
    //    return;
    //}

    AddNewGeometryDrawing(brush, pen, geometry);
}

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
/*
void DrawingDrawingContext::DrawingDrawingContext::DrawImage(
    ImageSource imageSource,
    QRectF const & rectangle)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawImage(const)");
//#endif

    // Forward call to the animate version with nullptr animations
    DrawImage(imageSource, rectangle, nullptr);
}

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
void DrawingDrawingContext::DrawingDrawingContext::DrawImage(
    ImageSource imageSource,
    QRectF const & rectangle,
    AnimationClock * rectangleAnimations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawImage(animate)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    if (imageSource == nullptr)
    {
        return;
    }

    //
    // Create a drawing & add animations if they exist
    //

    ImageDrawing imageDrawing = new ImageDrawing();

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    imageDrawing.CanBeInheritanceContext = CanBeInheritanceContext;

    imageDrawing.ImageSource = imageSource;
    imageDrawing.Rect = rectangle;

    SetupNewFreezable(
        imageDrawing,
        (null == rectangleAnimations) && // Freeze if there are no animations
        imageSource.IsFrozen            // and the bitmap source is frozen
        );

    if (rectangleAnimations != nullptr)
    {
        imageDrawing.ApplyAnimationClock(ImageDrawing.RectProperty, rectangleAnimations);
    }

    AddDrawing(imageDrawing);
}
*/


/// <summary>
///     DrawImage -
///     Draw an Image into the region specified by the Rect.
///     The Image will potentially be stretched and distorted to fit the Rect.
///     For more fine grained control, consider filling a Rect with an ImageBrush via
///     DrawRectangle.
/// </summary>
/// <param name="imageSource"> The ImageSource to draw. </param>
/// <param name="rectangle">
///     The Rect into which the ImageSource will be fit.
/// </param>
void DrawingDrawingContext::DrawImage(
    QPixmap imageSource,
    QRectF const & rectangle)
{

#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawImage(const)");
#endif

    // Forward call to the animate version with null animations
    DrawImage(imageSource, rectangle, nullptr);
}

/// <summary>
///     DrawImage -
///     Draw an Image into the region specified by the Rect.
///     The Image will potentially be stretched and distorted to fit the Rect.
///     For more fine grained control, consider filling a Rect with an ImageBrush via
///     DrawRectangle.
/// </summary>
/// <param name="imageSource"> The ImageSource to draw. </param>
/// <param name="rectangle">
///     The Rect into which the ImageSource will be fit.
/// </param>
/// <param name="rectangleAnimations"> Optional AnimationClock for rectangle. </param>
void DrawingDrawingContext::DrawImage(
    QPixmap imageSource,
    QRectF const & rectangle,
    AnimationClock * rectangleAnimations)
{

#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawImage(animate)");
#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //if (imageSource == nullptr)
    //{
    //    return;
    //}

    //
    // Create a drawing & add animations if they exist
    //

    ImageDrawing* imageDrawing = new ImageDrawing();

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //imageDrawing.CanBeInheritanceContext = CanBeInheritanceContext;

    imageDrawing->SetImageSource(imageSource);
    imageDrawing->SetRect(rectangle);

    //SetupNewFreezable(
    //    imageDrawing,
    //    (null == rectangleAnimations) && // Freeze if there are no animations
    //    imageSource.IsFrozen            // and the bitmap source is frozen
    //    );

    if (rectangleAnimations != nullptr)
    {
        //imageDrawing.ApplyAnimationClock(ImageDrawing.RectProperty, rectangleAnimations);
    }

    AddDrawing(imageDrawing);
}

/// <summary>
///     DrawDrawing -
///     Draw a Drawing.
///     For more fine grained control, consider filling a QRectF const & with an DrawingBrush via
///     DrawRect.
/// </summary>
/// <param name="drawing"> The Drawing to draw. </param>
void DrawingDrawingContext::DrawingDrawingContext::DrawDrawing(
    Drawing* drawing)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawDrawing(const)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    if (drawing == nullptr)
    {
        return;
    }

    //
    // Add the Drawing to the children collection
    //

    AddDrawing(drawing);
}

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
/*
void DrawingDrawingContext::DrawingDrawingContext::DrawVideo(
    MediaPlayer player,
    QRectF const & rectangle)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawVideo(const)");
//#endif

    // Forward to non-animate version with nullptr animations
    DrawVideo(player, rectangle, nullptr);
}

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
void DrawingDrawingContext::DrawingDrawingContext::DrawVideo(
    MediaPlayer player,
    QRectF const & rectangle,
    AnimationClock * rectangleAnimations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawVideo(animate)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    if (player == nullptr)
    {
        return;
    }

    //
    // Create a drawing & add animations if they exist
    //

    VideoDrawing videoDrawing = new VideoDrawing();

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    videoDrawing.CanBeInheritanceContext = CanBeInheritanceContext;

    videoDrawing.Player = player;
    videoDrawing.Rect = rectangle;

    SetupNewFreezable(
        videoDrawing,
        false   // Don't ever freeze a VideoDrawing because it would
                // lose it's MediaPlayer
        );

    if (rectangleAnimations != nullptr)
    {
        videoDrawing.ApplyAnimationClock(VideoDrawing.RectProperty, rectangleAnimations);
    }

    AddDrawing(videoDrawing);
}
*/
/// <summary>
///     PushClip -
///     Push a clip region, which will apply to all drawing primitives until the
///     corresponding Pop call.
/// </summary>
/// <param name="clipGeometry"> The Geometry to which we will clip. </param>
void DrawingDrawingContext::DrawingDrawingContext::PushClip(
    Geometry * clipGeometry)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushClip(const)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Set the clip on the new DrawingGroup
    //

    //_currentDrawingGroup.ClipGeometry = clipGeometry;
}

void DrawingDrawingContext::DrawingDrawingContext::PushOpacityMask(QBrush brush)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushOpacityMask(const)");
//#endif

    //
    // Verify this object's state
    //
    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //
    PushNewDrawingGroup();

    //
    // Set the opacity mask
    //
    //_currentDrawingGroup.OpacityMask = brush;
}

/// <summary>
///     PushOpacity -
///     Push an opacity which will blend the composite of all drawing primitives added
///     until the corresponding Pop call.
/// </summary>
/// <param name="opacity">
///     The opacity with which to blend - 0 is transparent, 1 is opaque.
/// </param>
void DrawingDrawingContext::DrawingDrawingContext::PushOpacity(
    double opacity
    )
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushOpacity(const)");
//#endif

    // Forward to the animate version with nullptr animations
    PushOpacity(opacity, nullptr);
}

/// <summary>
///     PushOpacity -
///     Push an opacity which will blend the composite of all drawing primitives added
///     until the corresponding Pop call.
/// </summary>
/// <param name="opacity">
///     The opacity with which to blend - 0 is transparent, 1 is opaque.
/// </param>
/// <param name="opacityAnimations"> Optional AnimationClock * for opacity. </param>
void DrawingDrawingContext::DrawingDrawingContext::PushOpacity(
    double opacity,
    AnimationClock * opacityAnimations)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushOpacity(animate)");
//#endif

    //
    // Verify this object's state
    //

    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Set the opacity & opacity animations on the new DrawingGroup
    //

    //_currentDrawingGroup.Opacity = opacity;

    if (nullptr != opacityAnimations)
    {
        //_currentDrawingGroup.ApplyAnimationClock(DrawingGroup.OpacityProperty, opacityAnimations);
    }
}
/// <summary>
///     PushTransform -
///     Push a Transform which will apply to all drawing operations until the corresponding
///     Pop.
/// </summary>
/// <param name="transform"> The Transform to push. </param>
void DrawingDrawingContext::DrawingDrawingContext::PushTransform(
    QTransform transform)
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushTransform(const)");
//#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Set the transform on the new DrawingGroup
    //

    //_currentDrawingGroup.Transform = transform;
}

/// <summary>
///     PushGuidelineSet -
///     Push a set of guidelines which should be applied
///     to all drawing operations until the
///     corresponding Pop.
/// </summary>
/// <param name="guidelines"> The GuidelineSet to push. </param>
/*
void DrawingDrawingContext::DrawingDrawingContext::PushGuidelineSet(
    GuidelineSet guidelines)
{

#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushGuidelineSet");
#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Set the guideline collection on the new DrawingGroup
    //

    _currentDrawingGroup.GuidelineSet = guidelines;
}
*/
/// <summary>
///     PushGuidelineY1 -
///     Explicitly push one horizontal guideline.
/// </summary>
/// <param name="coordinate"> The coordinate of leading guideline. </param>
/*
void DrawingDrawingContext::PushGuidelineY1(
    double coordinate)
{

#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushGuidelineY1");
#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Convert compact record to generic GuidelineSet.
    //

    GuidelineSet guidelineCollection = new GuidelineSet(
        nullptr,                           // x guidelines
        new double[] { coordinate, 0 }, // y guidelines
        true                            // dynamic flag
        );
    guidelineCollection.Freeze();

    //
    // Set the guideline collection on the new DrawingGroup
    //

    _currentDrawingGroup.GuidelineSet = guidelineCollection;
}

/// <summary>
///     PushGuidelineY2 -
///     Explicitly push a pair of horizontal guidelines.
/// </summary>
/// <param name="leadingCoordinate"> The coordinate of leading guideline. </param>
/// <param name="offsetToDrivenCoordinate">
///     The offset from leading guideline to driven guideline.
/// </param>
void DrawingDrawingContext::PushGuidelineY2(
    double leadingCoordinate,
    double offsetToDrivenCoordinate)
{

#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushGuidelineY2");
#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Convert compact record to generic GuidelineSet.
    //

    GuidelineSet guidelineCollection = new GuidelineSet(
        nullptr,                                   // x guidelines
        new double[]
            {
                leadingCoordinate,
                offsetToDrivenCoordinate
            }, // y guidelines
        true                                    // dynamic flag
        );
    guidelineCollection.Freeze();

    //
    // Set the guideline collection on the new DrawingGroup
    //

    _currentDrawingGroup.GuidelineSet = guidelineCollection;
}

/// <summary>
///     PushEffect -
///     Push a BitmapEffect which will apply to all drawing operations until the
///     corresponding Pop.
/// </summary>
/// <param name="effect"> The BitmapEffect to push. </param>
/// <param name="effectInput"> The BitmapEffectInput. </param>
[Obsolete(MS.Internal.Media.VisualTreeUtils.BitmapEffectObsoleteMessage)]
void DrawingDrawingContext::DrawingDrawingContext::PushEffect(
    BitmapEffect effect,
    BitmapEffectInput effectInput)
{

#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("PushEffect(const)");
#endif

    //
    // Verify that parameters & state are valid
    //



    VerifyApiNonstructuralChange();

    //
    // Instantiate a new drawing group and set it as the _currentDrawingGroup
    //

    PushNewDrawingGroup();

    //
    // Set the transform on the new DrawingGroup
    //

    // NOTE: Microsoft  Disabling this API for now

    _currentDrawingGroup.BitmapEffect = effect;
    _currentDrawingGroup.BitmapEffectInput = (effectInput != nullptr) ?
                                                effectInput : new BitmapEffectInput();

}
*/
/// <summary>
/// Pop
/// </summary>
void DrawingDrawingContext::DrawingDrawingContext::Pop()
{

//#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("Pop");
//#endif

    VerifyApiNonstructuralChange();

    // Verify that Pop hasn't been called too many times
    if ( _previousDrawingGroupStack.size() == 0)
    {
        throw std::exception("SR.Get(SRID.DrawingContext_TooManyPops)");
    }

    // Restore the previous value of the current drawing group
    _currentDrawingGroup = _previousDrawingGroupStack.back();
    _previousDrawingGroupStack.pop_back();
}

/// <summary>
/// Draw a GlyphRun.
/// </summary>
/// <param name="foregroundBrush">Foreground QBrush to draw GlyphRun with. </param>
/// <param name="glyphRun"> The GlyphRun to draw. </param>
/// <exception cref="ObjectDisposedException">
/// This call is illegal if this object has already been closed or disposed.
/// </exception>
/*
void DrawingDrawingContext::DrawingDrawingContext::DrawGlyphRun(Brush foregroundBrush, GlyphRun glyphRun)
{
#if DEBUG
    //MediaTrace.DrawingContextOp.Trace("DrawGlyphRun(constant)");
#endif

    //
    // Verify that parameters & state are valid
    //

    VerifyApiNonstructuralChange();

    if (foregroundBrush == nullptr || glyphRun == nullptr)
    {
        return;
    }

    // Add a GlyphRunDrawing to the Drawing graph

    GlyphRunDrawing glyphRunDrawing = new GlyphRunDrawing();

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    glyphRunDrawing.CanBeInheritanceContext = CanBeInheritanceContext;

    glyphRunDrawing.ForegroundBrush = foregroundBrush;
    glyphRunDrawing.GlyphRun = glyphRun;

    SetupNewFreezable(
        glyphRunDrawing,
        foregroundBrush.IsFrozen
        );

    AddDrawing(glyphRunDrawing);
}
*/
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
void DrawingDrawingContext::DrawingDrawingContext::Close()
{
    // Throw an exception if this object has already been closed/disposed.
    VerifyNotDisposed();

    // Close this object
    DisposeCore();
}

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
void DrawingDrawingContext::DrawingDrawingContext::DisposeCore()
{
    // Dispose may be called multiple times without throwing
    // an exception.
    if (!_disposed)
    {
        //
        // Match any outstanding Push calls with a Pop
        //

        //if (_previousDrawingGroupStack != nullptr)
        {
            int stackCount = _previousDrawingGroupStack.size();
            for (int i = 0; i < stackCount; i++)
            {
                Pop();
            }
        }

        //
        // Call CloseCore with the root DrawingGroup's children
        //

        QVector<Drawing*> rootChildren;

        if (_currentDrawingGroup != nullptr)
         {
            // If we created a root DrawingGroup because multiple elements
            // exist at the root level, provide it's Children collection
            // directly.
            rootChildren = _currentDrawingGroup->Children().toVector();
        }
        else
        {
            // Create a new DrawingCollection if we didn't create a
            // root DrawingGroup because the root level only contained
            // a single child.
            //
            // This collection is needed by DrawingGroup.Open because
            // Open always replaces it's Children collection.  It isn't
            // strictly needed for Append, but always using a collection
            // simplifies the TransactionalAppend implementation (i.e.,
            // a seperate implemention isn't needed for a single element)
            //rootChildren = new DrawingCollection();

            //
            // We may need to opt-out of inheritance through the new Freezable.
            // This is controlled by this.CanBeInheritanceContext.
            //

            //rootChildren.CanBeInheritanceContext = CanBeInheritanceContext;

            if (_rootDrawing != nullptr)
            {
                rootChildren.append(_rootDrawing);
            }
        }

        // Inform our derived classes that Close was called
        CloseCore(rootChildren);

        _disposed = true;
    }
}

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
void DrawingDrawingContext::DrawingDrawingContext::CloseCore(QVector<Drawing*> rootDrawingGroupChildren)
{
    // Default implementation is a no-op
}

/// <summary>
/// Verifies that the DrawingContext is being referenced from the
/// appropriate UIContext, and that the object hasn't been disposed
/// </summary>
void DrawingDrawingContext::DrawingDrawingContext::VerifyApiNonstructuralChange()
{
    //base.VerifyApiNonstructuralChange();

    VerifyNotDisposed();
}

//#endregion Protected Methods

//#region Internal Properties



//#endregion Internal Properties

//#region Methods

/// <summary>
/// Throws an exception if this object is already disposed.
/// </summary>
void DrawingDrawingContext::VerifyNotDisposed()
{
    if (_disposed)
    {
        throw std::exception("DrawingDrawingContext");
    }
}


/// <summary>
/// Contains the functionality common to GeometryDrawing operations of
/// instantiating the GeometryDrawing, setting it's Freezable state,
/// and Adding it to the Drawing Graph.
/// </summary>
void DrawingDrawingContext::AddNewGeometryDrawing(QBrush brush, QPen pen, Geometry* geometry)
{
    Debug::Assert(geometry != nullptr);

    // Instantiate the GeometryDrawing
    GeometryDrawing* geometryDrawing = new GeometryDrawing();

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //geometryDrawing->CanBeInheritanceContext = CanBeInheritanceContext;

    geometryDrawing->SetBrush(brush);
    geometryDrawing->SetPen(pen);
    geometryDrawing->SetGeometry(geometry);

    // Setup it's Freezeable-related state
    //SetupNewFreezable(
    //    geometryDrawing,
    //    ((brush == nullptr) || (brush.IsFrozen)) &&    // Freeze if the QBrush is frozen
    //    ((pen == nullptr) || (pen.IsFrozen)) &&        // and the QPen is frozen
    //    (geometry.IsFrozen)                         // and the geometry is frozen
    //    );

    // Add it to the drawing graph
    AddDrawing(geometryDrawing);
}

/// <summary>
/// Creates a new DrawingGroup for a Push* call by setting the
/// _currentDrawingGroup to a newly instantiated DrawingGroup,
/// and saving the previous _currentDrawingGroup value on the
/// _previousDrawingGroupStack.
/// </summary>
void DrawingDrawingContext::PushNewDrawingGroup()
{
    // Instantiate a new drawing group
    DrawingGroup* drawingGroup = new DrawingGroup();

    //
    // We may need to opt-out of inheritance through the new Freezable.
    // This is controlled by this.CanBeInheritanceContext.
    //

    //drawingGroup.CanBeInheritanceContext = CanBeInheritanceContext;

    // Setup it's freezable state
    //SetupNewFreezable(
    //        drawingGroup,
    //        false // Don't freeze, DrawingGroup's may contain unfrozen children/properties
    //        );

    // Add it to the drawing graph, like any other Drawing
    AddDrawing(drawingGroup);

    // Lazily allocate the stack when it is needed because many uses
    // of DrawingDrawingContext will have a depth of one.
    //if (null == _previousDrawingGroupStack)
    //{
    //    _previousDrawingGroupStack = new Stack<DrawingGroup>(2);
    //}

    // Save the previous _currentDrawingGroup value.
    //
    // If this is the first call, the value of _currentDrawingGroup
    // will be nullptr because AddDrawing doesn't create a _currentDrawingGroup
    // for the first drawing.  Having nullptr on the stack is valid, and simply
    // denotes that this new DrawingGroup is the first child in the root
    // DrawingGroup.  It is also possible for the first value on the stack
    // to be non-nullptr, which means that the root DrawingGroup has other
    // children.
    _previousDrawingGroupStack.push_back(_currentDrawingGroup);

    // Set this drawing group as the current one so that subsequent drawing's
    // are added as it's children until Pop is called.
    _currentDrawingGroup = drawingGroup;
}

/// <summary>
/// Adds a new Drawing to the DrawingGraph.
///
/// This method avoids creating a DrawingGroup for the common case
/// where only a single child exists in the root DrawingGroup.
/// </summary>
void DrawingDrawingContext::AddDrawing(Drawing* newDrawing)
{
    Debug::Assert(newDrawing != nullptr);

    if (_rootDrawing == nullptr)
    {
        // When a DrawingGroup is set, it should be made the root if
        // a root drawing didnt exist.
        Debug::Assert(_currentDrawingGroup == nullptr);

        // If this is the first Drawing being added, avoid DrawingDrawingContext::creating a DrawingGroup
        // and set this drawing as the root drawing.  This optimizes the common
        // case where only a single child exists in the root DrawingGroup.
        _rootDrawing = newDrawing;
    }
    else if (_currentDrawingGroup == nullptr)
    {
        // When the second drawing is added at the root level, set a
        // DrawingGroup as the root and add both drawings to it.

        // Instantiate the DrawingGroup
        _currentDrawingGroup = new DrawingGroup();

        //
        // We may need to opt-out of inheritance through the new Freezable.
        // This is controlled by this.CanBeInheritanceContext.
        //

        //_currentDrawingGroup.CanBeInheritanceContext = CanBeInheritanceContext;

        //SetupNewFreezable(
        //    _currentDrawingGroup,
        //    false // Don't freeze DrawingGroups, subsequent Draw calls may modify them
        //    );

        // Add both Children
        _currentDrawingGroup->Children().append(_rootDrawing);
        _currentDrawingGroup->Children().append(newDrawing);

        // Set the new DrawingGroup as the current
        _rootDrawing = _currentDrawingGroup;
    }
    else
    {
        // If there already is a current drawing group, then simply add
        // the new drawing too it.
        _currentDrawingGroup->Children().append(newDrawing);
    }
}

