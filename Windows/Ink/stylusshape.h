#ifndef STYLUSSHAPE_H
#define STYLUSSHAPE_H

#include "InkCanvas_global.h"

#include "stylustip.h"

#include <QMatrix>
#include <QPointF>
#include <QObject>

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
class INKCANVAS_EXPORT StylusShape : public QObject
{
    Q_OBJECT
private:
    double    m_width;
    double    m_height;
    double    m_rotation;
    QVector<QPointF>   m_vertices;
    StylusTip m_tip;
    QMatrix    _transform;


public:
    StylusShape();

    StylusShape(QPolygonF const & polygon);

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
    QVector<QPointF> GetVerticesAsVectors();


    /// <summary>
    /// This is the transform on the StylusShape
    /// </summary>
    QMatrix Transform()
    {
        return _transform;
    }
    void SetTransform(QMatrix const & value)
    {
        //System.Diagnostics.Debug.Assert(value.HasInverse);
        _transform = value;
    }

    ///<summary>
    /// A helper property.
    ///</summary>
    bool IsEllipse() { return m_vertices.isEmpty(); }

    ///<summary>
    /// A helper property.
    ///</summary>
    bool IsPolygon() { return !m_vertices.isEmpty(); }

    /// <summary>
    /// Generally, there's no need for the shape's bounding box.
    /// We use it to approximate v2 shapes with a rectangle for v1.
    /// </summary>
    QRectF BoundingBox();

    /// <summary>TBS</summary>
    void ComputeRectangleVertices();


    /// <summary> A transform might make the vertices in counter-clockwise order Fix it if this is the case.</summary>
    void FixCounterClockwiseVertices(QVector<QPointF> & vertices);


    QVector<QPointF> GetBezierControlPoints();

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

#endif // STYLUSSHAPE_H
