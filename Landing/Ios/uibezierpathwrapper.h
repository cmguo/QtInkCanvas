#ifndef UIBEZIERPATHWRAPPER_H
#define UIBEZIERPATHWRAPPER_H

void * UIBezierPathWrapper_new();
void UIBezierPathWrapper_moveToPoint(void * path, double x, double y);
void UIBezierPathWrapper_addLineToPoint(void * path, double x, double y);
void UIBezierPathWrapper_addQuadCurveToPoint(void * path, double x, double y, double cx, double cy);
void UIBezierPathWrapper_addCurveToPoint(void * path, double x, double y, double cx1, double cy1, double cx2, double cy2);
void UIBezierPathWrapper_addArcWithCenter(void * path, double x, double y, double radius, double startAngle, double endAngle, int clockwise);
void UIBezierPathWrapper_currentPoint(void * path, double * x, double * y);
void UIBezierPathWrapper_closePath(void * path);
void UIBezierPathWrapper_delete(void * path);

#endif // UIBEZIERPATHWRAPPER_H
