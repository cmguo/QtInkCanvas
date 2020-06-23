#ifndef STYLUSTIP_H
#define STYLUSTIP_H

#include "InkCanvas_global.h"

#include <QMetaType>

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

Q_DECLARE_METATYPE(StylusTip)

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

#endif // STYLUSTIP_H
