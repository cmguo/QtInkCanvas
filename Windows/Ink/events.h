#ifndef WINDOWS_INK_EVENTS_H
#define WINDOWS_INK_EVENTS_H

#include "strokecollection.h"
#include "Windows/Input/styluspointcollection.h"
#include "extendedproperty.h"
#include "eventargs.h"

#include "guid.h"
#include "variant.h"
#include "sharedptr.h"

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

class StrokeCollectionChangedEventArgs : public EventArgs
{
private:
    SharedPointer<StrokeCollection> _added;
    SharedPointer<StrokeCollection> _removed;
    int _index = -1;

public:
    /// <summary>Constructor</summary>
    StrokeCollectionChangedEventArgs(SharedPointer<StrokeCollection> added, SharedPointer<StrokeCollection> removed, int index) :
        StrokeCollectionChangedEventArgs(added, removed)
    {
        _index = index;
    }

    /// <summary>Constructor</summary>
    StrokeCollectionChangedEventArgs(SharedPointer<StrokeCollection> added, SharedPointer<StrokeCollection> removed)
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
    SharedPointer<StrokeCollection> Added()
    {
        if ( _added == nullptr )
        {
            _added = SharedPointer<StrokeCollection>(new StrokeCollection());
        }
        return _added;
    }

    /// <summary>Set of strokes that where removed, result may be an empty collection</summary>
    SharedPointer<StrokeCollection> Removed()
    {
        if ( _removed == nullptr )
        {
            _removed = SharedPointer<StrokeCollection>(new StrokeCollection());
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
    Guid _propertyGuid;
    Variant _newValue;
    Variant _previousValue;

public:
    /// <summary>Constructor</summary>
    PropertyDataChangedEventArgs(Guid propertyGuid,
                                        Variant newValue,
                                        Variant previousValue)
    {
        if ( newValue == nullptr && previousValue == nullptr )
        {
            throw std::runtime_error("newValue, previousValue");
        }

        _propertyGuid = propertyGuid;
        _newValue = newValue;
        _previousValue = previousValue;
    }

    /// <summary>
    /// Gets the property Guid that represents the DrawingAttribute that changed
    /// </summary>
    Guid PropertyGuid()
    {
        return _propertyGuid;
    }

    /// <summary>
    /// Gets the new value of the DrawingAttribute
    /// </summary>
    Variant NewValue()
    {
        return _newValue;
    }

    /// <summary>
    /// Gets the previous value of the DrawingAttribute
    /// </summary>
    Variant PreviousValue()
    {
        return _previousValue;
    }
};



/// <summary>
/// Event Arg used when the Custom attributes associated with an Variant have changed.
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
    DrawingAttributesReplacedEventArgs(SharedPointer<DrawingAttributes> newDrawingAttributes, SharedPointer<DrawingAttributes> previousDrawingAttributes)
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
    SharedPointer<DrawingAttributes> NewDrawingAttributes()
    {
        return _newDrawingAttributes;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    SharedPointer<DrawingAttributes> PreviousDrawingAttributes()
    {
        return _previousDrawingAttributes;
    }

private:
    SharedPointer<DrawingAttributes> _newDrawingAttributes;
    SharedPointer<DrawingAttributes> _previousDrawingAttributes;
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
    StylusPointsReplacedEventArgs(SharedPointer<StylusPointCollection> newStylusPoints, SharedPointer<StylusPointCollection> previousStylusPoints)
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
    SharedPointer<StylusPointCollection> NewStylusPoints()
    {
        return _newStylusPoints;
    }

    /// <summary>
    /// [TBS]
    /// </summary>
    SharedPointer<StylusPointCollection> PreviousStylusPoints()
    {
        return _previousStylusPoints;
    }

private:
    SharedPointer<StylusPointCollection> _newStylusPoints;
    SharedPointer<StylusPointCollection> _previousStylusPoints;
};

class PropertyChangedEventArgs : public EventArgs
{
private:
    char const * propertyName;

public:
    /// <devdoc>
    /// <para>Initializes a new instance of the <see cref='System.ComponentModel.PropertyChangedEventArgs'/>
    /// class.</para>
    /// </devdoc>
    PropertyChangedEventArgs(char const * propertyName)
        : propertyName(propertyName)
    {
        //propertyName = propertyName;
    }

    /// <devdoc>
    ///    <para>Indicates the name of the property that changed.</para>
    /// </devdoc>
    char const * PropertyName() {
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

#endif // WINDOWS_INK_EVENTS_H
