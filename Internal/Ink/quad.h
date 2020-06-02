#ifndef QUAD_H
#define QUAD_H

#include "InkCanvas_global.h"
#include "Windows/rect.h"
#include "Collections/Generic/list.h"

// namespace MS.Internal.Ink
INKCANVAS_BEGIN_NAMESPACE

class Quad
{
private:
    static Quad s_empty;

public:
    /// <summary> Returns the static object representing an empty (unitialized) quad </summary>
    static Quad Empty() { return s_empty; }

    /// <summary> Constructor </summary>
    Quad(Point a, Point b, Point c, Point d)
    {
        _A = a; _B = b; _C = c; _D = d;
    }

    /// <summary> The A vertex of the quad </summary>
public:
    Point& A() { return _A; }
    void SetA(Point const& value) { _A = value; }

    /// <summary> The B vertex of the quad </summary>
    Point& B() { return _B; }
    void SetB(Point const& value) { _B = value; }

    /// <summary> The C vertex of the quad </summary>
    Point& C() { return _C; }
    void SetC(Point const& value) { _C = value; }

    /// <summary> The D vertex of the quad </summary>
    Point& D() { return _D; }
    void SetD(Point const& value) { _D = value; }

    // Returns quad's vertex by index where A is of the index 0, B - is 1, etc
    Point & operator[](int index)
    {
        switch (index)
        {
            case 0: return _A;
            case 1: return _B;
            case 2: return _C;
            case 3: return _D;
            default:
                throw new std::runtime_error("index");
        }
    }

    /// <summary> Tells whether the quad is invalid (empty) </summary>
    bool IsEmpty()
    {
        return (_A == _B) && (_C == _D);
    }

    void GetPoints(List<Point> & pointBuffer)
    {
        pointBuffer.Add(_A);
        pointBuffer.Add(_B);
        pointBuffer.Add(_C);
        pointBuffer.Add(_D);
    }

    /// <summary> Returns the bounds of the quad </summary>
    Rect Bounds()
    {
        return IsEmpty() ? Rect() : Rect::Union(Rect(_A, _B), Rect(_C, _D));
    }

private:
    Point _A;
    Point _B;
    Point _C;
    Point _D;
};

INKCANVAS_END_NAMESPACE

#endif // QUAD_H
