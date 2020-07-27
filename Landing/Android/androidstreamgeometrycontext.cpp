#include "androidstreamgeometrycontext.h"
#include "cmath.h"

#include <Windows/Media/matrix.h>
#include <Windows/Media/streamgeometry.h>
#include <Internal/debug.h>

INKCANVAS_BEGIN_NAMESPACE

static JavaVM* s_vm = nullptr;

static jclass sc_Path = nullptr;
static jmethodID sm_Path = nullptr;
static jmethodID sm_Path_moveTo = nullptr;
static jmethodID sm_Path_lineTo = nullptr;
static jmethodID sm_Path_quadTo = nullptr;
static jmethodID sm_Path_cubicTo = nullptr;
static jmethodID sm_Path_arcTo = nullptr;
static jmethodID sm_Path_close = nullptr;

bool AndroidStreamGeometryContext::init(JavaVM * vm, JNIEnv * env)
{
    s_vm = vm;
    sc_Path = env->FindClass("android/graphics/Path");
    if (sc_Path == nullptr) {
        return false;
    }
    sc_Path = reinterpret_cast<jclass>(env->NewGlobalRef(sc_Path));
    sm_Path = env->GetMethodID(sc_Path, "<init>", "()V");
    sm_Path_moveTo = env->GetMethodID(sc_Path, "moveTo", "(FF)V");
    sm_Path_lineTo = env->GetMethodID(sc_Path, "lineTo", "(FF)V");
    sm_Path_quadTo = env->GetMethodID(sc_Path, "quadTo", "(FFFF)V");
    sm_Path_cubicTo = env->GetMethodID(sc_Path, "cubicTo", "(FFFFFF)V");
    sm_Path_arcTo = env->GetMethodID(sc_Path, "arcTo", "(FFFFFFZ)V");
    sm_Path_close = env->GetMethodID(sc_Path, "close", "()V");
    return true;
}

AndroidStreamGeometryContext::AndroidStreamGeometryContext(StreamGeometry * geometry)
{
    //Debug::Log("AndroidStreamGeometryContext");
    geometry_ = geometry;
    int status = s_vm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6);
    if (status != JNI_OK) {
        Debug::Log("AndroidStreamGeometryContext GetEnv failed %d", status);
        return;
    }
    path_ = env_->NewObject(sc_Path, sm_Path);
    //Debug::Log("path %p", path_);
}

#define P() \
    if (env_ == nullptr || path_ == nullptr) \
        return; \
    jobject p = reinterpret_cast<jobject>(path_)

void AndroidStreamGeometryContext::BeginFigure(const Point &startPoint, bool, bool)
{
    //Debug::Log("BeginFigure (%lf,%lf)", startPoint.X(), startPoint.Y());
    P();
    env_->CallVoidMethod(p, sm_Path_moveTo, startPoint.X(), startPoint.Y());
    lastPoint_ = startPoint;
}

void AndroidStreamGeometryContext::LineTo(const Point &point, bool, bool)
{
    //Debug::Log("LineTo (%lf,%lf)", point.X(), point.Y());
    P();
    env_->CallVoidMethod(p, sm_Path_lineTo, point.X(), point.Y());
    lastPoint_ = point;
}

void AndroidStreamGeometryContext::QuadraticBezierTo(const Point &point1, const Point &point2, bool, bool)
{
    //Debug::Log("QuadraticBezierTo (%lf,%lf) (%lf,%lf)", point1.X(), point1.Y(), point2.X(), point2.Y());
    P();
    env_->CallVoidMethod(p, sm_Path_quadTo, point1.X(), point1.Y(), point2.X(), point2.Y());
    lastPoint_ = point2;
}

void AndroidStreamGeometryContext::BezierTo(const Point &point1, const Point &point2, const Point &point3, bool, bool)
{
    //Debug::Log("BezierTo (%lf,%lf) (%lf,%lf) (%lf,%lf)", point1.X(), point1.Y(), point2.X(), point2.Y(), point3.X(), point3.Y());
    P();
    env_->CallVoidMethod(p, sm_Path_cubicTo, point1.X(), point1.Y(), point2.X(), point2.Y(), point3.X(), point3.Y());
    lastPoint_ = point3;
}

void AndroidStreamGeometryContext::PolyLineTo(const List<Point> &points, bool isStroked, bool isSmoothJoin)
{
    for (Point const & pt : points) {
        LineTo(pt, isStroked, isSmoothJoin);
    }
}

void AndroidStreamGeometryContext::PolyQuadraticBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.Count(); i += 2) {
        QuadraticBezierTo(points[i], points[i + 1], isStroked, isSmoothJoin);
    }
}

void AndroidStreamGeometryContext::PolyBezierTo(const List<Point> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.Count(); i += 3) {
        BezierTo(points[i], points[i + 1], points[i + 2], isStroked, isSmoothJoin);
    }
}

void AndroidStreamGeometryContext::ArcTo(const Point &point, const Size &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool, bool)
{
    P();
    Point lastp = lastPoint_;
    Point curtp = point;
    //Debug::Log("ArcTo (%lf,%lf) %lf (%lf,%lf) (%lf,%lf)", size.Width(), size.Height(),
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
    //Debug::Log("t (%lf,%lf)", t.X(), t.Y());
    // a1 = (t1 + t2) / 2, a2 = (t1 - t2) / 2; t1 > t2
    double a1 = atan(-t.X() / t.Y());
    double product = t.LengthSquared();
    if (product > 1.0) {
        //Debug::Log("product %lf (%lf,%lf)", product, t);
        if (!DoubleUtil::AreClose2(product, 1.0)) {
            Debug::Log("product %lf", product);
            LineTo(curtp, false, false);
            return;
        }
        product = 1.0;
    }
    double a2 = asin(product);
    //Debug::Log("a1 <-> a2 %lf %lf", (a1 * 180 / Math::PI), (a2 * 180 / Math::PI));
    if (isLargeArc != (sweepDirection == SweepDirection::Clockwise))
        a2 = -a2;
    if (a2 * t.Y() <= 0)
        a1 += Math::PI;
    double t1 = a1 + a2, t2 = a1 - a2;
    //Debug::Log("t1 <-> t2 %lf %lf", (t1 * 180 / Math::PI), (t2 * 180 / Math::PI));
    //Vector pt1(cos(t1), sin(t1));
    Vector pt2(cos(t2), sin(t2));
    //Debug::Log("pt1 <-> pt2 (%lf,%lf) (%lf,%lf)", pt1.X(), pt1.Y(), pt2.X(), pt2.Y());
    //t = (pt1 - pt2) / 2;
    //Debug::Log("t (%lf,%lf)", t.X(), t.Y());
    Point c = curtp - matrix.Transform(pt2);
    //
    Rect rect(-rx, -ry, rx * 2, ry * 2);
    //Point pe1(pt1.X() * rx, pt1.Y() * ry);
    //Point pe2(pt2.X() * rx, pt2.Y() * ry);
    //Debug::Log("pe1 <-> pe2 %lf %lf", pe1, pe2);
    a1 = 360 - t1 * 180 / Math::PI; // Y axis is up
    a2 = 360 - t2 * 180 / Math::PI;
    //Debug::Log("a1 <-> a2 %lf %lf", a1, a2);
    a2 -= a1;
    if (sweepDirection == SweepDirection::Clockwise) {
        if (a2 > 0)
            a2 -= 360;
    } else {
        if (a2 < 0)
            a2 += 360;
    }
    rect.Offset((c - rect.TopLeft()) - Vector(rect.GetSize()) / 2.0);
    env_->CallVoidMethod(p, sm_Path_arcTo, rect.Left(), rect.Top(), rect.Right(), rect.Bottom(),
                          360.0 - a1, -a2, false);
    lastPoint_ = point;
}

void AndroidStreamGeometryContext::SetClosedState(bool)
{
}

void AndroidStreamGeometryContext::Close()
{
    //Debug::Log("Close");
    P();
    env_->CallVoidMethod(p, sm_Path_close);
    StreamGeometryContext::Close();
}

void AndroidStreamGeometryContext::DisposeCore()
{
    //Debug::Log("DisposeCore");
    geometry_->Close(path_);
    path_ = nullptr;
}

INKCANVAS_END_NAMESPACE
