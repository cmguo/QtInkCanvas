#ifndef STROKEINTERSECTION_H
#define STROKEINTERSECTION_H

#include "Internal/Ink/strokefindices.h"
#include "Internal/doubleutil.h"

#include <QRectF>
#include <QPointF>
#include <QVector>
#include <QList>

// namespace System.Windows.Ink

class StrokeIntersection
{
private:
    static StrokeIntersection s_empty;
    static StrokeIntersection s_full;


    /// <summary>
    /// BeforeFirst
    /// </summary>
    /// <value></value>
public:
    static constexpr double BeforeFirst = StrokeFIndices::BeforeFirst;

    /// <summary>
    /// AfterLast
    /// </summary>
    /// <value></value>
    static constexpr double AfterLast = StrokeFIndices::AfterLast;

    StrokeIntersection()
    {
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="hitBegin"></param>
    /// <param name="inBegin"></param>
    /// <param name="inEnd"></param>
    /// <param name="hitEnd"></param>
    StrokeIntersection(double hitBegin, double inBegin, double inEnd, double hitEnd)
        : _hitSegment(hitBegin, hitEnd)
        , _inSegment(inBegin, inEnd)
    {
        //ISSUE-2004/12/06-XiaoTu: should we validate the input?
    }

    /// <summary>
    /// hitBeginFIndex
    /// </summary>
    /// <value></value>
    void SetHitBegin(double value)
    {
        _hitSegment.SetBeginFIndex(value);
    }

    /// <summary>
    /// hitEndFIndex
    /// </summary>
    /// <value></value>
    double HitEnd()
    {
        return _hitSegment.EndFIndex();
    }
    void SetHitEnd(double value)
    {
        _hitSegment.SetEndFIndex(value);
    }


    /// <summary>
    /// InBegin
    /// </summary>
    /// <value></value>
    double InBegin()
    {
        return _hitSegment.BeginFIndex();
    }
    void SetInBegin(double value)
    {
        _hitSegment.SetBeginFIndex(value);
    }


    /// <summary>
    /// InEnd
    /// </summary>
    /// <value></value>
    double InEnd()
    {
        return _hitSegment.EndFIndex();
    }
    void SetInEnd(double value)
    {
        _hitSegment.SetEndFIndex(value);
    }


    /// <summary>
    /// ToString
    /// </summary>
    QString ToString() const
    {
        return "{"  + StrokeFIndices::GetStringRepresentation(_hitSegment.BeginFIndex()) + ","
                    + StrokeFIndices::GetStringRepresentation(_inSegment.BeginFIndex())  + ","
                    + StrokeFIndices::GetStringRepresentation(_inSegment.EndFIndex())    + ","
                    + StrokeFIndices::GetStringRepresentation(_hitSegment.EndFIndex())   + "}";
    }


    /// <summary>
    /// Equals
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    //public override bool Equals(Object obj)
    //{
        // Check for null and compare run-time types
    //    if (obj == null || GetType() != obj.GetType())
    //        return false;
    //    return ((StrokeIntersection)obj == this);
    //}

    /// <summary>
    /// GetHashCode
    /// </summary>
    /// <returns></returns>
    //public override int GetHashCode()
    //{
    //    return _hitSegment.GetHashCode() ^ _inSegment.GetHashCode();
    //}


    /// <summary>
    /// operator ==
    /// </summary>
    /// <param name="left"></param>
    /// <param name="right"></param>
    /// <returns></returns>
    friend bool operator ==(StrokeIntersection const & left, StrokeIntersection const & right)
    {
        return (left._hitSegment == right._hitSegment && left._inSegment == right._inSegment);
    }

    /// <summary>
    /// operator !=
    /// </summary>
    /// <param name="left"></param>
    /// <param name="right"></param>
    /// <returns></returns>
    friend bool operator !=(StrokeIntersection const & left, StrokeIntersection const & right)
    {
        return !(left == right);
    }


    /// <summary>
    ///
    /// </summary>
    static StrokeIntersection Full() { return s_full; }

    /// <summary>
    ///
    /// </summary>
    bool IsEmpty() { return _hitSegment.IsEmpty(); }


    /// <summary>
    ///
    /// </summary>
    StrokeFIndices & HitSegment()
    {
        return _hitSegment;
    }

    /// <summary>
    ///
    /// </summary>
    StrokeFIndices & InSegment()
    {
        return _inSegment;
    }


    /// <summary>
    /// Get the "in-segments" of the intersections.
    /// </summary>
    static QVector<StrokeFIndices> GetInSegments(QVector<StrokeIntersection> & intersections)
    {
        //System.Diagnostics.Debug.Assert(intersections != null);
        //System.Diagnostics.Debug.Assert(intersections.Length > 0);

        QList<StrokeFIndices> inFIndices;
        inFIndices.reserve(intersections.size());
        for (int j = 0; j < intersections.size(); j++)
        {
            //System.Diagnostics.Debug.Assert(!intersections[j].IsEmpty);
            if (!intersections[j].InSegment().IsEmpty())
            {
                if (inFIndices.size() > 0 &&
                    inFIndices[inFIndices.size() - 1].EndFIndex() >=
                    intersections[j].InSegment().BeginFIndex())
                {
                    //merge
                    StrokeFIndices sfiPrevious = inFIndices[inFIndices.size() - 1];
                    sfiPrevious.SetEndFIndex(intersections[j].InSegment().EndFIndex());
                    inFIndices[inFIndices.size() - 1] = sfiPrevious;
                }
                else
                {
                    inFIndices.append(intersections[j].InSegment());
                }
            }
        }
        return inFIndices.toVector();
    }

    /// <summary>
    /// Get the "hit-segments"
    /// </summary>
    static QVector<StrokeFIndices> GetHitSegments(QVector<StrokeIntersection> & intersections)
    {
        //System.Diagnostics.Debug.Assert(intersections != null);
        //System.Diagnostics.Debug.Assert(intersections.Length > 0);

        QList<StrokeFIndices> hitFIndices;
        hitFIndices.reserve(intersections.size());
        for (int j = 0; j < intersections.size(); j++)
        {
            //System.Diagnostics.Debug.Assert(!intersections[j].IsEmpty);
            if (!intersections[j].HitSegment().IsEmpty())
            {
                if (hitFIndices.size() > 0 &&
                    hitFIndices[hitFIndices.size() - 1].EndFIndex() >=
                    intersections[j].HitSegment().BeginFIndex())
                {
                    //merge
                    StrokeFIndices sfiPrevious = hitFIndices[hitFIndices.size() - 1];
                    sfiPrevious.SetEndFIndex(intersections[j].HitSegment().EndFIndex());
                    hitFIndices[hitFIndices.size() - 1] = sfiPrevious;
                }
                else
                {
                    hitFIndices.append(intersections[j].HitSegment());
                }
            }
        }
        return hitFIndices.toVector();
    }


private:
    StrokeFIndices _hitSegment;
    StrokeFIndices _inSegment;
};

#endif // STROKEINTERSECTION_H
