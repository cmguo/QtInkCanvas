#ifndef QUAD_H
#define QUAD_H

#include <QPointF>
#include <QRectF>
#include <QList>

// namespace MS.Internal.Ink

class Quad
{
private:
    static Quad s_empty;

public:
    /// <summary> Returns the static object representing an empty (unitialized) quad </summary>
    static Quad Empty() { return s_empty; }

    /// <summary> Constructor </summary>
    Quad(QPointF a, QPointF b, QPointF c, QPointF d)
    {
        _A = a; _B = b; _C = c; _D = d;
    }

    /// <summary> The A vertex of the quad </summary>
public:
    QPointF& A() { return _A; }
    void SetA(QPointF const& value) { _A = value; }

    /// <summary> The B vertex of the quad </summary>
    QPointF& B() { return _B; }
    void SetB(QPointF const& value) { _B = value; }

    /// <summary> The C vertex of the quad </summary>
    QPointF& C() { return _C; }
    void SetC(QPointF const& value) { _C = value; }

    /// <summary> The D vertex of the quad </summary>
    QPointF& D() { return _D; }
    void SetD(QPointF const& value) { _D = value; }

    // Returns quad's vertex by index where A is of the index 0, B - is 1, etc
    QPointF & operator[](int index)
    {
        switch (index)
        {
            case 0: return _A;
            case 1: return _B;
            case 2: return _C;
            case 3: return _D;
            default:
                throw new std::exception("index");
        }
    }

    /// <summary> Tells whether the quad is invalid (empty) </summary>
    bool IsEmpty()
    {
        return (_A == _B) && (_C == _D);
    }

    void GetPoints(QList<QPointF> & pointBuffer)
    {
        pointBuffer.append(_A);
        pointBuffer.append(_B);
        pointBuffer.append(_C);
        pointBuffer.append(_D);
    }

    /// <summary> Returns the bounds of the quad </summary>
    QRectF Bounds()
    {
        return IsEmpty() ? QRectF() : QRectF(_A, _B).normalized().united(QRectF(_C, _D).normalized());
    }

private:
    QPointF _A;
    QPointF _B;
    QPointF _C;
    QPointF _D;
};

#endif // QUAD_H
