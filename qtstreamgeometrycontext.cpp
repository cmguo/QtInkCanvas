#include "qtstreamgeometrycontext.h"
#include "Windows/Media/streamgeometry.h"

#include <QtMath>
#include <QDebug>

QtStreamGeometryContext::QtStreamGeometryContext(StreamGeometry* geometry)
    : geometry_(geometry)
{
    path_.setFillRule(Qt::FillRule::WindingFill);
}

void QtStreamGeometryContext::BeginFigure(const QPointF &startPoint, bool isFilled, bool isClosed)
{
    if (isStarted_) {
        if (isClosed_)
            path_.closeSubpath();
    } else {
        isStarted_ = true;
    }
    isFilled_ = isFilled;
    isClosed_ = isClosed;
    path_.moveTo(startPoint);
}

void QtStreamGeometryContext::LineTo(const QPointF &point, bool isStroked, bool isSmoothJoin)
{
    path_.lineTo(point);
}

void QtStreamGeometryContext::QuadraticBezierTo(const QPointF &point1, const QPointF &point2, bool isStroked, bool isSmoothJoin)
{
    path_.quadTo(point1, point2);
}

void QtStreamGeometryContext::BezierTo(const QPointF &point1, const QPointF &point2, const QPointF &point3, bool isStroked, bool isSmoothJoin)
{
    path_.cubicTo(point1, point2, point3);
}

void QtStreamGeometryContext::PolyLineTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin)
{
    for (QPointF const & pt : points)
        path_.lineTo(pt);
}

void QtStreamGeometryContext::PolyQuadraticBezierTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.size(); i += 2) {
        path_.quadTo(points[i], points[i + 1]);
    }
}

void QtStreamGeometryContext::PolyBezierTo(const QList<QPointF> &points, bool isStroked, bool isSmoothJoin)
{
    for (int i = 0; i < points.size(); i += 3) {
        path_.cubicTo(points[i], points[i + 1], points[i + 2]);
    }
}


void QtStreamGeometryContext::ArcTo(const QPointF &point, const QSizeF &size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection, bool isStroked, bool isSmoothJoin)
{
    //path_.lineTo(point);
    //return;
    qDebug() << path_.currentPosition() << point << size << rotationAngle << isLargeArc << sweepDirection;
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
    rotationAngle = rotationAngle * M_PI / 180;
    //QPointF rotate(cos(rotationAngle), sin(rotationAngle)); // [cos(r), sin(r)]
    qreal rx = size.width(), ry = size.height();
    //QMatrix matrix(rx * rotate.x(), rx * rotate.y(),
    //            -ry * rotate.y(), ry * rotate.x(), 0, 0);
    QMatrix matrix(rx, 0, 0, ry, 0, 0);
    // t = [pt1 - pt2] / 2 = [cos(t1) - cos(t2), sin(t1) - sin(t2)] / 2;
    //                     = [-sin((t1 + t2) / 2) * sin((t1 - t2) / 2), cos((t1 + t2) / 2) * sin((t1 - t2) / 2)]
    QPointF t = matrix.inverted().map(path_.currentPosition() - point) / 2;
    qDebug() << "t" << t;
    // a1 = (t1 + t2) / 2, a2 = (t1 - t2) / 2; t1 > t2
    qreal a1 = atan(-t.x() / t.y());
    qreal product = QPointF::dotProduct(t, t);
    if (product > 1.0) {
        qDebug() << "product" << product;
        path_.lineTo(point);
        return;
    }
    qreal a2 = asin(product);
    qDebug() << "a1 <-> a2" << (a1 * 180 / M_PI) << (a2 * 180 / M_PI);
    if (isLargeArc != (sweepDirection == SweepDirection::Clockwise))
        a2 = -a2;
    if (a2 * t.y() < 0)
        a1 += M_PI;
    qreal t1 = a1 + a2, t2 = a1 - a2;
    qDebug() << "t1 <-> t2" << (t1 * 180 / M_PI) << (t2 * 180 / M_PI);
    QPointF pt1(cos(t1), sin(t1));
    QPointF pt2(cos(t2), sin(t2));
    qDebug() << "pt1 <-> pt2" << pt1 << pt2;
    qDebug() << "t" << (pt1 - pt2) / 2;
    QPointF c = point - matrix.map(pt2);
    //
    QRectF rect(-rx, -ry, rx * 2, ry * 2);
    QPointF pe1(pt1.x() * rx, pt1.y() * ry);
    QPointF pe2(pt2.x() * rx, pt2.y() * ry);
    qDebug() << "pe1 <-> pe2" << pe1 << pe2;
    a1 = 360 - t1 * 180 / M_PI; // Y axis is up
    a2 = 360 - t2 * 180 / M_PI;
    qDebug() << "a1 <-> a2" << a1 << a2;
    QPainterPath ph(pe1);
    a2 -= a1;
    if (sweepDirection == SweepDirection::Clockwise) {
        if (a2 > 0)
            a2 -= 360;
    } else {
        if (a2 < 0)
            a2 += 360;
    }
    rect.moveCenter(c);
    path_.arcTo(rect, a1, a2);
    //path_.addPath(QMatrix(rotate.x(), rotate.y(), -rotate.y(), rotate.x(), c.x(), c.y()).map(ph));
    //path_.moveTo(point);
}

void QtStreamGeometryContext::DisposeCore()
{
    if (isStarted_) {
        if (isClosed_)
            path_.closeSubpath();
        isStarted_ = false;
    }
    geometry_->Close(path_);
    path_ = QPainterPath();
}

void QtStreamGeometryContext::SetClosedState(bool closed)
{
    isClosed_ = closed;
}
