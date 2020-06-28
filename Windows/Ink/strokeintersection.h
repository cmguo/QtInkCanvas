#ifndef STROKEINTERSECTION_H
#define STROKEINTERSECTION_H

#include "Internal/Ink/strokefindices.h"
#include "Internal/doubleutil.h"
#include "Collections/Generic/list.h"
#include "Windows/rect.h"

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

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
    double HitBegin() const
    {
        return _hitSegment.BeginFIndex();
    }
    void SetHitBegin(double value)
    {
        _hitSegment.SetBeginFIndex(value);
    }

    /// <summary>
    /// hitEndFIndex
    /// </summary>
    /// <value></value>
    double HitEnd() const
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
    double InBegin() const
    {
        return _inSegment.BeginFIndex();
    }
    void SetInBegin(double value)
    {
        _inSegment.SetBeginFIndex(value);
    }


    /// <summary>
    /// InEnd
    /// </summary>
    /// <value></value>
    double InEnd() const
    {
        return _inSegment.EndFIndex();
    }
    void SetInEnd(double value)
    {
        _inSegment.SetEndFIndex(value);
    }

#ifdef INKCANVAS_QT
    /// <summary>
    /// ToString
    /// </summary>
    QString ToString() const
    {
        return QString("{")  + StrokeFIndices::GetStringRepresentation(_hitSegment.BeginFIndex()) + ","
                    + StrokeFIndices::GetStringRepresentation(_inSegment.BeginFIndex())  + ","
                    + StrokeFIndices::GetStringRepresentation(_inSegment.EndFIndex())    + ","
                    + StrokeFIndices::GetStringRepresentation(_hitSegment.EndFIndex())   + "}";
    }
#endif

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
    bool IsEmpty() const { return _hitSegment.IsEmpty(); }


    /// <summary>
    ///
    /// </summary>
    StrokeFIndices  const & HitSegment() const
    {
        return _hitSegment;
    }

    /// <summary>
    ///
    /// </summary>
    StrokeFIndices const & InSegment() const
    {
        return _inSegment;
    }


    /// <summary>
    /// Get the "in-segments" of the intersections.
    /// </summary>
    static Array<StrokeFIndices> GetInSegments(Array<StrokeIntersection> & intersections);

    /// <summary>
    /// Get the "hit-segments"
    /// </summary>
    static Array<StrokeFIndices> GetHitSegments(Array<StrokeIntersection> & intersections);


    static List<StrokeIntersection> GetMaskedHitSegments(List<StrokeIntersection> & intersections,
                                                        Array<StrokeIntersection> const & clip);

private:
    StrokeFIndices _hitSegment;
    StrokeFIndices _inSegment;
};

INKCANVAS_END_NAMESPACE

#endif // STROKEINTERSECTION_H
