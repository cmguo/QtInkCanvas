#include "cuspdata.h"
#include "styluspointcollection.h"
#include "styluspoint.h"
#include "strokecollectionserializer.h"
#include "doubleutil.h"

CuspData::CuspData()
{

}

/// <summary>
/// Constructs internal data structure from points for doing operations like
/// cusp detection or tangent computation
/// </summary>
/// <param name="stylusPoints">Points to be analyzed</param>
/// <param name="rSpan">Distance between two consecutive distinct points</param>
void CuspData::Analyze(StylusPointCollection & stylusPoints, double rSpan)
{
    // If the count is less than 1, return
    if ((stylusPoints.size() == 0))
        return;

    _points.resize(stylusPoints.size());
    _nodes.resize(stylusPoints.size());

    // Construct the lists of data points and nodes
    _nodes.push_back(0);
    CDataPoint cdp0;
    cdp0.Index = 0;
    //convert from Avalon to Himetric
    QPointF point = stylusPoints[0];
    point *= StrokeCollectionSerializer::AvalonToHimetricMultiplier;
    cdp0.Point = point;
    _points.push_back(cdp0);

    //drop duplicates
    int index = 0;
    for (int i = 1; i < stylusPoints.size(); i++)
    {
        if (!qFuzzyCompare(stylusPoints[i].X(), stylusPoints[i - 1].X()) ||
            !qFuzzyCompare(stylusPoints[i].Y(), stylusPoints[i - 1].Y()))
        {
            //this is a unique point, add it
            index++;

            CDataPoint cdp;
            cdp.Index = index;

            //convert from Avalon to Himetric
            QPointF point2 = stylusPoints[i];
            point2 *= StrokeCollectionSerializer::AvalonToHimetricMultiplier;
            cdp.Point = point2;

            _points.insert(index, cdp);
            _nodes.insert(index, _nodes[index - 1] + Length(XY(index) - XY(index - 1)));
        }
    }

    SetLinks(rSpan);
}

/// <summary>
/// Set links amongst the points for tangent computation
/// </summary>
/// <param name="rError">Shortest distance between two distinct points</param>
void CuspData::SetTanLinks(double rError)
{
    int count = Count();

    if (rError < 1.0)
        rError = 1.0;

    for (int i = 0; i < count; ++i)
    {
        // Find a StylusPoint at distance-_span forward
        for (int j = i + 1; j < count; j++)
        {
            if (_nodes[j] - _nodes[i] >= rError)
            {
                CDataPoint cdp = _points[i];
                cdp.TanNext = j;
                _points[i] = cdp;

                CDataPoint cdp2 = _points[j];
                cdp2.TanPrev = i;
                _points[j] = cdp2;
                break;
            }
        }

        if (0 > _points[i].TanPrev)
        {
            for (int j = i - 1; 0 <= j; --j)
            {
                if (_nodes[i] - _nodes[j] >= rError)
                {
                    CDataPoint cdp = _points[i];
                    cdp.TanPrev = j;
                    _points[i] = cdp;
                    break;
                }
            }
        }

        if (0 > _points[i].TanNext)
        {
            CDataPoint cdp = _points[i];
            cdp.TanNext = count - 1;
            _points[i] = cdp;
        }

        if (0 > _points[i].TanPrev)
        {
            CDataPoint cdp = _points[i];
            cdp.TanPrev = 0;
            _points[i] = cdp;
        }
    }
}




/// <summary>
/// Return the Index of the next cusp or the
/// Index of the last StylusPoint if no cusp was found
/// </summary>
/// <param name="iCurrent">Current StylusPoint Index</param>
/// <returns>Index into CuspData object for the next cusp </returns>
int CuspData::GetNextCusp(int iCurrent)
{
    int last = Count() - 1;

    if (iCurrent < 0)
        return 0;

    if (iCurrent >= last)
        return last;

    // Perform a binary search
    int s = 0, e = _cusps.size();
    int m = (s + e) / 2;

    while (s < m)
    {
        if (_cusps[m] <= iCurrent)
            s = m;
        else
            e = m;

        m = (s + e) / 2;
    }

    return _cusps[m + 1];
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
bool CuspData::Tangent(QPointF & ptT, int nAt, int nPrevCusp, int nNextCusp, bool bReverse, bool bIsCusp)
{
    // Tangent is computed as the unit vector along
    // PT = (P1 - P0) + (P2 - P0) + (P3 - P0)
    // => PT = P1 + P2 + P3 - 3 * P0
    int i_1, i_2, i_3;

    if (bIsCusp)
    {
        if (bReverse)
        {
            i_1 = _points[nAt].TanPrev;
            if (i_1 < nPrevCusp || (0 > i_1))
            {
                i_2 = nPrevCusp;
                i_1 = (i_2 + nAt) / 2;
            }
            else
            {
                i_2 = _points[i_1].TanPrev;
                if (i_2 < nPrevCusp)
                    i_2 = nPrevCusp;
            }
        }
        else
        {
            i_1 = _points[nAt].TanNext;
            if (i_1 > nNextCusp || (0 > i_1))
            {
                i_2 = nNextCusp;
                i_1 = (i_2 + nAt) / 2;
            }
            else
            {
                i_2 = _points[i_1].TanNext;
                if (i_2 > nNextCusp)
                    i_2 = nNextCusp;
            }
        }
        ptT = XY(i_1) + 0.5 * XY(i_2) - 1.5 * XY(nAt);
    }
    else
    {
        //Debug.Assert(bReverse);
        i_1 = nAt;
        i_2 = _points[nAt].TanPrev;
        if (i_2 < nPrevCusp)
        {
            i_3 = nPrevCusp;
            i_2 = (i_3 + i_1) / 2;
        }
        else
        {
            i_3 = _points[i_2].TanPrev;
            if (i_3 < nPrevCusp)
                i_3 = nPrevCusp;
        }

        nAt = _points[nAt].TanNext;
        if (nAt > nNextCusp)
            nAt = nNextCusp;

        ptT = XY(i_1) + XY(i_2) + 0.5 * XY(i_3) - 2.5 * XY(nAt);
    }

    if (qFuzzyIsNull(QPointF::dotProduct(ptT, ptT)))
    {
        return false;
    }

    ptT /= sqrt(QPointF::dotProduct(ptT, ptT));
    return true;
}

/// <summary>
/// This "curvature" is not the theoretical curvature.  it is a number between
/// 0 and 2 that is defined as 1 - cos(angle between segments) at this StylusPoint.
/// </summary>
/// <param name="iPrev">Previous data StylusPoint Index</param>
/// <param name="iCurrent">Current data StylusPoint Index </param>
/// <param name="iNext">Next data StylusPoint Index</param>
/// <returns>"Curvature"</returns>
double CuspData::GetCurvature(int iPrev, int iCurrent, int iNext)
{
    QPointF V = XY(iCurrent) - XY(iPrev);
    QPointF W = XY(iNext) - XY(iCurrent);
    double r = sqrt(QPointF::dotProduct(V, V) * (QPointF::dotProduct(W, W)));

    if (qFuzzyIsNull(r))
        return 0;

    return 1 - QPointF::dotProduct(V, W) / r;
}


/// <summary>
/// Find all cusps for the stroke
/// </summary>
void CuspData::FindAllCusps()
{
    // Clear the existing cusp indices
    _cusps.clear();

    // There is nothing to find out from
    if (1 > Count())
        return;

    // First StylusPoint is always a cusp
    _cusps.push_back(0);

    int iPrev = 0, iNext = 0, iCuspPrev = 0;

    // Find the next StylusPoint for Index 0
    // The following check will cover coincident points, stroke with
    // less than 3 points
    if (!FindNextAndPrev(0, iCuspPrev, iPrev, iNext))
    {
        // Point count is zero, thus, there can't be any cusps
        if (0 == Count())
            _cusps.clear();
        else if (1 < Count()) // Last StylusPoint is always a cusp
            _cusps.push_back(iNext);

        return;
    }

    // Start the algorithm with the next StylusPoint
    int iPoint = iNext;
    double rCurv = 0;

    // Check all the points on the chord of the stroke
    while (FindNextAndPrev(iPoint, iCuspPrev, iPrev, iNext))
    {
        // Find the curvature at iPoint
        rCurv = GetCurvature(iPrev, iPoint, iNext);

        /*
            We'll look at every StylusPoint where rPrevCurv is a local maximum, and the
            curvature is more than the noise threashold.  If we're near the beginning
            of the stroke then we'll ignore it and carry on.  If we're near the end
            then we'll skip to the end.  Otherwise, we'll flag it as a cusp if it
            deviates is significantly from the curvature at nearby points, forward
            and backward
        */
        if (0.80 < rCurv)
        {
            double rMaxCurv = rCurv;
            int iMaxCurv = iPoint;
            int m = 0, k = 0;

            if (!FindNextAndPrev(iNext, iCuspPrev, k, m))
            {
                // End of the stroke has been reached
                break;
            }

            for (int i = iPrev + 1; (i <= m) && FindNextAndPrev(i, iCuspPrev, iPrev, iNext); ++i)
            {
                rCurv = GetCurvature(iPrev, i, iNext);
                if (rCurv > rMaxCurv)
                {
                    rMaxCurv = rCurv;
                    iMaxCurv = i;
                }
            }

            // Save the Index with max curvature
            _cusps.push_back(iMaxCurv);

            // Continue the search with next StylusPoint
            iPoint = m + 1;
            iCuspPrev = iMaxCurv;
        }
        else if (0.035 > rCurv)
        {
            // If the angle is less than 15 degree, skip the segment
            iPoint = iNext;
        }
        else
            ++iPoint;
    }

    // If everything went right, add the last StylusPoint to the list of cusps
    _cusps.push_back(Count() - 1);
}


/// <summary>
/// Finds the next and previous data StylusPoint Index for the given data Index
/// </summary>
/// <param name="iPoint">Index at which the computation is performed</param>
/// <param name="iPrevCusp">Previous cusp</param>
/// <param name="iPrev">Previous data Index</param>
/// <param name="iNext">Next data Index</param>
/// <returns>Returns true if the end has NOT been reached.</returns>
bool CuspData::FindNextAndPrev(int iPoint, int iPrevCusp, int & iPrev, int & iNext)
{
    bool bHasMore = true;

    if (iPoint >= Count())
    {
        bHasMore = false;
        iPoint = Count() - 1;
    }

    // Find a StylusPoint at distance-_span forward
    for (iNext = iPoint + 1; iNext < Count(); ++iNext)
        if (_nodes[iNext] - _nodes[iPoint] >= _span)
            break;

    if (iNext >= Count())
    {
        bHasMore = false;
        iNext = Count() - 1;
    }

    for (iPrev = iPoint - 1; iPrevCusp <= iPrev; --iPrev)
        if (_nodes[iPoint] - _nodes[iPrev] >= _span)
            break;

    if (iPrev < 0)
        iPrev = 0;

    return bHasMore;
}


void CuspData::SetLinks(double rSpan)
{
    // NOP, if there is only one StylusPoint
    int count = Count();

    if (2 > count)
        return;

    // Set up the links to next and previous probe
    double rL = XY(0).x();
    double rT = XY(0).y();
    double rR = rL;
    double rB = rT;

    for (int i = 0; i < count; ++i)
    {
        UpdateMinMax(XY(i).x(), rL, rR);
        UpdateMinMax(XY(i).y(), rT, rB);
    }

    rR -= rL;
    rB -= rT;
    _dist = qAbs(rR) + qAbs(rB);
    if (false == qFuzzyIsNull(rSpan))
        _span = rSpan;
    else if (0 < _dist)
    {
        /***
        _nodes[count - 1] at this StylusPoint contains the length of the stroke.
        _dist is the half peripheri of the bounding box of the stroke.
        The idea here is that the Length/_dist is somewhat analogous to the
        "fractal dimension" of the stroke (or in other words, how much the stroke
        winds.)
        Length/count is the average distance between two consequitive points
        on the stroke. Thus, this average distance is multiplied by the winding
        factor.
        If the stroke were a PURE straight line across the diagone of a square,
        Lenght/Dist will be approximately 1.41. And if there were one pixel per
        co-ordinate, the span would have been 1.41, which works fairly well in
        cusp detection
        ***/
        _span = 0.75 * (_nodes[count - 1] * _nodes[count - 1]) / (count * _dist);
    }

    if (_span < 1.0)
        _span = 1.0;

    FindAllCusps();
}
