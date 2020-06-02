#ifndef GUID_H
#define GUID_H

#include "InkCanvas_global.h"

#ifndef INKCANVAS_CORE
#include <QUuid>
#endif

INKCANVAS_BEGIN_NAMESPACE

struct Guid
{
public:
    static Guid Empty;

    typedef unsigned char byte;

    ////////////////////////////////////////////////////////////////////////////////
    //  Member variables
    ////////////////////////////////////////////////////////////////////////////////
private:
    int         _a;
    short       _b;
    short       _c;
    byte       _d;
    byte       _e;
    byte       _f;
    byte       _g;
    byte       _h;
    byte       _i;
    byte       _j;
    byte       _k;

public:
    ////////////////////////////////////////////////////////////////////////////////
    //  Constructors
    ////////////////////////////////////////////////////////////////////////////////

    // Creates a new guid from an array of bytes.
    //
    Guid() { _a = 0; _b = _c = 0; _d = _e = _f = _g = _h = _i = _j = _k = 0; }

    Guid(byte b[])
    {
        _a = ((int)b[3] << 24) | ((int)b[2] << 16) | ((int)b[1] << 8) | b[0];
        _b = (short)(((int)b[5] << 8) | b[4]);
        _c = (short)(((int)b[7] << 8) | b[6]);
        _d = b[8];
        _e = b[9];
        _f = b[10];
        _g = b[11];
        _h = b[12];
        _i = b[13];
        _j = b[14];
        _k = b[15];
    }

    constexpr Guid (unsigned int a, unsigned short b, unsigned short c, byte d, byte e, byte f, byte g, byte h, byte i, byte j, byte k)
        : _a((int)a)
        , _b((short)b)
        , _c((short)c)
        , _d(d), _e(e), _f(f), _g(g), _h(h), _i(i), _j(j), _k(k)
    {
    }


    // Creates a new GUID initialized to the value represented by the arguments.
    //
    Guid(int a, short b, short c, byte d[])
    {
        _a  = a;
        _b  = b;
        _c  = c;
        _d = d[0];
        _e = d[1];
        _f = d[2];
        _g = d[3];
        _h = d[4];
        _i = d[5];
        _j = d[6];
        _k = d[7];
    }

    // Creates a new GUID initialized to the value represented by the
    // arguments.  The bytes are specified like this to avoid endianness issues.
    //
    constexpr Guid(int a, short b, short c, byte d, byte e, byte f, byte g, byte h, byte i, byte j, byte k)
        : _a(a)
        , _b(b)
        , _c(c)
        , _d(d), _e(e), _f(f), _g(g), _h(h), _i(i), _j(j), _k(k)
    {
    }

    friend bool operator==(Guid const & l, Guid const & r)
    {
        return l._a == r._a && l._b == r._b && l._c == r._c
                && l._d == r._d && l._e == r._e && l._f == r._f
                && l._g == r._g && l._h == r._h && l._i == r._i
                && l._j== r._j && l._k == r._k;
    }

    friend bool operator!=(Guid const & l, Guid const & r)
    {
        return !(l == r);
    }

#ifndef INKCANVAS_CORE
    operator QUuid() const
    {
        return QUuid(_a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k);
    }
#endif
};

INKCANVAS_END_NAMESPACE

#endif // GUID_H
