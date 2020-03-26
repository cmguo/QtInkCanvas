#ifndef CUSPDATA_H
#define CUSPDATA_H

#include <QList>
#include <QPointF>

class StylusPointCollection;

// namespace MS.Internal.Ink

class CuspData
{
public:
    CuspData();
    /// <summary>
    /// Constructs internal data structure from points for doing operations like
    /// cusp detection or tangent computation
    /// </summary>
    /// <param name="stylusPoints">Points to be analyzed</param>
    /// <param name="rSpan">Distance between two consecutive distinct points</param>
    void Analyze(StylusPointCollection & stylusPoints, double rSpan);

    /// <summary>
    /// Set links amongst the points for tangent computation
    /// </summary>
    /// <param name="rError">Shortest distance between two distinct points</param>
    void SetTanLinks(double rError);


    /// <summary>
    /// Return the Index of the next cusp or the
    /// Index of the last StylusPoint if no cusp was found
    /// </summary>
    /// <param name="iCurrent">Current StylusPoint Index</param>
    /// <returns>Index into CuspData object for the next cusp </returns>
    int GetNextCusp(int iCurrent);

    /// <summary>
    /// Point at Index i into the cusp data structure
    /// </summary>
    /// <param name="i">Index</param>
    /// <returns>StylusPoint</returns>
    /// <remarks>The Index is within the bounds</remarks>
    QPointF const & XY(int i) const
    {
        return _points[i].Point;
    }


    /// <summary>
    /// Number of points in the internal data structure
    /// </summary>
    int Count() const
    {
        return _points.size();
    }


    /// <summary>
    /// Returns the chord length of the i-th StylusPoint from start of the stroke
    /// </summary>
    /// <param name="i">StylusPoint Index</param>
    /// <returns>distance</returns>
    /// <remarks>The Index is within the bounds</remarks>
    double Node(int i) const
    {
        return _nodes[i];
    }


    /// <summary>
    /// Returns the Index into original points given an Index into cusp data
    /// </summary>
    /// <param name="nodeIndex">Cusp data Index</param>
    /// <returns>Original StylusPoint Index</returns>
    int GetPointIndex(int nodeIndex) const
    {
        return _points[nodeIndex].Index;
    }


    /// <summary>
    /// Distance
    /// </summary>
    /// <returns>distance</returns>
    double Distance() const
    {
        return _dist;
    }


    /// <summary>
    /// Finds the approximante tangent at a given StylusPoint
    /// </summary>
    /// <param name="ptT">Tangent vector</param>
    /// <param name="nAt">Index at which the tangent is calculated</param>
    /// <param name="nPrevCusp">Index of the previous cusp</param>
    /// <param name="nNextCusp">Index of the next cusp</param>
    /// <param name="bReverse">Forward or reverse tangent</param>
    /// <param name="bIsCusp">Whether the current idex is a cusp StylusPoint</param>
    /// <returns>Return whether the tangent computation succeeded</returns>
    bool Tangent(QPointF & ptT, int nAt, int nPrevCusp, int nNextCusp, bool bReverse, bool bIsCusp);

    /// <summary>
    /// This "curvature" is not the theoretical curvature.  it is a number between
    /// 0 and 2 that is defined as 1 - cos(angle between segments) at this StylusPoint.
    /// </summary>
    /// <param name="iPrev">Previous data StylusPoint Index</param>
    /// <param name="iCurrent">Current data StylusPoint Index </param>
    /// <param name="iNext">Next data StylusPoint Index</param>
    /// <returns>"Curvature"</returns>
    double GetCurvature(int iPrev, int iCurrent, int iNext);


    /// <summary>
    /// Find all cusps for the stroke
    /// </summary>
    void FindAllCusps();


    /// <summary>
    /// Finds the next and previous data StylusPoint Index for the given data Index
    /// </summary>
    /// <param name="iPoint">Index at which the computation is performed</param>
    /// <param name="iPrevCusp">Previous cusp</param>
    /// <param name="iPrev">Previous data Index</param>
    /// <param name="iNext">Next data Index</param>
    /// <returns>Returns true if the end has NOT been reached.</returns>
    bool FindNextAndPrev(int iPoint, int iPrevCusp, int & iPrev, int & iNext);


    /// <summary>
    ///
    /// </summary>
    /// <param name="a"></param>
    /// <param name="rMin"></param>
    /// <param name="rMax"></param>
    static void UpdateMinMax(double a, double & rMin, double & rMax)
    {
        rMin = qMin(rMin, a);
        rMax = qMax(a, rMax);
    }

    /// <summary>
    /// Sets up the internal data structure to construct chain of points
    /// </summary>
    /// <param name="rSpan">Shortest distance between two distinct points</param>
    void SetLinks(double rSpan);

private:
    struct CDataPoint
    {
        QPointF      Point;       // Point (coordinates are double)
        int          Index;       // Index into the original array
        int          TanPrev = 0;    // Previous StylusPoint Index for tangent computation
        int          TanNext = 0;    // Next StylusPoint Index for tangent computation
    };

    QList<CDataPoint> _points;
    QList<double> _nodes;
    double _dist = 0;
    QList<int> _cusps;

    // Parameters governing the cusp detection algorithm
    // Distance between probes for curvature checking
    double _span = 3; // Default span

};

#endif // CUSPDATA_H
