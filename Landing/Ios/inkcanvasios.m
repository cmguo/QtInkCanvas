#include "inkcanvasios.h"
#include "strokewrapper.h"

@implementation InkCanvasIos

- (long) newStrokeWithPoints:(CGPoint[]) points pressures:(float[]) pressures
                              width:(double) width fitToCorve:(bool) fitToCorve
                              ellipseShape:(bool) ellipseShape addPressure:(bool) addPressure {
    int n;
    double x[];
    double y[];
    return StrokeWrapper_new(n, x, y, pressures, width,
                             fitToCorve, ellipseShape, addPressure);
}

- (long) cloneStroke:(long) stroke {
    return StrokeWrapper_clone(stroke);
}

- (void) transformStroke:(long) stroke withMatrix:(CGAffineTransform) matrix {
    double matrix2[6];
    StrokeWrapper_transform(stroke, matrix2);
}

- (bool) hitTestStroke:(long) stroke withPoint:(CGPoint) point {
    return StrokeWrapper_hitTest(stroke, point.x, point.y);
}

- (UIBezierPath*) getStrokeGeometry:(long) stroke andBounds:(CGRect) bounds {
    double b[4];
    void * path = StrokeWrapper_getGeometry(stroke, b);
    bounds = CGRectMake(b[0], b[1], b[2], b[3]);
    return (UIBezierPath*) path;
}

- (void) deleteStroke:(long) stroke {
    StrokeWrapper_delete(stroke);
}

@end