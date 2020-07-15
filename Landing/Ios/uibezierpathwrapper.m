#include "uibezierpathwrapper.h"

#import "UIBezierPath"

void * UIBezierPathWrapper_new()
{
    UIBezierPath *aPath = [UIBezierPath new];
    return aPath;
}

void UIBezierPathWrapper_moveToPoint(void * path, double x, double y)
{
    UIBezierPath *aPath = (id) path;
    [aPath moveToPoint:CGPointMake(x, y)];
}

void UIBezierPathWrapper_addLineToPoint(void * path, double x, double y)
{
    UIBezierPath *aPath = (id) path;
    [aPath addLineToPoint:CGPointMake(x, y)];
}

void UIBezierPathWrapper_addQuadCurveToPoint(void * path, double x, double y, double cx, double cy)
{
    UIBezierPath *aPath = (id) path;
    [aPath addQuadCurveToPoint:CGPointMake(x, y) controlPoint:CGPointMake(cx, cy)];
}

void UIBezierPathWrapper_addCurveToPoint(void * path, double x, double y, double cx1, double cy1, double cx2, double cy2)
{
    UIBezierPath *aPath = (id) path;
    [aPath addCurveToPoint:CGPointMake(x, y) controlPoint1:CGPointMake(cx1, cy1) controlPoint2:CGPointMake(cx2, cy2)];
}

void UIBezierPathWrapper_addArcWithCenter(void * path, double x, double y, double radius, double startAngle, double endAngle, bool clockwise)
{
    UIBezierPath *aPath = (id) path;
    [aPath addArcWithCenter:CGPointMake(x, y) radius:radius startAngle:startAngle endAngle:endAngle clockwise:clockwise];
}

void UIBezierPathWrapper_currentPoint(void * path, double & x, double & y)
{
    UIBezierPath *aPath = (id) path;
    CGPoint p = [aPath currentPoint];
    x = p.x; y = p.y;
}

void UIBezierPathWrapper_closePath(void * path)
{
    UIBezierPath *aPath = (id) path;
    [aPath closePath];
}

void UIBezierPathWrapper_delete(void * path)
{
}

