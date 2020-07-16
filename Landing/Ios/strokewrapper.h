#ifndef STROKEWRAPPER_H
#define STROKEWRAPPER_H

long StrokeWrapper_new(int n, double x[], double y[], float pressures[],
                       double width, int fitToCorve, int ellipseShape, int addPressure);
long StrokeWrapper_clone(long stroke);
void StrokeWrapper_transform(long stroke, double matrix[6]);
bool StrokeWrapper_hitTest(long stroke, double x, double y);
void * StrokeWrapper_getGeometry(long stroke, double bounds[4]);
void StrokeWrapper_delete(long stroke);

#endif // STROKEWRAPPER_H
