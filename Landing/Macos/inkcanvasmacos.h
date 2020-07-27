#ifndef INKCANVASIOS_H
#define INKCANVASIOS_H

#import "Foundation/Foundation.h"
#import "AppKit/NSBezierPath.h"

@interface InkCanvasMacos : NSObject {}
- (long) newStrokeWithPoints:(int) n points:(CGPoint[]) points pressures:(float[]) pressures
                              width:(double) width fitToCorve:(bool) fitToCorve
                              ellipseShape:(bool) ellipseShape addPressure:(bool) addPressure;
- (long) cloneStroke:(long) stroke;
- (void) transformStroke:(long) stroke withMatrix:(CGAffineTransform) matrix;
- (bool) hitTestStroke:(long) stroke withPoint:(CGPoint) point;
- (NSBezierPath*) getStrokeGeometry:(long) stroke andBounds:(CGRect) bounds;
- (void) deleteStroke:(long) stroke;
@end

#endif // INKCANVASIOS_H
