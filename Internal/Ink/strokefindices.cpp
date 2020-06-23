#include "Internal/Ink/strokefindices.h"

#include <QString>

INKCANVAS_BEGIN_NAMESPACE

StrokeFIndices StrokeFIndices::s_empty(AfterLast, BeforeFirst);
StrokeFIndices StrokeFIndices::s_full(BeforeFirst, AfterLast);

StrokeFIndices::StrokeFIndices(double beginFIndex, double endFIndex)
{
    _beginFIndex = beginFIndex;
    _endFIndex = endFIndex;
}

QString StrokeFIndices::ToString() const
{
    return "{" + GetStringRepresentation(_beginFIndex) + "," + GetStringRepresentation(_endFIndex) + "}";
}

QString StrokeFIndices::GetStringRepresentation(double fIndex)
{
    if (DoubleUtil::AreClose(fIndex, BeforeFirst))
    {
        return "BeforeFirst";
    }
    if (DoubleUtil::AreClose(fIndex, AfterLast))
    {
        return "AfterLast";
    }
    return QString("%1").arg(fIndex);
}

INKCANVAS_END_NAMESPACE
