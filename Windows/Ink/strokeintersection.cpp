#include "Windows/Ink/strokeintersection.h"

#include <Collections/Generic/list.h>

INKCANVAS_BEGIN_NAMESPACE

StrokeIntersection StrokeIntersection::s_empty(AfterLast, AfterLast, BeforeFirst, BeforeFirst);
StrokeIntersection StrokeIntersection::s_full(BeforeFirst, BeforeFirst, AfterLast, AfterLast);

/// <summary>
/// Get the "in-segments" of the intersections.
/// </summary>
Array<StrokeFIndices> StrokeIntersection::GetInSegments(Array<StrokeIntersection> & intersections)
{
    //System.Diagnostics.Debug.Assert(intersections != null);
    //System.Diagnostics.Debug.Assert(intersections.Length > 0);

    List<StrokeFIndices> inFIndices;
    inFIndices.reserve(intersections.Length());
    for (int j = 0; j < intersections.Length(); j++)
    {
        //System.Diagnostics.Debug.Assert(!intersections[j].IsEmpty);
        if (!intersections[j].InSegment().IsEmpty())
        {
            if (inFIndices.Count() > 0 &&
                inFIndices[inFIndices.Count() - 1].EndFIndex() >=
                intersections[j].InSegment().BeginFIndex())
            {
                //merge
                StrokeFIndices sfiPrevious = inFIndices[inFIndices.Count() - 1];
                sfiPrevious.SetEndFIndex(intersections[j].InSegment().EndFIndex());
                inFIndices[inFIndices.Count() - 1] = sfiPrevious;
            }
            else
            {
                inFIndices.Add(intersections[j].InSegment());
            }
        }
    }
    return inFIndices.ToArray();
}

/// <summary>
/// Get the "hit-segments"
/// </summary>
Array<StrokeFIndices> StrokeIntersection::GetHitSegments(Array<StrokeIntersection> & intersections)
{
    //System.Diagnostics.Debug.Assert(intersections != null);
    //System.Diagnostics.Debug.Assert(intersections.Length > 0);

    List<StrokeFIndices> hitFIndices;
    hitFIndices.reserve(intersections.Length());
    for (int j = 0; j < intersections.Length(); j++)
    {
        //System.Diagnostics.Debug.Assert(!intersections[j].IsEmpty);
        if (!intersections[j].HitSegment().IsEmpty())
        {
            if (hitFIndices.Count() > 0 &&
                hitFIndices[hitFIndices.Count() - 1].EndFIndex() >=
                intersections[j].HitSegment().BeginFIndex())
            {
                //merge
                StrokeFIndices sfiPrevious = hitFIndices[hitFIndices.Count() - 1];
                sfiPrevious.SetEndFIndex(intersections[j].HitSegment().EndFIndex());
                hitFIndices[hitFIndices.Count() - 1] = sfiPrevious;
            }
            else
            {
                hitFIndices.Add(intersections[j].HitSegment());
            }
        }
    }
    return hitFIndices.ToArray();
}


List<StrokeIntersection> StrokeIntersection::GetMaskedHitSegments(List<StrokeIntersection> &intersections,
                                                                      Array<StrokeIntersection> const & mask)
{
    List<StrokeIntersection> result;
    int j = 0;
    for (StrokeIntersection & si : intersections) {
        while (j < mask.Length() && mask[j].HitSegment().EndFIndex() <= si.HitSegment().BeginFIndex()) {
            ++j;
        }
        if (j >= mask.Length()) {
            result.Add(si);
            continue;
        }
        if (mask[j].HitSegment().EndFIndex() >= si.HitSegment().EndFIndex()) {
            if (mask[j].HitSegment().BeginFIndex() >= si.HitSegment().EndFIndex()) {
            } else if (mask[j].HitSegment().BeginFIndex() > si.HitSegment().BeginFIndex()) {
                result.Add(si);
                result.back().SetHitEnd(mask[j].HitSegment().BeginFIndex());
            }
        } else if (mask[j].HitSegment().BeginFIndex() < si.HitSegment().BeginFIndex()) {
            result.Add(si);
            result.back().SetHitBegin(mask[j].HitSegment().EndFIndex());
        } else {
            result.Add(si);
            result.back().SetHitEnd(mask[j].HitSegment().BeginFIndex());
            result.Add(si);
            result.back().SetHitBegin(mask[j].HitSegment().EndFIndex());
        }
    }
    return result;
}

INKCANVAS_END_NAMESPACE
