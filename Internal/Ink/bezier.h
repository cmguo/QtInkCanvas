#ifndef BEZIER_H
#define BEZIER_H

#include "Windows/Input/styluspointcollection.h"

#include <QList>
#include <QPointF>

INKCANVAS_BEGIN_NAMESPACE

class CuspData;

// namespace MS.Internal.Ink

class Bezier
{
public:
    Bezier();


    /// <summary>
    /// Construct bezier control points from points
    /// </summary>
    /// <param name="stylusPoints">Original StylusPointCollection</param>
    /// <param name="fitError">Fitting error</param>
    /// <returns>Whether the algorithm succeeded</returns>
    bool ConstructBezierState(StylusPointCollection & stylusPoints, double fitError);

    /// <summary>
    /// Flatten bezier with a given resolution
    /// </summary>
    /// <param name="tolerance">tolerance</param>
    QList<QPointF> Flatten(double tolerance);


    /// <summary>
    /// Extend the current bezier segment if possible
    /// </summary>
    /// <param name="error">Fitting error sqaure</param>
    /// <param name="data">Data points</param>
    /// <param name="from">Starting index</param>
    /// <param name="next_cusp">NExt cusp index</param>
    /// <param name="to">Index of the last index, updated here</param>
    /// <param name="cusp">Whether there is a cusp at the end</param>
    /// <param name="done">Whether end of the stroke is reached</param>
    /// <returns>Whether the the segment was extended</returns>
    bool ExtendingRange(double error, CuspData & data, int from, int next_cusp, int & to, bool & cusp, bool & done);


    /// <summary>
    /// Add a bezier segment to the bezier buffer
    /// </summary>
    /// <param name="data">In: Data points</param>
    /// <param name="from">In: Index of the first point</param>
    /// <param name="tanStart">In: Unit tangent vector at the start</param>
    /// <param name="to">In: Index of the last point, updated here</param>
    /// <param name="tanEnd">In: Unit tangent vector at the end</param>
    /// <returns>True if the segment was added</returns>
    bool AddBezierSegment(CuspData & data, int from, QPointF & tanStart, int to, QPointF & tanEnd);


    /// <summary>
    /// Construct bezier curve from data points
    /// </summary>
    /// <param name="data">In: Data points</param>
    /// <param name="fitError">In: tolerated error</param>
    /// <returns>Whether bezier construction is possible</returns>
    bool ConstructFromData(CuspData & data, double fitError);


    /// <summary>
    /// Add parabola to the bezier
    /// </summary>
    /// <param name="data">In: Data points</param>
    /// <param name="from">In: The index of the parabola's first point</param>
    void AddParabola(CuspData & data, int from);


    /// <summary>
    /// Add Line to the bezier
    /// </summary>
    /// <param name="data">In: Data points</param>
    /// <param name="from">In: The index of the line's first point</param>
    /// <param name="to">In: The index of the line's last point</param>
    void AddLine(CuspData & data, int from, int to);


    /// <summary>
    /// Add least square fit curve to the bezier
    /// </summary>
    /// <param name="data">In: Data points</param>
    /// <param name="from">In: Index of the first point</param>
    /// <param name="V">In: Unit tangent vector at the start</param>
    /// <param name="to">In: Index of the last point, updated here</param>
    /// <param name="W">In: Unit tangent vector at the end</param>
    /// <returns>Return true segment added</returns>
    bool AddLeastSquares(CuspData & data, int from, QPointF & V, int to, QPointF & W);


    /// <summary>
    /// Checks whether five points are co-cubic within tolerance
    /// </summary>
    /// <param name="data">In: Data points</param>
    /// <param name="i">In: Array of 5 indices</param>
    /// <param name="fitError">In: tolerated error - squared</param>
    /// <returns>Return true if extended</returns>
    static bool CoCubic(CuspData & data, int i[], double fitError);

    /// <summary>
    /// Add Bezier point to the output buffer
    /// </summary>
    /// <param name="point">In: The point to add</param>
    void AddBezierPoint(QPointF const & point);


    /// <summary>
    /// Add segment point
    /// </summary>
    /// <param name="data">In: Interpolation data</param>
    /// <param name="index">In: The index of the point to add</param>
    void AddSegmentPoint(CuspData & data, int index);

    /// <summary>
    /// Evaluate on a Bezier segment a point at a given parameter
    /// </summary>
    /// <param name="iFirst">Index of Bezier segment's first point</param>
    /// <param name="t">Parameter value t</param>
    /// <returns>Return the point at parameter t on the curve</returns>
    QPointF DeCasteljau(int iFirst, double t);

    /// <summary>
    ///  Flatten a Bezier segment within given resolution
    /// </summary>
    /// <param name="iFirst">Index of Bezier segment's first point</param>
    /// <param name="tolerance">tolerance</param>
    /// <param name="points"></param>
    /// <returns></returns>
    void FlattenSegment(int iFirst, double tolerance, QList<QPointF> & points);

    /// <summary>
    /// Returns a single bezier control point at index
    /// </summary>
    /// <param name="index">Index</param>
    /// <returns></returns>
    QPointF GetBezierPoint(int index)
    {
        return _bezierControlPoints[index];
    }


    /// <summary>
    /// Count of bezier control points
    /// </summary>
    int BezierPointCount()
    {
        return _bezierControlPoints.size();
    }

    // Bezier points
private:
    QList<QPointF> _bezierControlPoints;
};

INKCANVAS_END_NAMESPACE

#endif // BEZIER_H
