#include "nsbezierpathwrapper.h"

#import "AppKit/NSBezierPath.h"

void * NSBezierPathWrapper_new()
{
    NSBezierPath *aPath = [NSBezierPath new];
    return aPath;
}

void NSBezierPathWrapper_moveToPoint(void * path, double x, double y)
{
    NSBezierPath *aPath = (id) path;
    [aPath moveToPoint:CGPointMake(x, y)];
}

void NSBezierPathWrapper_addLineToPoint(void * path, double x, double y)
{
    NSBezierPath *aPath = (id) path;
    [aPath lineToPoint:CGPointMake(x, y)];
}

void NSBezierPathWrapper_addQuadCurveToPoint(void * path, double x, double y, double cx, double cy)
{
    NSBezierPath *aPath = (id) path;
    [aPath quadCurveToPoint:CGPointMake(x, y) controlPoint:CGPointMake(cx, cy)];
}

void NSBezierPathWrapper_addCurveToPoint(void * path, double x, double y, double cx1, double cy1, double cx2, double cy2)
{
    NSBezierPath *aPath = (id) path;
    [aPath curveToPoint:CGPointMake(x, y) controlPoint1:CGPointMake(cx1, cy1) controlPoint2:CGPointMake(cx2, cy2)];
}

void NSBezierPathWrapper_addArcWithCenter(void * path, double x, double y, double radius, double startAngle, double endAngle, int clockwise)
{
    NSBezierPath *aPath = (id) path;
    [aPath appendBezierPathWithArcWithCenter:CGPointMake(x, y) radius:radius startAngle:startAngle endAngle:endAngle clockwise:clockwise];
}

void NSBezierPathWrapper_currentPoint(void * path, double * x, double * y)
{
    NSBezierPath *aPath = (id) path;
    CGPoint p = [aPath currentPoint];
    *x = p.x; *y = p.y;
}

void NSBezierPathWrapper_closePath(void * path)
{
    NSBezierPath *aPath = (id) path;
    [aPath closePath];
}

void NSBezierPathWrapper_delete(void * path)
{
}

