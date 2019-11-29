#ifndef HITTESTRESULT_H
#define HITTESTRESULT_H

class DependencyObject;
/// <summary>
/// This base returns the visual that was hit during a hit test pass.
/// </summary>
class HitTestResult
{
private:
    DependencyObject* _visualHit;

    friend bool operator==(HitTestResult const & l, HitTestResult const & r)
    {
        return l._visualHit == r._visualHit;
    }

    friend bool operator!=(HitTestResult const & l, HitTestResult const & r)
    {
        return !(l == r);
    }

public:
    HitTestResult(DependencyObject* visualHit)
    {
        _visualHit = visualHit;
    }

    /// <summary>
    /// Returns the visual that was hit.  May be a Visual or Visual3D.
    /// </summary>
    DependencyObject* VisualHit()
    {
        return _visualHit;
    }
};

#endif // HITTESTRESULT_H
