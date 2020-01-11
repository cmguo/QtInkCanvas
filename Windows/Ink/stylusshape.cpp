#include "Windows/Ink/stylusshape.h"
#include "Windows/Input/styluspoint.h"
#include "Internal/doubleutil.h"
#include "Windows/Ink/drawingattributes.h"
#include "Internal/debug.h"

StylusShape::StylusShape()
{

}

StylusShape::StylusShape(QPolygonF polygon)
{
    m_vertices = polygon;
}

StylusShape::StylusShape(StylusTip tip, double width, double height, double rotation)
{
    if (qIsNaN(width) || qIsInf(width) || width < DrawingAttributes::MinWidth || width > DrawingAttributes::MaxWidth)
    {
        throw std::exception("width");
    }

    if (qIsNaN(height) || qIsInf(height) || height < DrawingAttributes::MinHeight || height > DrawingAttributes::MaxHeight)
    {
        throw std::exception("height");
    }

    if (qIsNaN(rotation) || qIsInf(rotation))
    {
        throw std::exception("rotation");
    }

    if (!StylusTipHelper::IsDefined(tip))
    {
        throw std::exception("tip");
    }


    //
    //  mod rotation to 360 (720 to 0, 361 to 1, -270 to 90)
    //
    m_width = width;
    m_height = height;
    m_rotation = qIsNull(rotation) ? 0 : (rotation - floor(rotation / 360) * 360);
    m_tip = tip;
    if (tip == StylusTip::Rectangle)
    {
        ComputeRectangleVertices();
    }
}


QVector<QPointF> StylusShape::GetVerticesAsVectors()
{
    QVector<QPointF> vertices;

    if (!m_vertices.isEmpty())
    {
        // For a Rectangle
        vertices.resize(m_vertices.size());

        if (_transform.isIdentity())
        {
            for (int i = 0; i < vertices.size(); i++)
            {
                vertices[i] = m_vertices[i];
            }
        }
        else
        {
            for (int i = 0; i < vertices.size(); i++)
            {
                vertices[i] = _transform.map(m_vertices[i]);
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
        QVector<QPointF> p = GetBezierControlPoints();
        vertices.resize(p.size());
        for (int i = 0; i < vertices.size(); i++)
        {
            vertices[i] = p[i];
        }
    }
    return vertices;
}

QRectF StylusShape::BoundingBox()
{
    QRectF bbox;

    if (IsPolygon())
    {
        for (QPointF vertex : m_vertices)
        {
            bbox = United(bbox, vertex);
        }
    }
    //
    else //if (DoubleUtil.IsZero(m_rotation) || DoubleUtil.AreClose(m_width, m_height))
    {
        bbox = QRectF(-(m_width * 0.5), -(m_height * 0.5), m_width, m_height);
    }
    //else
    //{
    //    throw new NotImplementedException("Rotated ellipse");
    //}

    return bbox;
}

void StylusShape::ComputeRectangleVertices()
{
    QPointF topLeft(-(m_width * 0.5), -(m_height * 0.5));
    m_vertices = QVector<QPointF>( { topLeft,
                                topLeft + QPointF(m_width, 0),
                                topLeft + QPointF(m_width, m_height),
                                topLeft + QPointF(0, m_height)});
    if (false == DoubleUtil::IsZero(m_rotation))
    {
        QMatrix rotationTransform;
        rotationTransform.rotate(m_rotation);
        for (QPointF & p : m_vertices)
            p = rotationTransform.map(p);
    }
}


/// <summary> A transform might make the vertices in counter-clockwise order Fix it if this is the case.</summary>
void StylusShape::FixCounterClockwiseVertices(QVector<QPointF> & vertices)
{
    // The method should only called for Rectangle case.
    Debug::Assert(vertices.size() == 4);

    QPointF prevVertex = vertices[vertices.size() - 1];
    int counterClockIndex = 0, clockWiseIndex = 0;

    for (int i = 0; i < vertices.size(); i++)
    {
        QPointF vertex = vertices[i];
        QPointF edge = vertex - prevVertex;

        // Verify that the next vertex is on the right side off the edge vector.
        double det = Determinant(edge, vertices[(i + 1) % vertices.size()] - vertex);
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
    Debug::Assert(clockWiseIndex == vertices.size() || counterClockIndex == vertices.size());

    if (counterClockIndex == vertices.size())
    {
        // Make it Clockwise
        int lastIndex = vertices.size() -1;
        for (int j = 0; j < vertices.size()/2; j++)
        {
            QPointF tmp = vertices[j];
            vertices[j] = vertices[lastIndex - j];
            vertices[lastIndex-j] = tmp;
        }
    }
}


QVector<QPointF> StylusShape::GetBezierControlPoints()
{
    Debug::Assert(m_tip == StylusTip::Ellipse);

    // Approximating a 1/4 circle with a Bezier curve (borrowed from Avalon's EllipseGeometry.cs)
    const double ArcAsBezier = 0.5522847498307933984; // =(\/2 - 1)*4/3

    double radiusX = m_width / 2;
    double radiusY = m_height / 2;
    double borderMagicX = radiusX * ArcAsBezier;
    double borderMagicY = radiusY * ArcAsBezier;

    QVector<QPointF> controlPoints = {
        QPointF(    -radiusX, -borderMagicY),
        QPointF(-borderMagicX,     -radiusY),
        QPointF(            0,     -radiusY),
        QPointF( borderMagicX,     -radiusY),
        QPointF(     radiusX, -borderMagicY),
        QPointF(     radiusX,             0),
        QPointF(     radiusX,  borderMagicY),
        QPointF( borderMagicX,      radiusY),
        QPointF(            0,      radiusY),
        QPointF(-borderMagicX,      radiusY),
        QPointF(    -radiusX,  borderMagicY),
        QPointF(    -radiusX,             0)};

    //

    QMatrix transform;
    if (!qIsNull(m_rotation))
    {
        transform.rotate(m_rotation);
    }

    if (_transform.isIdentity() == false)
    {
        transform *= _transform;
    }

    if (transform.isIdentity() == false)
    {
        for (int i = 0; i < controlPoints.size(); i++)
        {
            controlPoints[i] = transform.map(controlPoints[i]);
        }
    }

    return controlPoints;
}
