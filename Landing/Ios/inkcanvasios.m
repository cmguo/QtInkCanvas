#include "inkcanvasios.h"
#include "strokewrapper.h"

void NS_Log(char const * t, char const * m, va_list args)
{
    NSLogv([NSString stringWithFormat:@"%s %s", t, m], args);
}

@implementation ICStroke

- (instancetype) initWithPoints:(int) n points:(CGPoint[]) points pressures:(float[]) pressures
                              width:(double) width fitToCorve:(bool) fitToCorve
                              ellipseShape:(bool) ellipseShape addPressure:(bool) addPressure {
    double x[n];
    double y[n];
    for (int i = 0; i < n; ++i) {
        x[i] = points[i].x;
        y[i] = points[i].y;
    }
    _stroke = StrokeWrapper_new(n, x, y, pressures, width,
                             fitToCorve, ellipseShape, addPressure);
    return self;
}

- (ICStroke*) clone {
     ICStroke* sc = [ICStroke alloc];
     sc->_stroke = StrokeWrapper_clone(_stroke);
     return sc;
}

- (void) transformWithMatrix:(CGAffineTransform) matrix {
    double matrix2[6] = {matrix.a, matrix.b, matrix.c, matrix.d, matrix.tx, matrix.ty};
    StrokeWrapper_transform(_stroke, matrix2);
}

- (bool) hitTestWithPoint:(CGPoint) point {
    return StrokeWrapper_hitTest(_stroke, point.x, point.y);
}

- (UIBezierPath*) getGeometryAndBounds:(CGRect*) bounds {
    double b[4];
    void * path = StrokeWrapper_getGeometry(_stroke, b);
    if (bounds)
        *bounds = CGRectMake(b[0], b[1], b[2], b[3]);
    return (UIBezierPath*) path;
}

- (void) dealloc {
    StrokeWrapper_delete(_stroke);
    [super dealloc];
}

@end
