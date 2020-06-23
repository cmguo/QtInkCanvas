#ifndef EVENTS_H
#define EVENTS_H

#include "strokecollection.h"
#include "Windows/Input/styluspointcollection.h"
#include "extendedproperty.h"
#include "eventargs.h"

#include <QUuid>
#include <QVariant>
#include <QSharedPointer>

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

class StrokeCollectionChangedEventArgs : public EventArgs
{
private:
    QSharedPointer<StrokeCollection> _added;
    QSharedPointer<StrokeCollection> _removed;
    int _index = -1;

public:
    /// <summary>Constructor</summary>
    StrokeCollectionChangedEventArgs(QSharedPointer<StrokeCollection> added, QSharedPointer<StrokeCollection> removed, int index) :
        StrokeCollectionChangedEventArgs(added, removed)
    {
        _index = index;
    }

    /// <summary>Constructor</summary>
    StrokeCollectionChangedEventArgs(QSharedPointer<StrokeCollection> added, QSharedPointer<StrokeCollection> removed)
        : _added(added)
        , _removed(removed)
    {
        if ( added == nullptr && removed == nullptr )
        {
            throw std::runtime_error("added, removed");
        }
        //_added = ( added == null ) ? null : new StrokeCollection.ReadOnlyStrokeCollection(added);
        //_removed = ( removed == null ) ? null : new StrokeCollection.ReadOnlyStrokeCollection(removed);
    }

    /// <summary>Set of strokes that where added, result may be an empty collection</summary>
    QSharedPointer<StrokeCollection> Added()
    {
        if ( _added == nullptr )
        {
            _added = QSharedPointer<StrokeCollection>(new StrokeCollection());
        }
        return _added;
    }

    /// <summary>Set of strokes that where removed, result may be an empty collection</summary>
    QSharedPointer<StrokeCollection> Removed()
    {
        if ( _removed == nullptr )
        {
            _removed = QSharedPointer<StrokeCollection>(new StrokeCollection());
        }
        return _removed;
    }

    /// <summary>
    /// The zero based starting index that was affected
    /// </summary>
    int Index()
    {
        return _index;
    }
};


/// <summary>
/// Event arg used a change to the drawing attributes associated with one or more strokes has occurred.
/// </summary>
class PropertyDataChangedEventArgs : public EventArgs
{
private:
    QUuid _propertyGuid;
    QVariant _newValue;
    QVariant _previousValue;

public:
    /// <summary>Constructor</summary>
    PropertyDataChangedEventArgs(QUuid propertyGuid,
                                        QVariant newValue,
                                        QVariant previousValue)
    {
        if ( newValue.isNull() && previousValue.isNull() )
        {
            throw std::runtime_error("newValue, previousValue");
        }

        _propertyGuid = propertyGuid;
        _newValue = newValue;
        _previousValue = previousValue;
    }

    /// <summary>
    /// Gets the property QUuid that represents the DrawingAttribute that changed
    /// </summary>
    QUuid PropertyGuid()
    {
        return _propertyGuid;
    }

    /// <summary>
    /// Gets the new value of the DrawingAttribute
    /// </summary>
    QVariant NewValue()
    {
        return _newValue;
    }

    /// <summary>
    /// Gets the previous value of the DrawingAttribute
    /// </summary>
    QVariant PreviousValue()
    {
        return _previousValue;
    }
};



/// <summary>
/// Event Arg used when the Custom attributes associated with an QVariant have changed.
/// </summary>
class ExtendedPropertiesChangedEventArgs : public EventArgs
{
private:
    ExtendedProperty _oldProperty;
    ExtendedProperty _newProperty;

public:
    /// <summary>Constructor</summary>
    ExtendedPropertiesChangedEventArgs(ExtendedProperty oldProperty,
                                                ExtendedProperty newProperty)
        : _oldProperty(oldProperty)
        , _newProperty(newProperty)
    {
        //if ( oldProperty == null && newProperty == null )
        //{
        //    throw std::runtime_error("oldProperty");
        //}
        //_oldProperty = oldProperty;
        //_newProperty = newProperty;
    }

    /// <summary>
    /// The value of the previous property.  If the Changed event was caused
    /// by an ExtendedProperty being added, this value is null
    /// </summary>
    ExtendedProperty& OldProperty()
    {
        return _oldProperty;
    }

    /// <summary>
    /// The value of the new property.  If the Changed event was caused by
    /// an ExtendedProperty being removed, this value is null
    /// </summary>
    ExtendedProperty& NewProperty()
    {
        return _newProperty;
    }
};

class DrawingAttributes;

/// <summary>
///    DrawingAttributesReplacedEventArgs
/// </summary>
class DrawingAttributesReplacedEventArgs : public EventArgs
{
public:
    /// <summary>
    /// DrawingAttributesReplacedEventArgs
    /// </summary>
    /// <remarks>
    /// This must be so InkCanvas can instance it
    /// </remarks>
    DrawingAttributesReplacedEventArgs(QSharedPointer<DrawingAttributes> newDrawingAttributes, QSharedPointer<DrawingAttributes> previousDrawingAttributes)
        : _newDrawingAttributes(newDrawingAttributes)
        , _previousDrawingAttributes(previousDrawingAttributes)
    {
        //if ( newDrawingAttributes == null )
        //{
        //    throw std::runtime_error("newDrawingAttributes");
        //}
        //if ( previousDrawingAttributes == null )
        //{
        //    throw std::runtime_error("previousDrawingAttributes");
        //}
        //_newDrawingAttributes = newDrawingAttributes;
        //_previousDrawingAttributes = previousDrawingAttributes;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<DrawingAttributes> NewDrawingAttributes()
    {
        return _newDrawingAttributes;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<DrawingAttributes> PreviousDrawingAttributes()
    {
        return _previousDrawingAttributes;
    }

private:
    QSharedPointer<DrawingAttributes> _newDrawingAttributes;
    QSharedPointer<DrawingAttributes> _previousDrawingAttributes;
};


/// <summary>
///    StylusPointsReplacedEventArgs
/// </summary>
class StylusPointsReplacedEventArgs : public EventArgs
{
public:
    /// <summary>
    /// StylusPointsReplacedEventArgs
    /// </summary>
    /// <remarks>
    /// This must be so InkCanvas can instance it
    /// </remarks>
    StylusPointsReplacedEventArgs(QSharedPointer<StylusPointCollection> newStylusPoints, QSharedPointer<StylusPointCollection> previousStylusPoints)
        : _newStylusPoints(newStylusPoints)
        , _previousStylusPoints(previousStylusPoints)
    {
        //if ( newStylusPoints == null )
        //{
        //    throw std::runtime_error("newStylusPoints");
        //}
        //if ( previousStylusPoints == null )
        //{
        //    throw std::runtime_error("previousStylusPoints");
        //}
        //_newStylusPoints = newStylusPoints;
        //_previousStylusPoints = previousStylusPoints;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<StylusPointCollection> NewStylusPoints()
    {
        return _newStylusPoints;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    QSharedPointer<StylusPointCollection> PreviousStylusPoints()
    {
        return _previousStylusPoints;
    }

private:
    QSharedPointer<StylusPointCollection> _newStylusPoints;
    QSharedPointer<StylusPointCollection> _previousStylusPoints;
};

class PropertyChangedEventArgs : public EventArgs
{
private:
    QString propertyName;

public:
    /// <devdoc>
    /// <para>Initializes a new instance of the <see cref='System.ComponentModel.PropertyChangedEventArgs'/>
    /// class.</para>
    /// </devdoc>
    PropertyChangedEventArgs(QString propertyName)
        : propertyName(propertyName)
    {
        //propertyName = propertyName;
    }

    /// <devdoc>
    ///    <para>Indicates the name of the property that changed.</para>
    /// </devdoc>
    virtual QString PropertyName() {
        return propertyName;
    }
};


class CancelEventArgs : public EventArgs
{

private:
    /// <devdoc>
    ///     Indicates, on return, whether or not the operation should be cancelled
    ///     or not.  'true' means cancel it, 'false' means don't.
    /// </devdoc>
    bool cancel;

public:
    /// <devdoc>
    ///    <para>
    ///       Initializes a new instance of the <see cref='System.ComponentModel.CancelEventArgs'/> class with
    ///       cancel set to <see langword='false'/>.
    ///    </para>
    /// </devdoc>
    CancelEventArgs() : CancelEventArgs(false) {
    }

    /// <devdoc>
    ///    <para>
    ///       Initializes a new instance of the <see cref='System.ComponentModel.CancelEventArgs'/> class with
    ///       cancel set to the given value.
    ///    </para>
    /// </devdoc>
    CancelEventArgs(bool cancel)
        : cancel(cancel)
    {
    }

    /// <devdoc>
    ///    <para>
    ///       Gets or sets a value
    ///       indicating whether the operation should be cancelled.
    ///    </para>
    /// </devdoc>
    bool Cancel() {
        return cancel;
    }
    void SetCancel(bool value) {
        cancel = value;
    }
};

INKCANVAS_END_NAMESPACE

#endif // EVENTS_H
