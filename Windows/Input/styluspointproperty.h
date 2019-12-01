#ifndef STYLUSPOINTPROPERTY_H
#define STYLUSPOINTPROPERTY_H

#include "styluspointpropertyids.h"

#include <QString>

// namespace System.Windows.Input

class StylusPointProperty
{
public:
    /// <summary>
    /// Instance data
    /// </summary>
private:
    QUuid _id;
    bool _isButton;

    /// <summary>
    /// StylusPointProperty
    /// </summary>
    /// <param name="identifier">identifier
    /// <param name="isButton">isButton
public:
    StylusPointProperty() {}

    StylusPointProperty(QUuid const & identifier, bool isButton)
    {
        Initialize(identifier, isButton);
    }

    /// <summary>
    /// StylusPointProperty
    /// </summary>
    /// <param name="stylusPointProperty">
    /// <remarks>Protected - used by the StylusPointPropertyInfo ctor</remarks>
    StylusPointProperty(StylusPointProperty const & stylusPointProperty)
    {
        Initialize(stylusPointProperty._id, stylusPointProperty._isButton);
    }

    /// <summary>
    /// Common ctor helper
    /// </summary>
    /// <param name="identifier">identifier
    /// <param name="isButton">isButton
private:
    void Initialize(QUuid const & identifier, bool isButton);

public:
    /// <summary>
    /// Id
    /// </summary>
    QUuid Id() const
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

    /// <summary>
    /// Returns a human readable string representation
    /// </summary>
    QString ToString() const
    {
        return "{Id=" +
            StylusPointPropertyIds::GetStringRepresentation(_id) +
            ", IsButton=" +
            _isButton +
            "}";
    }
};

#endif // STYLUSPOINTPROPERTY_H
