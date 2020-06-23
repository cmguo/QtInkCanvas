#ifndef STROKEFINDICES_H
#define STROKEFINDICES_H

#include "Internal/doubleutil.h"

#include <QtMath>

#include <float.h>

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// A helper struct that represents a fragment of a stroke spine.
/// </summary>
class StrokeFIndices
{
private:
    static StrokeFIndices s_empty;
    static StrokeFIndices s_full;

    /// <summary>
    /// BeforeFirst
    /// </summary>
    /// <value></value>
public:
    static constexpr double BeforeFirst = -DBL_MAX;

    /// <summary>
    /// AfterLast
    /// </summary>
    /// <value></value>
    static constexpr double AfterLast = DBL_MAX;

    StrokeFIndices()
    {
    }

    /// <summary>
    /// StrokeFIndices
    /// </summary>
    /// <param name="beginFIndex">beginFIndex</param>
    /// <param name="endFIndex">endFIndex</param>
    StrokeFIndices(double beginFIndex, double endFIndex);

    /// <summary>
    /// BeginFIndex
    /// </summary>
    /// <value></value>
    double BeginFIndex() const
    {
        return _beginFIndex;
    }
    void SetBeginFIndex(double value)
    {
        _beginFIndex = value;
    }

    /// <summary>
    /// EndFIndex
    /// </summary>
    /// <value></value>
    double EndFIndex() const
    {
        return _endFIndex;
    }
    void SetEndFIndex(double value)
    {
        _endFIndex = value;
    }

    /// <summary>
    /// ToString
    /// </summary>
    QString ToString() const;

    /// <summary>
    /// Equals
    /// </summary>
    /// <param name="strokeFIndices"></param>
    /// <returns></returns>
    bool Equals(StrokeFIndices const & strokeFIndices) const
    {
        return (strokeFIndices == *this);
    }


    /// <summary>
    /// operator ==
    /// </summary>
    /// <param name="sfiLeft"></param>
    /// <param name="sfiRight"></param>
    /// <returns></returns>
    friend bool operator ==(StrokeFIndices const & sfiLeft, StrokeFIndices const & sfiRight)
    {
        return (DoubleUtil::AreClose(sfiLeft._beginFIndex, sfiRight._beginFIndex)
                && DoubleUtil::AreClose(sfiLeft._endFIndex, sfiRight._endFIndex));
    }

    /// <summary>
    /// operator !=
    /// </summary>
    /// <param name="sfiLeft"></param>
    /// <param name="sfiRight"></param>
    /// <returns></returns>
    friend bool operator !=(StrokeFIndices const & sfiLeft, StrokeFIndices const & sfiRight)
    {
        return !(sfiLeft == sfiRight);
    }

    static QString GetStringRepresentation(double fIndex);

    /// <summary>
    ///
    /// </summary>
    static StrokeFIndices Empty() { return s_empty; }

    /// <summary>
    ///
    /// </summary>
    static StrokeFIndices Full() { return s_full; }

    /// <summary>
    ///
    /// </summary>
    bool IsEmpty() const { return DoubleUtil::GreaterThanOrClose(_beginFIndex, _endFIndex); }

    /// <summary>
    ///
    /// </summary>
    bool IsFull() const { return ((DoubleUtil::AreClose(_beginFIndex, BeforeFirst))
                                  && (DoubleUtil::AreClose(_endFIndex,AfterLast))); }


#if DEBUG
    /// <summary>
    ///
    /// </summary>
    private bool IsValid { get { return !double.IsNaN(_beginFIndex) && !double.IsNaN(_endFIndex) && _beginFIndex < _endFIndex; } }

#endif

    /// <summary>
    /// Compare StrokeFIndices based on the BeinFIndex
    /// </summary>
    /// <param name="fIndices"></param>
    /// <returns></returns>
    int CompareTo(StrokeFIndices const & fIndices) const
    {
#ifdef DEBUG
        System.Diagnostics.Debug.Assert(!double.IsNaN(_beginFIndex) && !double.IsNaN(_endFIndex) && DoubleUtil::LessThan(_beginFIndex, _endFIndex));
#endif
        if (DoubleUtil::AreClose(BeginFIndex(), fIndices.BeginFIndex()))
        {
            return 0;
        }
        else if (DoubleUtil::GreaterThan(BeginFIndex(), fIndices.BeginFIndex()))
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }

private:
    double _beginFIndex;
    double _endFIndex;
};

INKCANVAS_END_NAMESPACE

#endif // STROKEFINDICES_H
