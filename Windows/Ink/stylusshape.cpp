#include "Windows/Ink/stylusshape.h"
#include "Windows/Input/styluspoint.h"
#include "Internal/doubleutil.h"
#include "Windows/Ink/drawingattributes.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

StylusShape::StylusShape()
{

}

#if STROKE_COLLECTION_EDIT_MASK
StylusShape::StylusShape(QPolygonF const & polygon)
{
    m_vertices.resize(polygon.size() - 1);
    for (int i = 0; i < polygon.size() - 1; ++i)
        m_vertices[i] = polygon[i];
}
#endif

StylusShape::StylusShape(StylusTip tip, double width, double height, double rotation)
{
    if (Double::IsNaN(width) || Double::IsInfinity(width) || width < DrawingAttributes::MinWidth || width > DrawingAttributes::MaxWidth)
    {
        throw std::runtime_error("width");
    }

    if (Double::IsNaN(height) || Double::IsInfinity(height) || height < DrawingAttributes::MinHeight || height > DrawingAttributes::MaxHeight)
    {
        throw std::runtime_error("height");
    }

    if (Double::IsNaN(rotation) || Double::IsInfinity(rotation))
    {
        throw std::runtime_error("rotation");
    }

    if (!StylusTipHelper::IsDefined(tip))
    {
        throw std::runtime_error("tip");
    }


    //
    //  mod rotation to 360 (720 to 0, 361 to 1, -270 to 90)
    //
    m_width = width;
    m_height = height;
    m_rotation = rotation == 0 ? 0 : Mod(rotation, 360);
    m_tip = tip;
    if (tip == StylusTip::Rectangle)
    {
        ComputeRectangleVertices();
    }
}


Array<Vector> StylusShape::GetVerticesAsVectors()
{
    Array<Vector> vertices;

    if (!m_vertices.empty())
    {
        // For a Rectangle
        vertices.resize(m_vertices.Length());

        if (_transform.IsIdentity())
        {
            for (int i = 0; i < vertices.Length(); i++)
            {
                vertices[i] = m_vertices[i];
            }
        }
        else
        {
            for (int i = 0; i < vertices.Length(); i++)
            {
                vertices[i] = _transform.Transform(m_vertices[i]);
            }

            // A transform might make the vertices in counter-clockwise order
            // Fix it if this is the case.
            FixCounterClockwiseVertices(vertices);
        }

    }
    else
    {
        // For ellipse

        // The transform is already applied on these points.
        Array<Point> p = GetBezierControlPoints();
        vertices.resize(p.Length());
        for (int i = 0; i < vertices.Length(); i++)
        {
            vertices[i] = p[i];
        }
    }
    return vertices;
}

Rect StylusShape::BoundingBox()
{
    Rect bbox = Rect::Empty();

    if (IsPolygon())
    {
        for (Point vertex : m_vertices)
        {
            bbox.Union(vertex);
        }
    }
    //
    else //if (DoubleUtil.IsZero(m_rotation) || DoubleUtil.AreClose(m_width, m_height))
    {
        bbox = Rect(-(m_width * 0.5), -(m_height * 0.5), m_width, m_height);
    }
    //else
    //{
    //    throw new NotImplementedException("Rotated ellipse");
    //}

    return bbox;
}

void StylusShape::ComputeRectangleVertices()
{
    Point topLeft(-(m_width * 0.5), -(m_height * 0.5));
    m_vertices = Array<Point>( { topLeft,
                                topLeft + Vector(m_width, 0),
                                topLeft + Vector(m_width, m_height),
                                topLeft + Vector(0, m_height)});
    if (false == DoubleUtil::IsZero(m_rotation))
    {
        Matrix rotationTransform;
        rotationTransform.Rotate(m_rotation);
        for (Point & p : m_vertices)
            p = rotationTransform.Transform(p);
    }
}


/// <summary> A transform might make the vertices in counter-clockwise order Fix it if this is the case.</summary>
void StylusShape::FixCounterClockwiseVertices(Array<Vector> & vertices)
{
    // The method should only called for Rectangle case.
    Debug::Assert(vertices.Length() == 4);

    Point prevVertex = vertices[vertices.Length() - 1];
    int counterClockIndex = 0, clockWiseIndex = 0;

    for (int i = 0; i < vertices.Length(); i++)
    {
        Point vertex = vertices[i];
        Vector edge = vertex - prevVertex;

        // Verify that the next vertex is on the right side off the edge vector.
        double det = Vector::Determinant(edge, (Point)vertices[(i + 1) % vertices.Length()] - vertex);
        if (0 > det)
        {
            counterClockIndex++;
        }
        else if (0 < det)
        {
            clockWiseIndex++;
        }

        prevVertex = vertex;
    }

    // Assert the transform will make it either clockwise or counter-clockwise.
    Debug::Assert(clockWiseIndex == vertices.Length() || counterClockIndex == vertices.Length());

    if (counterClockIndex == vertices.Length())
    {
        // Make it Clockwise
        int lastIndex = vertices.Length() -1;
        for (int j = 0; j < vertices.Length()/2; j++)
        {
            Point tmp = vertices[j];
            vertices[j] = vertices[lastIndex - j];
            vertices[lastIndex-j] = tmp;
        }
    }
}


Array<Point> StylusShape::GetBezierControlPoints()
{
    Debug::Assert(m_tip == StylusTip::Ellipse);

    // Approximating a 1/4 circle with a Bezier curve (borrowed from Avalon's EllipseGeometry.cs)
    const double ArcAsBezier = 0.5522847498307933984; // =(\/2 - 1)*4/3

    double radiusX = m_width / 2;
    double radiusY = m_height / 2;
    double borderMagicX = radiusX * ArcAsBezier;
    double borderMagicY = radiusY * ArcAsBezier;

    Array<Point> controlPoints = {
        Point(    -radiusX, -borderMagicY),
        Point(-borderMagicX,     -radiusY),
        Point(            0,     -radiusY),
        Point( borderMagicX,     -radiusY),
        Point(     radiusX, -borderMagicY),
        Point(     radiusX,             0),
        Point(     radiusX,  borderMagicY),
        Point( borderMagicX,      radiusY),
        Point(            0,      radiusY),
        Point(-borderMagicX,      radiusY),
        Point(    -radiusX,  borderMagicY),
        Point(    -radiusX,             0)};

    //

    Matrix transform;
    if (m_rotation != 0)
    {
        transform.Rotate(m_rotation);
    }

    if (_transform.IsIdentity() == false)
    {
        transform *= _transform;
    }

    if (transform.IsIdentity() == false)
    {
        for (int i = 0; i < controlPoints.Length(); i++)
        {
            controlPoints[i] = transform.Transform(controlPoints[i]);
        }
    }

    return controlPoints;
}

INKCANVAS_END_NAMESPACE
