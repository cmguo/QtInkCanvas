#ifndef STYLUSTIP_H
#define STYLUSTIP_H

enum StylusTip
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
