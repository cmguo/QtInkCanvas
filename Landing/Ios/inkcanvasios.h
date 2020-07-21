#ifndef INKCANVASIOS_H
#define INKCANVASIOS_H

#import "Foundation/Foundation.h"
#import "UIKit/UIBezierPath.h"

@interface ICStroke : NSObject {
  long _stroke;
}

- (instancetype) initWithPoints:(int) n points:(CGPoint[]) points pressures:(float[]) pressures
                              width:(double) width fitToCorve:(bool) fitToCorve
                              ellipseShape:(bool) ellipseShape addPressure:(bool) addPressure;
- (ICStroke*) clone;
- (void) transformWithMatrix:(CGAffineTransform) matrix;
- (bool) hitTestWithPoint:(CGPoint) point;
- (UIBezierPath*) getGeometryAndBounds:(CGRect*) bounds;
- (void) dealloc;
@end

#endif // INKCANVASIOS_H
