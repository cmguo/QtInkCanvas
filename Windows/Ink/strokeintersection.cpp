#include "Windows/Ink/strokeintersection.h"

StrokeIntersection StrokeIntersection::s_empty(AfterLast, AfterLast, BeforeFirst, BeforeFirst);
StrokeIntersection StrokeIntersection::s_full(BeforeFirst, BeforeFirst, AfterLast, AfterLast);

/// <summary>
/// Get the "in-segments" of the intersections.
/// </summary>
QVector<StrokeFIndices> StrokeIntersection::GetInSegments(QVector<StrokeIntersection> & intersections)
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
QVector<StrokeFIndices> StrokeIntersection::GetHitSegments(QVector<StrokeIntersection> & intersections)
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


QVector<StrokeIntersection> StrokeIntersection::GetClippedHitSegments(QVector<StrokeIntersection> &intersections,
                                                                      QVector<StrokeIntersection> &clip)
{
    QVector<StrokeIntersection> result;
    int j = 0;
    for (StrokeIntersection & si : intersections) {
        while (j < clip.size() && clip[j].HitSegment().EndFIndex() <= si.HitSegment().BeginFIndex()) {
            ++j;
        }
        if (clip[j].HitSegment().EndFIndex() >= si.HitSegment().EndFIndex()) {
            if (clip[j].HitSegment().BeginFIndex() >= si.HitSegment().EndFIndex()) {
            } else if (clip[j].HitSegment().BeginFIndex() > si.HitSegment().BeginFIndex()) {
                result.append(si);
                result.back().SetHitEnd(clip[j].HitSegment().BeginFIndex());
            }
        } else if (clip[j].HitSegment().BeginFIndex() < si.HitSegment().BeginFIndex()) {
            result.append(si);
            result.back().SetHitBegin(clip[j].HitSegment().EndFIndex());
        } else {
            result.append(si);
            result.back().SetHitEnd(clip[j].HitSegment().BeginFIndex());
            result.append(si);
            result.back().SetHitBegin(clip[j].HitSegment().EndFIndex());
        }
    }
    return result;
}
