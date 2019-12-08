#ifndef EXTENDEDPROPERTYCOLLECTION_H
#define EXTENDEDPROPERTYCOLLECTION_H

#include "Windows/Ink/extendedproperty.h"

#include <QUuid>
#include <QVariant>
#include <QObject>

class ExtendedPropertiesChangedEventArgs;

/// <summary>
/// A collection of name/value pairs, called ExtendedProperties, can be stored
/// in a collection to enable aggregate operations and assignment to Ink object
/// model objects, such StrokeCollection and Stroke.
/// </summary>
class ExtendedPropertyCollection : public QObject //does not implement ICollection, we don't need it
{
    Q_OBJECT
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
    bool Contains(QUuid const & attributeId);

    /// <summary>
    /// Copies the ExtendedPropertyCollection
    /// </summary>
    /// <returns>Copy of the ExtendedPropertyCollection</returns>
    /// <remarks>Any reference types held in the collection will only be deep copied (e.g. Arrays).
    /// </remarks>
    ExtendedPropertyCollection* Clone();

    /// <summary>
    /// Add
    /// </summary>
    /// <param name="id">Id</param>
    /// <param name="value">value</param>
    void Add(QUuid const& id, QVariant value);

    /// <summary>
    /// Remove
    /// </summary>
    /// <param name="id">id</param>
    void Remove(QUuid const& id);

    /// <value>
    ///     Retrieve the QUuid const& array of ExtendedProperty Ids  in the collection.
    ///     <paramref>Guid[]</paramref> is of type <see cref="System.Int32"/>.
    ///     <seealso cref="System.Collections.ICollection.Count"/>
    /// </value>
    QVector<QUuid> GetGuidArray();

    /// <summary>
    /// Generic accessor for the ExtendedPropertyCollection.
    /// </summary>
    /// <param name="attributeId">Attribue Id to find</param>
    /// <returns>Value for attribute specified by Id</returns>
    /// <exception cref="System.ArgumentException">Specified identifier was not found</exception>
    /// <remarks>
    /// Note that you can access extended properties via this indexer.
    /// </remarks>
    QVariant operator[](QUuid const& attributeId);
    void Set(QUuid const& attributeId, QVariant value);

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
        return _extendedProperties.size();
    }

signals:
    /// <summary>
    /// Event fired whenever a ExtendedProperty is modified in the collection
    /// </summary>
    void Changed(ExtendedPropertiesChangedEventArgs&);

private:
    /// <summary>
    /// private Add, we need to consider making this public in order to implement the generic ICollection
    /// </summary>
    void Add(ExtendedProperty const & extendedProperty);

    /// <summary>
    /// Private helper for getting an EP out of our internal collection
    /// </summary>
    /// <param name="id">id</param>
    ExtendedProperty* GetExtendedPropertyById(QUuid const& id);

    // the set of ExtendedProperties stored in this collection
private:
    QList<ExtendedProperty> _extendedProperties;


    //used to optimize across Contains / Index calls
    int _optimisticIndex = -1;
};

#endif // EXTENDEDPROPERTYCOLLECTION_H
