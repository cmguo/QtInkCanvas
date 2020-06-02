#include "Windows/Ink/extendedpropertycollection.h"
#include "Windows/Ink/events.h"
#include "Internal/debug.h"

INKCANVAS_BEGIN_NAMESPACE

/// <summary>
/// Create a new empty ExtendedPropertyCollection
/// </summary>
ExtendedPropertyCollection::ExtendedPropertyCollection()
{
}

/// <summary>Overload of the Equals method which determines if two ExtendedPropertyCollection
/// objects contain equivalent key/value pairs</summary>
bool ExtendedPropertyCollection::Equals(ExtendedPropertyCollection const & that) const
{
    //
    // compare counts
    //
    if (that.Count() != Count())
    {
        return false;
    }

    //
    // counts are equal, compare individual items
    //
    //
    for (int x = 0; x < that.Count(); x++)
    {
        bool cont = false;
        for (int i = 0; i < _extendedProperties.Count(); i++)
        {
            if (_extendedProperties[i] == that[x])
            {
                cont = true;
                break;
            }
        }
        if (!cont)
        {
            return false;
        }
    }
    return true;
}



/// <summary>
/// Check to see if the attribute is defined in the collection.
/// </summary>
/// <param name="attributeId">Attribute identifier</param>
/// <returns>True if attribute is set in the mask, false otherwise</returns>
bool ExtendedPropertyCollection::Contains(Guid const & attributeId) const
{
    for (int x = 0; x < _extendedProperties.Count(); x++)
    {
        if (_extendedProperties[x].Id() == attributeId)
        {
            //
            // a typical pattern is to first check if
            // ep.Contains(guid)
            // before accessing:
            // object o = ep[guid];
            //
            // I'm caching the index that contains returns so that we
            // can look there first for the guid in the indexer
            //
            _optimisticIndex = x;
            return true;
        }
    }
    return false;
}

/// <summary>
/// Copies the ExtendedPropertyCollection
/// </summary>
/// <returns>Copy of the ExtendedPropertyCollection</returns>
/// <remarks>Any reference types held in the collection will only be deep copied (e.g. Arrays).
/// </remarks>
ExtendedPropertyCollection* ExtendedPropertyCollection::Clone() const
{
    ExtendedPropertyCollection* copied = new ExtendedPropertyCollection();
    for (int x = 0; x < _extendedProperties.Count(); x++)
    {
        copied->Add(_extendedProperties[x]);
    }
    return copied;
}

/// <summary>
/// Add
/// </summary>
/// <param name="id">Id</param>
/// <param name="value">value</param>
void ExtendedPropertyCollection::Add(Guid const& id, Variant const & value)
{
    if (Contains(id))
    {
        throw std::runtime_error("id");
    }

    ExtendedProperty extendedProperty(id, value);

    //this will raise change events
    Add(extendedProperty);
}


/// <summary>
/// Remove
/// </summary>
/// <param name="id">id</param>
void ExtendedPropertyCollection::Remove(Guid const& id)
{
    if (!Contains(id))
    {
        throw std::runtime_error("id");
    }

    ExtendedProperty* propertyToRemove = GetExtendedPropertyById(id);
    Debug::Assert(propertyToRemove != nullptr );
    ExtendedPropertiesChangedEventArgs eventArgs(*propertyToRemove, ExtendedProperty::Empty );

    _extendedProperties.Remove(*propertyToRemove);

    //
    // this value is bogus now
    //
    _optimisticIndex = -1;

#ifdef INKCANVAS_QT
    // fire notification event
    //if (this.Changed != nullptr )
    {
        Changed(eventArgs);
    }
#endif
}

/// <value>
///     Retrieve the Guid const& array of ExtendedProperty Ids  in the collection.
///     <paramref>Guid[]</paramref> is of type <see cref="System.Int32"/>.
///     <seealso cref="System.Collections.ICollection.Count"/>
/// </value>
Array<Guid> ExtendedPropertyCollection::GetGuidArray() const
{
    if (_extendedProperties.Count() > 0)
    {
        Array<Guid> guids(_extendedProperties.Count());
        for (int i = 0; i < _extendedProperties.Count(); i++)
        {
            guids[i] = (*this)[i].Id();
        }
        return guids;
    }
    else
    {
        return Array<Guid>();
    }
}

/// <summary>
/// Generic accessor for the ExtendedPropertyCollection.
/// </summary>
/// <param name="attributeId">Attribue Id to find</param>
/// <returns>Value for attribute specified by Id</returns>
/// <exception cref="System.ArgumentException">Specified identifier was not found</exception>
/// <remarks>
/// Note that you can access extended properties via this indexer.
/// </remarks>
Variant ExtendedPropertyCollection::operator[](Guid const& attributeId) const
{
        ExtendedProperty* ep = const_cast<ExtendedPropertyCollection*>(this)->GetExtendedPropertyById(attributeId);
        if (ep == nullptr )
        {
            throw std::runtime_error("attributeId");
        }
        return ep->Value();
}

void ExtendedPropertyCollection::Set(Guid const& attributeId, Variant const & value)
{
    if (value == nullptr )
    {
        throw std::runtime_error("value");
    }
    for (int i = 0; i < _extendedProperties.Count(); i++)
    {
        ExtendedProperty& currentProperty = _extendedProperties[i];

        if (currentProperty.Id() == attributeId)
        {
            Variant oldValue = currentProperty.Value();
            //this will raise events
            currentProperty.SetValue(value);

#ifdef INKCANVAS_QT
            //raise change if anyone is listening
            //if (this.Changed != nullptr )
            {
                ExtendedPropertiesChangedEventArgs eventArgs(
                        ExtendedProperty(currentProperty.Id(), oldValue), //old prop
                        currentProperty);                                   //new prop
                Changed( eventArgs);
            }
#endif
            return;
        }
    }

    //
    //  we didn't find the Id in the collection, we need to add it.
    //  this will raise change notifications
    //
    ExtendedProperty attributeToAdd(attributeId, value);
    Add(attributeToAdd);
}


/// <summary>
/// private Add, we need to consider making this public in order to implement the generic ICollection
/// </summary>
void ExtendedPropertyCollection::Add(ExtendedProperty const & extendedProperty)
{
    Debug::Assert(!Contains(extendedProperty.Id()), "ExtendedProperty already belongs to the collection");

    _extendedProperties.Add(extendedProperty);
#ifdef INKCANVAS_QT
    // fire notification event
    //if (this.Changed != nullptr )
    {
        ExtendedPropertiesChangedEventArgs eventArgs(ExtendedProperty::Empty, extendedProperty);
        Changed(eventArgs);
    }
#endif
}

/// <summary>
/// Private helper for getting an EP out of our internal collection
/// </summary>
/// <param name="id">id</param>
ExtendedProperty* ExtendedPropertyCollection::GetExtendedPropertyById(Guid const& id)
{
    //
    // a typical pattern is to first check if
    // ep.Contains(guid)
    // before accessing:
    // object o = ep[guid];
    //
    // The last call to .Contains sets this index
    //
    int optimisticIndex = _optimisticIndex; // for thread safe
    if (optimisticIndex != -1 &&
        optimisticIndex < _extendedProperties.Count() &&
        _extendedProperties[optimisticIndex].Id() == id)
    {
        return &_extendedProperties[optimisticIndex];
    }

    //we didn't find the ep optimistically, perform linear lookup
    for (int i = 0; i < _extendedProperties.Count(); i++)
    {
        if (_extendedProperties[i].Id() == id)
        {
            return &_extendedProperties[i];
        }
    }

    return nullptr ;
}

INKCANVAS_END_NAMESPACE
