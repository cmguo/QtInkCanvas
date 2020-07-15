#ifndef INKCANVASIOS_H
#define INKCANVASIOS_H

@interface InkCanvasIos : Object
{
- (long) newStrokeWithPoints:(CGPoint[]) points pressures:(float[]) pressures
                              width:(double) width fitToCorve:(bool) fitToCorve
                              ellipseShape:(bool) ellipseShape addPressure:(bool) addPressure;
- (long) cloneStroke:(long) stroke;
- (void) transformStroke:(long) stroke withMatrix:(CGAffineTransform) matrix;
- (bool) hitTestStroke:(long) stroke withPoint:(CGPoint) point;
- (UIBezierPath*) getStrokeGeometry:(long) stroke andBounds:(CGRect) bounds;
- (void) deleteStroke:(long) stroke;
};
@end

#endif // INKCANVASIOS_H
