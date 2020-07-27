#ifndef NSBezierPathWRAPPER_H
#define NSBezierPathWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void * NSBezierPathWrapper_new();
void NSBezierPathWrapper_moveToPoint(void * path, double x, double y);
void NSBezierPathWrapper_addLineToPoint(void * path, double x, double y);
void NSBezierPathWrapper_addQuadCurveToPoint(void * path, double x, double y, double cx, double cy);
void NSBezierPathWrapper_addCurveToPoint(void * path, double x, double y, double cx1, double cy1, double cx2, double cy2);
void NSBezierPathWrapper_addArcWithCenter(void * path, double x, double y, double radius, double startAngle, double endAngle, int clockwise);
void NSBezierPathWrapper_currentPoint(void * path, double * x, double * y);
void NSBezierPathWrapper_closePath(void * path);
void NSBezierPathWrapper_delete(void * path);

#ifdef __cplusplus
}
#endif

#endif // NSBezierPathWRAPPER_H
