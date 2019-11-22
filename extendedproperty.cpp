#include "extendedproperty.h"
#include "knownids.h"

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
