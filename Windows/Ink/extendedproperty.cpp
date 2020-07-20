#include "Windows/Ink/extendedproperty.h"
#include "Windows/Ink/knownids.h"

INKCANVAS_BEGIN_NAMESPACE

ExtendedProperty ExtendedProperty::Empty;

/// <summary>
/// Create a new drawing attribute with the specified key and value
/// </summary>
/// <param name="id">Identifier of attribute</param>
/// <param name="value">Attribute value - not that the Type for value is tied to the id</param>
/// <exception cref="System.ArgumentException">Value type must be compatible with attribute Id</exception>
ExtendedProperty::ExtendedProperty(Guid const & id, Variant const & value)
{
    if (id== Guid::Empty)
    {
        throw std::runtime_error("(SR.Get(SRID.InvalidGuid)");
    }
    _id = id;
    _value = value;
}

ExtendedProperty::ExtendedProperty()
{
}

#ifdef INKCANVAS_QT_VARIANT

QString ExtendedProperty::ToString()
{
    QString val;
    //if (Value == null)
    //{
    //    val = "<undefined value>";
    //}
    //else if (Value is string)
    //{
    //    val = "\"" + Value.ToString() + "\"";
    //}
    //else
    //{
    //    val = Value.ToString();
    //}
    return KnownIds::ConvertToString(Id()) + "," + Value().toString();
}

#endif

INKCANVAS_END_NAMESPACE
