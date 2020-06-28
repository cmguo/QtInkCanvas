#include "Internal/Ink/strokefindices.h"

#ifdef INKCANVAS_QT
#include <QString>
#endif

INKCANVAS_BEGIN_NAMESPACE

StrokeFIndices StrokeFIndices::s_empty(AfterLast, BeforeFirst);
StrokeFIndices StrokeFIndices::s_full(BeforeFirst, AfterLast);

StrokeFIndices::StrokeFIndices(double beginFIndex, double endFIndex)
{
    _beginFIndex = beginFIndex;
    _endFIndex = endFIndex;
}

#ifdef INKCANVAS_QT
QString StrokeFIndices::ToString() const
{
    return QString("{") + GetStringRepresentation(_beginFIndex) + "," + GetStringRepresentation(_endFIndex) + "}";
}
#endif

char const * StrokeFIndices::GetStringRepresentation(double fIndex)
{
    if (DoubleUtil::AreClose(fIndex, BeforeFirst))
    {
        return "BeforeFirst";
    }
    if (DoubleUtil::AreClose(fIndex, AfterLast))
    {
        return "AfterLast";
    }
    return "";
    //return QString("%1").arg(fIndex);
}

INKCANVAS_END_NAMESPACE
