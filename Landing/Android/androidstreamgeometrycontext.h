#ifndef ANDROIDSTREAMGEOMETRYCONTEXT_H
#define ANDROIDSTREAMGEOMETRYCONTEXT_H

#include <Windows/Media/streamgeometrycontext.h>

INKCANVAS_BEGIN_NAMESPACE

class StreamGeometry;

void log(char const * msg, ...);

class AndroidStreamGeometryContext : public StreamGeometryContext
{
public:
    static bool init(void * env);

    AndroidStreamGeometryContext(StreamGeometry * geometry);

private:
    StreamGeometry * geometry_ = nullptr;
    void* path_;
    Point lastPoint_;

    // StreamGeometryContext interface
public:
    virtual void BeginFigure(const Point &startPoint, bool isFilled, bool isClosed) override;
    virtual void LineTo(const Point &point, bool isStroked, bool isSmoothJoin) override;
    virtual void QuadraticBezierTo(const Point &point1, const Point &point2, bool isStroked, bool isSmoothJoin) override;
    virtual void BezierTo(const Point &point1, const Point &point2, const Point &point3, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyLineTo(const List<Point> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyQuadraticBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void PolyBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin) override;
    virtual void ArcTo(const Point &point, const Size &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin) override;
    virtual void SetClosedState(bool closed) override;
    virtual void Close() override;
    virtual void DisposeCore() override;
};

INKCANVAS_END_NAMESPACE

#endif // ANDROIDSTREAMGEOMETRYCONTEXT_H
