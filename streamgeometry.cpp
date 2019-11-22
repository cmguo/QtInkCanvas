#include "streamgeometry.h"
#include "qtstreamgeometrycontext.h"

StreamGeometry::StreamGeometry()
    : context_(nullptr)
{
}

StreamGeometry::~StreamGeometry()
{
    if (context_)
        delete context_;
}

void StreamGeometry::SetFillRule(FillRule value)
{

}

StreamGeometryContext &StreamGeometry::Open()
{
    context_ = new QtStreamGeometryContext;
    return *context_;
}
