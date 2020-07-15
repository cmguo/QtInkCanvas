#include "iosstreamgeometrycontext.h"
#include "uibezierpathwrapper.h"

#include <Windows/Media/matrix.h>
#include <Windows/Media/streamgeometry.h>

INKCANVAS_BEGIN_NAMESPACE

IosStreamGeometryContext::IosStreamGeometryContext(StreamGeometry * geometry)
{
    geometry_ = geometry;
}

IosStreamGeometryContext::~IosStreamGeometryContext()
{
    if (path_) {
        UIBezierPathWrapper_delete(path_);
        path_ = nullptr;
    }
}

void IosStreamGeometryContext::BeginFigure(const Point &startPoint, bool, bool)
{
    UIBezierPathWrapper_moveToPoint(path_, startPoint.X(), startPoint.Y());
}

void IosStreamGeometryContext::LineTo(const Point &point, bool, bool)
{
    UIBezierPathWrapper_addLineToPoint(path_, point.X(), point.Y());
}

void IosStreamGeometryContext::QuadraticBezierTo(const Point &point1, const Point &point2, bool, bool)
{
    // point2: control point
    UIBezierPathWrapper_addQuadCurveToPoint(path_, point2.X(), point2.Y(), point1.X(), point1.Y());
}

void IosStreamGeometryContext::BezierTo(const Point &point1, const Point &point2, const Point &point3, bool isStroked, bool isSmoothJoin)
{
    UIBezierPathWrapper_addCurveToPoint(path_, point3.X(), point3.Y(), point1.X(), point1.Y(), point2.X(), point2.Y());
}

void IosStreamGeometryContext::PolyLineTo(const List<Point> &points, bool isStroked, bool isSmoothJoin)
{
    for (Point const & pt : points) {
        LineTo(pt, isStroked, isSmoothJoin);
    }
}

void IosStreamGeometryContext::PolyQuadraticBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.Count(); i += 2) {
        QuadraticBezierTo(points[i], points[i + 1], isStroked, isSmoothJoin);
    }
}

void IosStreamGeometryContext::PolyBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.Count(); i += 3) {
        BezierTo(points[i], points[i + 1], points[i + 2], isStroked, isSmoothJoin);
    }
}

void IosStreamGeometryContext::ArcTo(const Point &point, const Size &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin)
{
    double lx = 0, ly = 0;
    UIBezierPathWrapper_currentPoint(path_, lx, ly);
    Point lastp(lx, ly);
    Point curtp = point;
    //log("ArcTo (%lf,%lf) %lf (%lf,%lf) (%lf,%lf)", size.Width(), size.Height(),
    //    rotationAngle, lastp.X(), lastp.Y(), curtp.X(), curtp.Y());
    // pt1, pt2 are two points in unit circle (location at [0, 0] and with r = 1)
    //   pt1 = [cos(t1) , sin(t1)]
    //   pt2 = [cos(t2) , sin(t2)]
    // pe1, pe2 are pt1, pt2 scaled by [rx, ry] = size, that is in non-rotate Arc location at [0, 0]
    //   pe1 = [rx * cos(t1), ry * sin(t1)]
    //   pe2 = [rx * cos(t2), ry * sin(t2)]
    // pa1, pa2 are pe1, pe2 rotate by r = sweepDirection, that is in rotate Arc location at [0, 0]
    //   |pa1|   |cos(t1) sin(t1)|   |rx  0|   | cos(r) sin(r)|
    //   |   | = |               | * |     | * |              |
    //   |pa2|   |cos(t2) sin(t2)|   |0  ry|   |-sin(r) cos(r)|
    rotationAngle = rotationAngle * Math::PI / 180;
    //Point rotate(cos(rotationAngle), sin(rotationAngle)); // [cos(r), sin(r)]
    double rx = size.Width(), ry = size.Height();
    //Matrix matrix(rx * rotate.x(), rx * rotate.y(),
    //            -ry * rotate.y(), ry * rotate.x(), 0, 0);
    Matrix matrix(rx, 0, 0, ry, 0, 0);
    // t = [pt1 - pt2] / 2 = [cos(t1) - cos(t2), sin(t1) - sin(t2)] / 2;
    //                     = [-sin((t1 + t2) / 2) * sin((t1 - t2) / 2), cos((t1 + t2) / 2) * sin((t1 - t2) / 2)]
    Matrix matrix2 = matrix; matrix2.Invert();
    Vector t = matrix2.Transform(lastp - curtp) / 2;
    //log("t (%lf,%lf)", t.X(), t.Y());
    // a1 = (t1 + t2) / 2, a2 = (t1 - t2) / 2; t1 > t2
    double a1 = atan(-t.X() / t.Y());
    double product = t.LengthSquared();
    if (product > 1.0) {
        //log("product %lf", product << t);
        if (!DoubleUtil::AreClose(product, 1.0)) {
            LineTo(curtp, false, false);
            return;
        }
        product = 1.0;
    }
    double a2 = asin(product);
    //log("a1 <-> a2 %lf %lf", (a1 * 180 / Math::PI), (a2 * 180 / Math::PI));
    if (isLargeArc != (sweepDirection == SweepDirection::Clockwise))
        a2 = -a2;
    if (a2 * t.Y() <= 0)
        a1 += Math::PI;
    double t1 = a1 + a2, t2 = a1 - a2;
    //log("t1 <-> t2 %lf %lf", (t1 * 180 / Math::PI), (t2 * 180 / Math::PI));
    //Vector pt1(cos(t1), sin(t1));
    Vector pt2(cos(t2), sin(t2));
    //log("pt1 <-> pt2 (%lf,%lf) (%lf,%lf)", pt1.X(), pt1.Y(), pt2.X(), pt2.Y());
    //t = (pt1 - pt2) / 2;
    //log("t (%lf,%lf)", t.X(), t.Y());
    Point c = curtp - matrix.Transform(pt2);
    //
    Rect rect(-rx, -ry, rx * 2, ry * 2);
    //Point pe1(pt1.X() * rx, pt1.Y() * ry);
    //Point pe2(pt2.X() * rx, pt2.Y() * ry);
    //log("pe1 <-> pe2 %lf %lf", pe1, pe2);
    UIBezierPathWrapper_addArcWithCenter(path_, c.X(), c.Y(), size.Width(),
                               Math::PI * 2 - a1, Math::PI * 2 - a2,
                               sweepDirection == SweepDirection::Clockwise);
}

void IosStreamGeometryContext::Close()
{
    StreamGeometryContext::Close();
}

void IosStreamGeometryContext::SetClosedState(bool closed)
{

}

void IosStreamGeometryContext::DisposeCore()
{
    geometry_->Close(path_);
    path_ = nullptr;
}

INKCANVAS_END_NAMESPACE
