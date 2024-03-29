#ifndef WINDOWS_INK_STYLUSSHAPE_H
#define WINDOWS_INK_STYLUSSHAPE_H

#include "InkCanvas_global.h"

#include "Windows/Ink/stylustip.h"
#include "Collections/Generic/array.h"
#include "Windows/Media/matrix.h"

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

///<summary>
/// Defines the style of pen tip for rendering.
///</summary>
/// <remarks>
/// The Stylus size and coordinates are in units equal to 1/96th of an inch.
/// The default in V1 the default width is 1 pixel. This is 53 himetric units.
/// There are 2540 himetric units per inch.
/// This means that 53 high metric units is equivalent to 53/2540*96 in avalon.
/// </remarks>
class INKCANVAS_EXPORT StylusShape
{
private:
    double    m_width;
    double    m_height;
    double    m_rotation;
    Array<Point>   m_vertices;
    StylusTip m_tip;
    Matrix    _transform;


public:
    StylusShape();

#if STROKE_COLLECTION_EDIT_MASK
    StylusShape(QPolygonF const & polygon);
#endif

    ///<summary>
    /// constructor for a StylusShape.
    ///</summary>
    StylusShape(StylusTip tip, double width, double height, double rotation);


    ///<summary>
    /// Width of the non-rotated shape.
    ///</summary>
    double Width() { return m_width; }

    ///<summary>
    /// Height of the non-rotated shape.
    ///</summary>
    double Height() { return m_height; }

    ///<summary>
    /// The shape's rotation angle. The rotation is done about the origin (0,0).
    ///</summary>
    double Rotation() { return m_rotation; }

    /// <summary>
    /// GetVerticesAsVectors
    /// </summary>
    /// <returns></returns>
    Array<Vector> GetVerticesAsVectors();


    /// <summary>
    /// This is the transform on the StylusShape
    /// </summary>
    Matrix Transform()
    {
        return _transform;
    }
    void SetTransform(Matrix const & value)
    {
        //System.Diagnostics.Debug.Assert(value.HasInverse);
        _transform = value;
    }

    ///<summary>
    /// A helper property.
    ///</summary>
    bool IsEllipse() { return m_vertices.empty(); }

    ///<summary>
    /// A helper property.
    ///</summary>
    bool IsPolygon() { return !m_vertices.empty(); }

    /// <summary>
    /// Generally, there's no need for the shape's bounding box.
    /// We use it to approximate v2 shapes with a rectangle for v1.
    /// </summary>
    Rect BoundingBox();

    /// <summary>TBS</summary>
    void ComputeRectangleVertices();


    /// <summary> A transform might make the vertices in counter-clockwise order Fix it if this is the case.</summary>
    void FixCounterClockwiseVertices(Array<Vector> & vertices);


    Array<Point> GetBezierControlPoints();

};

/// <summary>
/// Class for an elliptical StylusShape
/// </summary>
class EllipseStylusShape : public StylusShape
{
public:
    /// <summary>
    /// Constructor for an elliptical StylusShape
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    EllipseStylusShape(double width, double height)
            :EllipseStylusShape(width, height, 0.0)
    {
    }

    /// <summary>
    /// Constructor for an ellptical StylusShape ,with roation in degree
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="rotation"></param>
    EllipseStylusShape(double width, double height, double rotation)
                        : StylusShape(StylusTip::Ellipse, width, height, rotation)
    {
    }

};

/// <summary>
/// Class for a rectangle StylusShape
/// </summary>
class RectangleStylusShape : public StylusShape
{
public:
    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    RectangleStylusShape(double width, double height)
                            : RectangleStylusShape(width, height, 0)
    {
    }

    /// <summary>
    /// Constructor with rogation in degree
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="rotation"></param>
    RectangleStylusShape(double width, double height, double rotation)
                                : StylusShape(StylusTip::Rectangle, width, height, rotation)
    {
    }

};

INKCANVAS_END_NAMESPACE

#endif // WINDOWS_INK_STYLUSSHAPE_H
