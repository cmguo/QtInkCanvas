#ifndef EXTENDEDPROPERTY_H
#define EXTENDEDPROPERTY_H

#include "InkCanvas_global.h"

#include <QUuid>
#include <QVariant>

// namespace System.Windows.Ink
INKCANVAS_BEGIN_NAMESPACE

class ExtendedProperty
{
public:
    static ExtendedProperty Empty;

    /// <summary>
    /// Create a new drawing attribute with the specified key and value
    /// </summary>
    /// <param name="id">Identifier of attribute</param>
    /// <param name="value">Attribute value - not that the Type for value is tied to the id</param>
    /// <exception cref="System.ArgumentException">Value type must be compatible with attribute Id</exception>
    ExtendedProperty(QUuid const & id, QVariant value);

    /// <summary>Returns a value that can be used to store and lookup
    /// ExtendedProperty QVariants in a hash table</summary>
    //public override int GetHashCode()
    //{
    //    return Id.GetHashCode() ^ Value.GetHashCode();
    //}

    /// <summary>Determine if two ExtendedProperty QVariants are equal</summary>
    friend bool operator==(ExtendedProperty const & thiz, ExtendedProperty const & that)
    {
        //if (obj == null || obj.GetType() != GetType())
        //{
        //    return false;
        //}

        //ExtendedProperty that = (ExtendedProperty)obj;

        if (that.Id() == thiz.Id())
        {
            //Type type1 = thiz.Value().userType();
            //Type type2 = that.Value().userType();
/*
            if (type1.IsArray && type2.IsArray)
            {
                Type elementType1 = type1.GetElementType();
                Type elementType2 = type2.GetElementType();
                if (elementType1 == elementType2 &&
                    elementType1.IsValueType &&
                    type1.GetArrayRank() == 1 &&
                    elementType2.IsValueType &&
                    type2.GetArrayRank() == 1)
                {
                    Array array1 = (Array)this.Value;
                    Array array2 = (Array)that.Value;
                    if (array1.Length == array2.Length)
                    {
                        for (int i = 0; i < array1.Length; i++)
                        {
                            if (!array1.GetValue(i).Equals(array2.GetValue(i)))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                }
            }
            else*/
            {
                return that.Value() == thiz.Value();
            }
        }
        return false;
    }

    /// <summary>Overload of the equality operator for comparing
    /// two ExtendedProperty QVariants</summary>
    //static bool operator ==(ExtendedProperty first, ExtendedProperty second)
    //{
    //    if ((QVariant)first == null && (QVariant)second == null)
    //    {
    //        return true;
    //    }
    //    else if ((QVariant)first == null || (QVariant)second == null)
    //    {
    //        return false;
    //    }
    //    else
    //    {
    //        return first.Equals(second);
    //    }
    //}

    /// <summary>Compare two custom attributes for Id and value inequality</summary>
    /// <remarks>Value comparison is performed based on Value.Equals</remarks>
    friend bool operator !=(ExtendedProperty const & first, ExtendedProperty const & second)
    {
        return !(first == second);
    }

    /// <summary>
    /// Returns a debugger-friendly version of the ExtendedProperty
    /// </summary>
    /// <returns></returns>
    QString ToString();

    /// <summary>
    /// Retrieve the Identifier, or key, for Drawing Attribute key/value pair
    /// </summary>
    QUuid const & Id() const
    {
        return _id;
    }
    /// <summary>
    /// Set or retrieve the value for ExtendedProperty key/value pair
    /// </summary>
    /// <exception cref="System.ArgumentException">Value type must be compatible with attribute Id</exception>
    /// <remarks>Value can be null.</remarks>
    QVariant const & Value() const
    {
       return _value;
    }
    void SetValue(QVariant const & value)
    {
        if (value.isNull())
        {
            throw std::runtime_error("value");
        }

        // validate the type information for the id against the id
        //ExtendedPropertySerializer.Validate(_id, value);

        _value = value;
    }

private:
    ExtendedProperty();

    /// <summary>
    /// Creates a copy of the QUuid and Value
    /// </summary>
    /// <returns></returns>
    /*
    ExtendedProperty Clone()
    {
        //
        // the only properties we accept are value types or arrays of
        // value types with the exception of string.
        //
        QUuid QUuid = _id; //QUuid is a ValueType that copies on assignment
        Type type = _value.GetType();

        //
        // check for the very common, copy on assignment
        // types (ValueType or string)
        //
        if (type.IsValueType || type == typeof(string))
        {
            //
            // either ValueType or string is passed by value
            //
            return new ExtendedProperty(QUuid, _value);
        }
        else if (type.IsArray)
        {
            Type elementType = type.GetElementType();
            if (elementType.IsValueType && type.GetArrayRank() == 1)
            {
                //
                // copy the array memebers, which we know are copy
                // on assignment value types
                //
                Array newArray = Array.CreateInstance(elementType, ((Array)_value).Length);
                Array.Copy((Array)_value, newArray, ((Array)_value).Length);
                return new ExtendedProperty(QUuid, newArray);
            }
        }
        //
        // we didn't find a type we expect, throw
        //
        //throw new InvalidOperationException(SR.Get(SRID.InvalidDataTypeForExtendedProperty));
    }
    */

private:
    QUuid _id;                // id of attribute
    QVariant _value;             // data in attribute
};

INKCANVAS_END_NAMESPACE

#endif // EXTENDEDPROPERTY_H
