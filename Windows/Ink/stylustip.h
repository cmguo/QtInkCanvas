#ifndef STYLUSTIP_H
#define STYLUSTIP_H

#include "InkCanvas_global.h"

#ifdef INKCANVAS_QT
#include <QMetaType>
#endif

INKCANVAS_BEGIN_NAMESPACE

enum class StylusTip
{
    /// <summary>
    /// Rectangle
    /// </summary>
    Rectangle = 0,

    /// <summary>
    /// Ellipse
    /// </summary>
    Ellipse
};

// namespace System.Windows.Ink

class StylusTipHelper
{
public:
    static bool IsDefined(StylusTip stylusTip)
    {
        if (stylusTip < StylusTip::Rectangle || stylusTip > StylusTip::Ellipse)
        {
            return false;
        }
        return true;
    }
};

INKCANVAS_END_NAMESPACE

#ifdef INKCANVAS_QT
Q_DECLARE_METATYPE(INKCANVAS_PREPEND_NAMESPACE(StylusTip))
#endif

#endif // STYLUSTIP_H
