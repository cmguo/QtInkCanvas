#include "strokewrapper.h"

#include <Windows/Ink/drawingattributes.h>
#include <Windows/Ink/stroke.h>

#include <memory>
#include <vector>

#include <Windows/Input/styluspoint.h>
#include <Windows/Input/styluspointcollection.h>

#include <Windows/Media/streamgeometry.h>

INKCANVAS_USE_NAMESPACE

static std::vector<std::shared_ptr<Stroke>> strokes(1, nullptr);

#define S(stroke) \
    if (stroke >= static_cast<long>(strokes.size())) { \
        return F; \
    } \
    std::shared_ptr<Stroke> & s = strokes[static_cast<size_t>(stroke)]; \
    if (s == nullptr) { \
        return F; \
    }

class MyDrawingAttribute : public DrawingAttributes
{
public:
    MyDrawingAttribute(double width, bool fitToCorve, bool ellipseShape)
    {
        SetWidth(width);
        SetHeight(width);
        SetFitToCurve(fitToCorve);
        SetStylusTip(ellipseShape ? StylusTip::Ellipse : StylusTip::Rectangle);
    }
};


long StrokeWrapper_new(int n, double x[], double y[], float pressures[], double width, bool fitToCorve, bool ellipseShape, bool addPressure)
{
    //log("createStroke %lf", width);
    std::shared_ptr<DrawingAttributes> da(new MyDrawingAttribute(static_cast<double>(width), fitToCorve, ellipseShape));
    std::shared_ptr<StylusPointCollection> stylusPoints(
                new StylusPointCollection);
    float p = 0.5f; // pressure
    for (int i = 0; i < n; ++i) {
        if (pressures)
            p = pressures[i];
        StylusPoint stylusPoint(x[i], y[i], p);
        stylusPoints->Add(stylusPoint);
    }
    if (pressures == nullptr && addPressure) {
        int n = 16;
        if (stylusPoints->Count() > n) {
            for (int i = 1; i < n; ++i) {
                int m = stylusPoints->Count() + i - n;
                StylusPoint point = (*stylusPoints)[m];
                float d = static_cast<float>(i) / static_cast<float>(n);
                point.SetPressureFactor(point.PressureFactor() * (1.0f - d * d));
                stylusPoints->SetItem(m, point);
            }
            --n;
        } else {
            n = 0;
        }
    }
    std::shared_ptr<Stroke> s(new Stroke(stylusPoints, da));
    auto iter = std::find(strokes.begin() + 1, strokes.end(), nullptr);
    if (iter == strokes.end())
        iter = strokes.insert(iter, s);
    else
        *iter = s;
    return static_cast<long>(std::distance(strokes.begin(), iter));
}

long StrokeWrapper_clone(long stroke)
{
    //log("cloneStroke");
#undef F
#define F 0
    S(stroke)
    s = s->Clone();
    auto iter = std::find(strokes.begin() + 1, strokes.end(), nullptr);
    if (iter == strokes.end())
        iter = strokes.insert(iter, s);
    else
        *iter = s;
    return static_cast<long>(std::distance(strokes.begin(), iter));
}

void StrokeWrapper_transform(long stroke, double matrix[])
{
    //log("transformStroke");
#undef F
#define F
    S(stroke)
#define V(i) static_cast<double>(matrix[i])
    Matrix matrix2(V(0), V(1), V(2), V(3), V(4), V(5));
    s->Transform(matrix2, false);
}

bool StrokeWrapper_hitTest(long stroke, double x, double y)
{
    //log("hitTestStroke");
#undef F
#define F false
    S(stroke)
    return s->HitTest(Point(static_cast<double>(x), static_cast<double>(y)));
}

void *StrokeWrapper_getGeometry(long stroke, double bounds[])
{
    //log("getStrokeGeometry");
#undef F
#define F nullptr
    S(stroke)
    s->GetGeometry();
    void * path = static_cast<StreamGeometry*>(s->GetGeometry())->path();
    if (bounds) {
        Rect r = s->GetBounds();
        bounds[0] = r.Left(); bounds[1] = r.Top();
        bounds[2] = r.Width(); bounds[3] = r.Height();
    }
    return path;
}

void StrokeWrapper_delete(long stroke)
{
    //log("freeStroke");
#undef F
#define F
    S(stroke)
    s.reset();
}
