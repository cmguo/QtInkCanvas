#ifndef EXTENDEDPROPERTYCOLLECTION_H
#define EXTENDEDPROPERTYCOLLECTION_H

#include "Windows/Ink/extendedproperty.h"
#include "Collections/Generic/list.h"
#include "Collections/Generic/array.h"

#ifdef INKCANVAS_QT
#include <QObject>
#endif

INKCANVAS_BEGIN_NAMESPACE

class ExtendedPropertiesChangedEventArgs;

/// <summary>
/// A collection of name/value pairs, called ExtendedProperties, can be stored
/// in a collection to enable aggregate operations and assignment to Ink object
/// model objects, such StrokeCollection and Stroke.
/// </summary>
#ifdef INKCANVAS_QT
class ExtendedPropertyCollection : public QObject //does not implement ICollection, we don't need it
{
    Q_OBJECT
#else
class ExtendedPropertyCollection //does not implement ICollection, we don't need it
{
#endif
public:
    /// <summary>
    /// Create a new empty ExtendedPropertyCollection
    /// </summary>
    ExtendedPropertyCollection();

    /// <summary>Overload of the Equals method which determines if two ExtendedPropertyCollection
    /// objects contain equivalent key/value pairs</summary>
    bool Equals(ExtendedPropertyCollection const & that) const;

    /// <summary>Overload of the equality operator which determines
    /// if two ExtendedPropertyCollections are equal</summary>
    friend bool operator ==(ExtendedPropertyCollection const & first, ExtendedPropertyCollection const & second)
    {
        return first.Equals(second);
    }

    /// <summary>Overload of the not equals operator to determine if two
    /// ExtendedPropertyCollections have different key/value pairs</summary>
    friend bool operator!=(ExtendedPropertyCollection const & first, ExtendedPropertyCollection const & second)
    {
        return !(first == second);
    }

    /// <summary>
    /// GetHashCode
    /// </summary>
    //int GetHashCode()
    //{
    //    return base.GetHashCode();
    //}

    /// <summary>
    /// Check to see if the attribute is defined in the collection.
    /// </summary>
    /// <param name="attributeId">Attribute identifier</param>
    /// <returns>True if attribute is set in the mask, false otherwise</returns>
    bool Contains(Guid const & attributeId) const;

    /// <summary>
    /// Copies the ExtendedPropertyCollection
    /// </summary>
    /// <returns>Copy of the ExtendedPropertyCollection</returns>
    /// <remarks>Any reference types held in the collection will only be deep copied (e.g. Arrays).
    /// </remarks>
    ExtendedPropertyCollection* Clone() const;

    /// <summary>
    /// Add
    /// </summary>
    /// <param name="id">Id</param>
    /// <param name="value">value</param>
    void Add(Guid const& id, Variant const & value);

    /// <summary>
    /// Remove
    /// </summary>
    /// <param name="id">id</param>
    void Remove(Guid const& id);

    /// <value>
    ///     Retrieve the Guid const& array of ExtendedProperty Ids  in the collection.
    ///     <paramref>Guid[]</paramref> is of type <see cref="System.Int32"/>.
    ///     <seealso cref="System.Collections.ICollection.Count"/>
    /// </value>
    Array<Guid> GetGuidArray() const;

    /// <summary>
    /// Generic accessor for the ExtendedPropertyCollection.
    /// </summary>
    /// <param name="attributeId">Attribue Id to find</param>
    /// <returns>Value for attribute specified by Id</returns>
    /// <exception cref="System.ArgumentException">Specified identifier was not found</exception>
    /// <remarks>
    /// Note that you can access extended properties via this indexer.
    /// </remarks>
    Variant operator[](Guid const& attributeId) const;
    void Set(Guid const& attributeId, Variant const & value);

    /// <summary>
    /// Generic accessor for the ExtendedPropertyCollection.
    /// </summary>
    /// <param name="index">index into masking collection to retrieve</param>
    /// <returns>ExtendedProperty specified at index</returns>
    /// <exception cref="System.ArgumentOutOfRangeException">Index was not found</exception>
    /// <remarks>
    /// Note that you can access extended properties via this indexer.
    /// </remarks>
    ExtendedProperty& operator[](int index)
    {
        return _extendedProperties[index];
    }

    ExtendedProperty const & operator[](int index) const
    {
        return _extendedProperties[index];
    }

    /// <value>
    ///     Retrieve the number of ExtendedProperty objects in the collection.
    ///     <paramref>Count</paramref> is of type <see cref="System.Int32"/>.
    ///     <seealso cref="System.Collections.ICollection.Count"/>
    /// </value>
    int Count() const
    {
        return _extendedProperties.Count();
    }

#ifdef INKCANVAS_QT
signals:
    /// <summary>
    /// Event fired whenever a ExtendedProperty is modified in the collection
    /// </summary>
    void Changed(ExtendedPropertiesChangedEventArgs&);
#endif

private:
    /// <summary>
    /// private Add, we need to consider making this public in order to implement the generic ICollection
    /// </summary>
    void Add(ExtendedProperty const & extendedProperty);

    /// <summary>
    /// Private helper for getting an EP out of our internal collection
    /// </summary>
    /// <param name="id">id</param>
    ExtendedProperty* GetExtendedPropertyById(Guid const& id);

    // the set of ExtendedProperties stored in this collection
private:
    List<ExtendedProperty> _extendedProperties;


    //used to optimize across Contains / Index calls
    mutable int _optimisticIndex = -1;
};

INKCANVAS_END_NAMESPACE

#endif // EXTENDEDPROPERTYCOLLECTION_H
