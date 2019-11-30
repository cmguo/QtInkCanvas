#ifndef STYLUSTIP_H
#define STYLUSTIP_H

#include <QMetaType>

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

#endif // STYLUSTIP_H
