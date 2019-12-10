#include "Internal/Ink/InkSerializedFormat/serializationhelper.h"

#include <QIODevice>

/// <summary>
/// returns the no of quint8 it requires to mutliquint8 encode a quint32 value
/// </summary>
/// <param name="Value"></param>
/// <returns></returns>
quint32 SerializationHelper::VarSize(quint32 Value)
{
    if (Value < 0x80)
        return 1;
    else if (Value < 0x4000)
        return 2;
    else if (Value < 0x200000)
        return 3;
    else if (Value < 0x10000000)
        return 4;
    else
        return 5;
}


/// <summary>
/// Multiquint8 Encodes an quint32 Value into the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="Value"></param>
/// <returns></returns>
quint32 SerializationHelper::Encode(QIODevice& strm, uint Value)
{
    quint64 ib = 0;

    for (; ; )
    {
        if (Value < 128)
        {
            strm.putChar(static_cast<quint8>(Value));
            return (uint)(ib + 1);
        }

        strm.putChar(static_cast<quint8>(0x0080 | (Value & 0x7f)));
        Value >>= 7;
        ib++;
    }
}


/// <summary>
/// Multiquint8 encodes a unsinged long value in the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="ulValue"></param>
/// <returns></returns>
quint32 SerializationHelper::EncodeLarge(QIODevice& strm, quint64 ulValue)
{
    quint32 ib = 0;

    for (; ; )
    {
        if (ulValue < 128)
        {
            strm.putChar(static_cast<quint8>(ulValue));
            return ib + 1;
        }

        strm.putChar(static_cast<quint8>(0x0080 | (ulValue & 0x7f)));
        ulValue >>= 7;
        ib++;
    }
}


/// <summary>
/// Multiquint8 encodes a signed integer value into a stream. Use 1's complement to
/// store signed values.  This means both 00 and 01 are actually 0, but we don't
/// need to encode all negative numbers as 64 bit values.
/// </summary>
/// <param name="strm"></param>
/// <param name="Value"></param>
/// <returns></returns>
// Use 1's complement to store signed values.  This means both 00 and 01 are
// actually 0, but we don't need to encode all negative numbers as 64 bit values.
quint32 SerializationHelper::SignEncode(QIODevice& strm, int Value)
{
    quint64 ull = 0;

    // special case LONG_MIN
    if (-2147483648 == Value)
    {
        ull = 0x0000000100000001;
    }
    else
    {
        ull = (quint64)qAbs(Value);

        // multiply by 2
        ull <<= 1;

        // For -ve nos, add 1
        if (Value < 0)
            ull |= 1;
    }

    return EncodeLarge(strm, ull);
}

/// <summary>
/// Decodes a multi quint8 encoded unsigned integer from the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="dw"></param>
/// <returns></returns>
quint32 SerializationHelper::Decode(QIODevice& strm, uint& dw)
{
    int shift = 0;
    quint8 b = 0;
    quint32 cb = 0;

    dw = 0;
    do
    {
        strm.getChar((char *)&b);
        cb++;
        dw += (quint32)(b & 0x7f) << shift;
        shift += 7;
    } while (((b & 0x80) > 0) && (shift < 29));

    return cb;
}


/// <summary>
/// Decodes a multiquint8 encoded unsigned long from the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="ull"></param>
/// <returns></returns>
quint32 SerializationHelper::DecodeLarge(QIODevice& strm, quint64& ull)
{
    long ull1;
    int shift = 0;
    quint8 b = 0, a = 0;
    quint32 cb = 0;

    ull = 0;
    do
    {
        strm.getChar((char *)&b);
        cb++;
        a = static_cast<quint8>(b & 0x7f);
        ull1 = a;
        ull |= (quint64)(ull1 << shift);
        shift += 7;
    } while (((b & 0x80) > 0) && (shift < 57));

    return cb;
}


/// <summary>
/// Decodes a multiquint8 encoded signed integer from the stream
/// </summary>
/// <param name="strm"></param>
/// <param name="i"></param>
/// <returns></returns>
quint32 SerializationHelper::SignDecode(QIODevice& strm, int& i)
{
    i = 0;

    quint64 ull = 0;
    quint32 cb = DecodeLarge(strm, ull);

    if (cb > 0)
    {
        bool fneg = false;

        if ((ull & 0x0001) > 0)
            fneg = true;

        ull = ull >> 1;

        long l = (long)ull;

        i = (int)(fneg ? -l : l);
    }

    return cb;
}

/// <summary>
/// Converts the CLR type information into a COM-compatible type enumeration
/// </summary>
/// <param name="type">The CLR type information of the object to convert</param>
/// <param name="throwOnError">Throw an exception if unknown type is used</param>
/// <returns>The COM-compatible type enumeration</returns>
/// <remarks>Only supports the types of data that are supported in ISF ExtendedProperties</remarks>
/*
VarEnum SerializationHelper::ConvertToVarEnum(Type type, bool throwOnError)
{
    if (typeof(char) == type)
    {
        return VarEnum.VT_I1;
    }
    else if (typeof(char[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_I1);
    }
    else if (typeofstatic_cast<quint8> == type)
    {
        return VarEnum.VT_UI1;
    }
    else if (typeof(byte[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_UI1);
    }
    else if (typeof(Int16) == type)
    {
        return VarEnum.VT_I2;
    }
    else if (typeof(Int16[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_I2);
    }
    else if (typeof(UInt16) == type)
    {
        return VarEnum.VT_UI2;
    }
    else if (typeof(UInt16[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_UI2);
    }
    else if (typeof(Int32) == type)
    {
        return VarEnum.VT_I4;
    }
    else if (typeof(Int32[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_I4);
    }
    else if (typeof(UInt32) == type)
    {
        return VarEnum.VT_UI4;
    }
    else if (typeof(UInt32[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_UI4);
    }
    else if (typeof(Int64) == type)
    {
        return VarEnum.VT_I8;
    }
    else if (typeof(Int64[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_I8);
    }
    else if (typeof(UInt64) == type)
    {
        return VarEnum.VT_UI8;
    }
    else if (typeof(UInt64[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_UI8);
    }
    else if (typeof(Single) == type)
    {
        return VarEnum.VT_R4;
    }
    else if (typeof(Single[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_R4);
    }
    else if (typeof(Double) == type)
    {
        return VarEnum.VT_R8;
    }
    else if (typeof(Double[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_R8);
    }
    else if (typeof(DateTime) == type)
    {
        return VarEnum.VT_DATE;
    }
    else if (typeof(DateTime[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_DATE);
    }
    else if (typeof(Boolean) == type)
    {
        return VarEnum.VT_BOOL;
    }
    else if (typeof(Boolean[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_BOOL);
    }
    else if (typeof(String) == type)
    {
        return VarEnum.VT_BSTR;
    }
    else if (typeof(Decimal) == type)
    {
        return VarEnum.VT_DECIMAL;
    }
    else if (typeof(Decimal[]) == type)
    {
        return (VarEnum.VT_ARRAY | VarEnum.VT_DECIMAL);
    }
    else
    {
        if (throwOnError)
        {
            throw new ArgumentException(SR.Get(SRID.InvalidDataTypeForExtendedProperty));
        }
        else
        {
            return VarEnum.VT_UNKNOWN;
        }
    }
}
*/
