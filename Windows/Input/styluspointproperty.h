#ifndef STYLUSPOINTPROPERTY_H
#define STYLUSPOINTPROPERTY_H

#include "Windows/Input/styluspointpropertyids.h"

#ifdef INKCANVAS_QT
#include <QString>
#endif

// namespace System.Windows.Input
INKCANVAS_BEGIN_NAMESPACE

class StylusPointProperty
{
public:
    /// <summary>
    /// Instance data
    /// </summary>
private:
    Guid const & _id;
    bool _isButton;

    /// <summary>
    /// StylusPointProperty
    /// </summary>
    /// <param name="identifier">identifier
    /// <param name="isButton">isButton
public:
    static StylusPointProperty const Empty;

    StylusPointProperty() : _id(Guid::Empty), _isButton(false) {}

    StylusPointProperty(Guid const & identifier, bool isButton)
        : _id(identifier)
        , _isButton(isButton)
    {
        Initialize(identifier, isButton);
    }

    /// <summary>
    /// StylusPointProperty
    /// </summary>
    /// <param name="stylusPointProperty">
    /// <remarks>Protected - used by the StylusPointPropertyInfo ctor</remarks>
    StylusPointProperty(StylusPointProperty const & stylusPointProperty)
        : _id(stylusPointProperty._id)
        , _isButton(stylusPointProperty._isButton)
    {
        //Initialize(stylusPointProperty._id, stylusPointProperty._isButton);
    }

    /// <summary>
    /// Common ctor helper
    /// </summary>
    /// <param name="identifier">identifier
    /// <param name="isButton">isButton
private:
    void Initialize(Guid const & identifier, bool isButton);

public:
    /// <summary>
    /// Id
    /// </summary>
    Guid const & Id() const
    {
        return _id;
    }

    /// <summary>
    /// IsButton
    /// </summary>
    bool IsButton() const
    {
        return _isButton;
    }

#ifdef INKCANVAS_QT
    /// <summary>
    /// Returns a human readable string representation
    /// </summary>
    QString ToString() const
    {
        return QString("{Id=") +
            StylusPointPropertyIds::GetStringRepresentation(_id) +
            ", IsButton=" +
            _isButton +
            "}";
    }
#endif

};

INKCANVAS_END_NAMESPACE

#endif // STYLUSPOINTPROPERTY_H
